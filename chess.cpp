/*
 * $Id: chess.cpp 1336 2014-12-08 09:29:59Z justin $
 * Copyright (C) 2009 Lucid Fusion Labs

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/app/app.h"
#include "core/app/gui.h"
#include "core/app/ipc.h"
#include "core/app/net/resolver.h"
#include "chess.h"
#include "fics.h"
#ifdef LFL_FLATBUFFERS
#include "term/term_generated.h"
#endif
#include "term/term.h"

namespace LFL {
DEFINE_string(connect, "freechess.org:5000", "Connect to server");
DEFINE_bool(click_or_drag_pieces, MOBILE, "Move by clicking or dragging");
DEFINE_bool(auto_close_old_games, true, "Close old games whenever new game starts");
DEFINE_string(seek_command, "5 0", "Seek command");
DEFINE_string(engine, "", "Chess engine");

struct MyAppState {
  point initial_board_dim = point(630, 630);
  point initial_term_dim = point(initial_board_dim.x / Fonts::InitFontWidth(), 10);
  unique_ptr<SystemAlertView> askseek, askresign;
  unique_ptr<SystemMenuView> editmenu, viewmenu, gamemenu;
  unique_ptr<SystemToolbarView> maintoolbar;
} *my_app;

struct ChessTerminalTab : public TerminalTabT<ChessTerminal> {
  using TerminalTabT::TerminalTabT;
  virtual bool Animating() const { return false; }
  virtual void UpdateTargetFPS() {}
  virtual void SetFontSize(int) {}
};

struct ChessGUI : public GUI {
  unique_ptr<UniversalChessInterfaceEngine> chess_engine;
  unique_ptr<ChessTerminalTab> chess_terminal;
  point term_dim=my_app->initial_term_dim;
  v2 square_dim;
  Box win, board, term;
  Widget::Divider divider;
  Chess::Game *top_game=0;
  unordered_map<int, Chess::Game> game_map;
  bool title_changed = 0, console_animating = 0;
  Time move_animation_time = Time(200);
  DragTracker drag_tracker;
  ChessGUI(Window *W) : GUI(W), divider(this, true, W->width) { (top_game = &game_map[0])->active = 0; }

  /**/  Chess::Game *Top()       { return top_game; }
  const Chess::Game *Top() const { return top_game; }

  Box SquareCoords(int p, bool flip_board) const {
    int sx = Chess::SquareX(p), sy = Chess::SquareY(p);
    return Box(board.x + (flip_board ? (7-sx) : sx) * square_dim.x,
               board.y + (flip_board ? (7-sy) : sy) * square_dim.y, square_dim.x, square_dim.y, true);
  }

  int SquareFromCoords(const point &p, bool flip_board) const {
    int sx = p.x / square_dim.x, sy = p.y / square_dim.y;
    return Chess::SquareFromXY(flip_board ? (7-sx) : sx, flip_board ? (7-sy) : sy);
  }

  void Open(const string &hostport) {
    Activate();
    if (hostport.size()) {
      INFO("connecting to ", hostport);
      auto c = make_unique<NetworkTerminalController>
        (chess_terminal.get(), hostport, bind(&ChessGUI::ClosedCB, this));
      c->frame_on_keyboard_input = true;
      chess_terminal->ChangeController(move(c));
    }

    auto t = chess_terminal->terminal;
    t->touch_toggles_keyboard = true;
    t->controller      = chess_terminal->controller.get();
    t->get_game_cb     = [=](int game_no){ return &game_map[game_no]; };
    t->illegal_move_cb = bind(&ChessGUI::IllegalMoveCB, this);
    t->login_cb        = bind(&ChessGUI::LoginCB,       this);
    t->game_start_cb   = bind(&ChessGUI::GameStartCB,   this, _1);
    t->game_over_cb    = bind(&ChessGUI::GameOverCB,    this, _1, _2, _3, _4);
    t->game_update_cb  = bind(&ChessGUI::GameUpdateCB,  this, _1, _2, _3, _4);
    t->SetColors(Singleton<Terminal::StandardVGAColors>::Get());
  }

  void Send(const string &b) {
    ChessTerminal *t;
    if ((t = chess_terminal->terminal) && t->controller) t->Send(b);
    else if (auto e = chess_engine.get()) {}
  }

  void LoadPosition(const vector<string> &arg) {
    auto g = Top();
    if (!g || !arg.size()) return;
    if (arg.size() == 1) {
      if      (arg[0] == "kiwipete")  g->last_position.LoadByteBoard(Chess::kiwipete_byte_board);
      else if (arg[0] == "perftpos3") g->last_position.LoadFEN      (Chess::perft_pos3_fen);
    } else if (g->last_position.LoadFEN(Join(arg, " "))) {}
    g->position = g->last_position;
  }

  void ListGames() { for (auto &i : game_map) INFO(i.first, " ", i.second.p1_name, " vs ", i.second.p2_name); }
  void FlipBoard(Window *w) { if (auto g = Top()) g->flip_board = !g->flip_board; app->scheduler.Wakeup(w); }
  void UpdateAnimating(Window *w) { app->scheduler.SetAnimating(w, (Top() && Top()->move_animate_from != -1) | console_animating); }

  void ConsoleAnimatingCB() {
    console_animating = root ? root->console->animating : 0;
    UpdateAnimating(root);
  }

  void ClosedCB() { chess_terminal->terminal->Write("\r\nConnection closed.\r\n"); }
  void LoginCB() { root->SetCaption(StrCat(chess_terminal->terminal->my_name, " @ ", FLAGS_connect)); }

  void GameStartCB(int game_no) {
    Chess::Game *game = top_game = &game_map[game_no];
    game->Reset();
    game->game_number = game_no;
    if (FLAGS_enable_audio) {
      static SoundAsset *start_sound = app->soundasset("start");
      app->PlaySoundEffect(start_sound);
    }
    if (FLAGS_auto_close_old_games) FilterValues<unordered_map<int, Chess::Game>>
      (&game_map, [](const pair<const int, Chess::Game> &x){ return !x.second.active; });
  }

  void GameOverCB(int game_no, const string &p1, const string &p2, const string &result) {
    const string &my_name = chess_terminal->terminal->my_name;
    bool lose = (my_name == p1 && result == "0-1") || (my_name == p2 && result == "1-0");
    if (FLAGS_enable_audio) {
      static SoundAsset *win_sound = app->soundasset("win"), *lose_sound = app->soundasset("lose");
      app->PlaySoundEffect(lose ? lose_sound : win_sound);
    }
    game_map[game_no].active = false;
  }

  void GameUpdateCB(Chess::Game *game, bool reapply_premove = false, int animate_from = -1, int animate_to = -1) {
    top_game = game;
    title_changed = true;
    if (FLAGS_enable_audio) {
      static SoundAsset *move_sound = app->soundasset("move"), *capture_sound = app->soundasset("capture");
      app->PlaySoundEffect(Chess::GetMoveCapture(game->position.move) ? capture_sound : move_sound);
    }
    if (game->position.move_number == 0) {
      game->my_color = chess_terminal->terminal->my_name == game->p1_name ? Chess::WHITE : Chess::BLACK;
      game->flip_board = game->my_color == Chess::BLACK;
    }
    if (reapply_premove) ReapplyPremoves(game);
    if (animate_from != -1 && animate_to != -1 &&
        Chess::GetPieceType((game->animating_piece = game->position.ClearSquare(animate_to)))) {
      game->move_animate_from = animate_from;
      game->move_animate_to = animate_to;
      game->move_animation_start = Now();
    } else game->move_animate_from = -1;
    UpdateAnimating(root);
  }

  void IllegalMoveCB() {
    if (FLAGS_enable_audio) {
      static SoundAsset *illegal_sound = app->soundasset("illegal");
      app->PlaySoundEffect(illegal_sound);
    }
  }

  void ClickCB(int button, int x, int y, int down) {
    Chess::Game *game = Top();
    if (!game || !down) return;
    point p = point(x, y) - board.Position();
    int square, start_square;
    Chess::Piece moved_piece=0;
    if ((square = SquareFromCoords(p, game->flip_board)) < 0) return;
    if (!Chess::GetPieceType(game->moving_piece)) {
      drag_tracker.beg_click = drag_tracker.end_click = p;
      game->moving_piece = game->position.ClearSquare(square);
      return;
    }
    swap(game->moving_piece, moved_piece);
    game->position.SetSquare(square, moved_piece);
    if ((start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board)) == square) return;
    MakeMove(game, Chess::GetPieceType(moved_piece), start_square, square);
  }

  void DragCB(int button, int x, int y, int down) {
    Chess::Game *game = Top();
    if (!game) return;
    point p = point(x, y) - board.Position();
    int square, start_square;
    Chess::Piece moved_piece=0;
    if ((square = SquareFromCoords(p, game->flip_board)) < 0) return;
    if (drag_tracker.Update(p, down)) game->moving_piece = game->position.ClearSquare(square);
    if (!Chess::GetPieceType(game->moving_piece) || down) return;
    swap(game->moving_piece, moved_piece);
    game->position.SetSquare(square, moved_piece);
    if ((start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board)) == square) return;
    MakeMove(game, Chess::GetPieceType(moved_piece), start_square, square);
  }

  void MakeMove(Chess::Game *game, int piece, int start_square, int end_square, bool animate=false) {
    ChessTerminal *t=0;
    if ((t = chess_terminal->terminal) && t->controller) {
      string move = StrCat(Chess::PieceChar(piece), Chess::SquareName(start_square), Chess::SquareName(end_square));
      if (game->position.flags.to_move_color == game->my_color) t->MakeMove(move);
      else {
        auto &pm = PushBack(game->premove, game->position);
        pm.name = move;
        pm.move = Chess::GetMove(piece, start_square, end_square, 0, 0, 0);
      }
    } else if (auto e = chess_engine.get()) {
      bool move_color = game->position.flags.to_move_color;
      if (Chess::GetPieceColor(game->last_position.GetSquare(start_square)) != move_color ||
          !(Chess::SquareMask(end_square) & game->last_position.PieceMoves
            (piece, start_square, move_color, game->last_position.AllAttacks(!move_color)))) {
        IllegalMoveCB();
        game->position = game->last_position;
      } else {
        if (game->history.empty()) {
          chess_terminal->terminal->my_name = game->p1_name = "Player1";
          game->p2_name = "Player2"; 
        }
        game->active = true;
        game->update_time = Now();
        game->position.move_number++;
        game->position.flags.to_move_color = game->position.move_number % 2;
        bool en_passant = piece == Chess::PAWN && Chess::SquareX(start_square) != Chess::SquareX(end_square) &&
          !Chess::GetPieceType(game->last_position.GetSquare(end_square));
        uint8_t capture_square = en_passant ? (end_square + 8 * (move_color ? 1 : -1)) : end_square, promotion = 0;
        Chess::Piece capture = game->last_position.GetSquare(capture_square);
        if (capture) game->position.ClearSquare(capture_square, move_color != Chess::WHITE, move_color != Chess::BLACK);
        if (piece == Chess::KING && abs(Chess::SquareX(end_square) - Chess::SquareX(start_square)) > 1) 
          game->position.UpdateCastles(move_color, end_square);
        if (piece == Chess::PAWN && Chess::SquareY(end_square) == (move_color ? 0 : 7)) {
          promotion = Chess::QUEEN;
          game->position.ClearSquare(end_square, move_color == Chess::WHITE, move_color == Chess::BLACK);
          game->position.SetSquare(end_square, Chess::GetPiece(move_color, promotion));
        }
        game->position.name = StrCat(Chess::PieceChar(piece), Chess::SquareName(start_square), Chess::SquareName(end_square));
        game->position.UpdateMove(true, piece, start_square, end_square, capture, promotion, en_passant ? Chess::MoveFlag::EnPassant : 0);
        game->AddNewMove();
        GameUpdateCB(game, animate, animate ? start_square : -1, animate ? end_square : -1);
      }
    }
  }

  void Reshaped() { divider.size = root->width; }
  void Layout() {
    Font *font = chess_terminal->terminal->style.font;
    ResetGUI();
    win = root->Box();
    term.w = win.w;
    term_dim.x = win.w / font->FixedWidth();
    int min_term_h = font->Height() * 3;
    divider.max_size = min(win.w, win.h - min_term_h);
    divider.LayoutDivideTop(win, &win, &term);
    CHECK_LE(win.h, win.w);
    if (int d = win.w - win.h) { win.w -= d; win.x += d/2; }
    CHECK_EQ(win.w, win.h);
    board = Box::DelBorder(win, Border(5,5,5,5));
    square_dim = v2(board.w/8.0, board.h/8.0);
    if (FLAGS_click_or_drag_pieces) mouse.AddClickBox(board, MouseController::CoordCB(bind(&ChessGUI::ClickCB, this, _1, _2, _3, _4)));
    else                            mouse.AddDragBox (board, MouseController::CoordCB(bind(&ChessGUI::DragCB,  this, _1, _2, _3, _4)));

    Texture *board_tex = &app->asset("board1")->tex;
    child_box.PushBack(win, Drawable::Attr(board_tex), board_tex);
  }

  int Frame(LFL::Window *W, unsigned clicks, int flag) {
    Chess::Game *game = Top();
    Time now = Now();
    GraphicsContext gc(W->gd);

    if (game && (title_changed || game->active)) {
      if (game && game->active) {
        int secs = game->position.move_number ? ToSeconds(now - game->update_time).count() : 0;
        game->last_p1_secs = game->p1_secs - (game->position.flags.to_move_color ? 0 : secs);
        game->last_p2_secs = game->p2_secs - (game->position.flags.to_move_color ? secs : 0);
      }
      string title = StringPrintf("%s %d:%02d vs %s %d:%02d",
                                 game->p1_name.c_str(), game->last_p1_secs/60, game->last_p1_secs%60,
                                 game->p2_name.c_str(), game->last_p2_secs/60, game->last_p2_secs%60);
      if (game->position.move_number)
        StringAppendf(&title, ": %d%s: %s", 
                      game->position.StandardMoveNumber(),
                      game->position.StandardMoveSuffix(), game->position.name.c_str());

      W->SetCaption(title);
      title_changed = false;
    }

    if (divider.changed) Layout();
    if (win.w != W->width) { ScopedFillColor sfc(W->gd, Color::grey70); gc.DrawTexturedBox(Box(W->x, win.y, W->width, win.h)); }
    Draw();
    DrawGame(W, game ? game : Singleton<Chess::Game>::Get(), now);

    W->gd->DisableBlend();
    { Scissor s(W->gd, term); chess_terminal->terminal->Draw(term); }
    if (divider.changing) BoxOutline().Draw(&gc, Box::DelBorder(term, Border(1,1,1,1)));

    W->DrawDialogs();
    return 0;
  }

  void DrawGame(Window *W, Chess::Game *game, Time now) {
    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = app->fonts->Get("ChessPieces1");
    Drawable::Attr draw_attr(pieces);
    GraphicsContext gc(W->gd);
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(game->position.white[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(W->gd, black_font_index[i]+6, SquareCoords(*b, game->flip_board));
      Bit::Indices(game->position.black[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(W->gd, black_font_index[i],   SquareCoords(*b, game->flip_board));
    }

    if (game->position.move_number) {
      W->gd->SetColor(Color(85, 85,  255)); BoxOutline().Draw(&gc, SquareCoords(Chess::GetMoveFromSquare(game->position.move), game->flip_board));
      W->gd->SetColor(Color(85, 255, 255)); BoxOutline().Draw(&gc, SquareCoords(Chess::GetMoveToSquare  (game->position.move), game->flip_board));
    }

    for (auto &pm : game->premove) {
      W->gd->SetColor(Color(255, 85,  85)); BoxOutline().Draw(&gc, SquareCoords(Chess::GetMoveFromSquare(pm.move), game->flip_board));
      W->gd->SetColor(Color(255, 255, 85)); BoxOutline().Draw(&gc, SquareCoords(Chess::GetMoveToSquare  (pm.move), game->flip_board));
    }

    if (auto piece_type = Chess::GetPieceType(game->moving_piece)) {
      bool piece_color = Chess::GetPieceColor(game->moving_piece);
      int start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board);
      Chess::BitBoard moves = game->position.PieceMoves(piece_type, start_square, piece_color, game->position.AllAttacks(!piece_color));
      Bit::Indices(moves, bits);
      W->gd->SetColor(Color(255, 85, 255));
      for (int *b = bits; *b != -1; b++)   BoxOutline().Draw(&gc, SquareCoords(*b, game->flip_board));
      W->gd->SetColor(Color(170, 0, 170)); BoxOutline().Draw(&gc, SquareCoords(start_square, game->flip_board));
      W->gd->SetColor(Color::white);

      int glyph_index = black_font_index[piece_type] + 6*(!piece_color);
      pieces->DrawGlyph(W->gd, glyph_index, SquareCoords(start_square, game->flip_board) + (drag_tracker.end_click - drag_tracker.beg_click));
    } else W->gd->SetColor(Color::white);

    if (game->move_animate_from != -1) {
      if (game->move_animation_start + move_animation_time < now) {
        game->position.SetSquare(game->move_animate_to, game->animating_piece);
        game->move_animate_from = -1;
        UpdateAnimating(W);
      } 
      int glyph_index = black_font_index[Chess::GetPieceType(game->animating_piece)] +
        6*(!Chess::GetPieceColor(game->animating_piece));
      Box start_square = SquareCoords(game->move_animate_from, game->flip_board);
      Box end_square   = SquareCoords(game->move_animate_to,   game->flip_board);
      float percent = min(1.0f, float((now - game->move_animation_start).count()) / move_animation_time.count());
      point slope(end_square.centerX() - start_square.centerX(), end_square.centerY() - start_square.centerY());
      point pos = start_square.center() + slope * percent;
      pieces->DrawGlyph(W->gd, glyph_index, Box(pos.x - start_square.w/2, pos.y - start_square.h/2, start_square.w, start_square.h));
    }
  }

  void WalkHistory(bool backwards) {
    Chess::Game *game = Top();
    if (!game || !game->history.size()) return;
    int last_history_ind = game->history_ind, ind;
    if (backwards) game->history_ind = min<int>(game->history.size() - 1, game->history_ind + 1);
    else           game->history_ind = max<int>(0,                        game->history_ind - 1);
    if (game->history_ind == last_history_ind) return;
    game->position = game->history[game->history.size()-1-game->history_ind];

    if (!backwards) GameUpdateCB(game, !game->history_ind, Chess::GetMoveFromSquare(game->position.move), Chess::GetMoveToSquare(game->position.move));
    else if (Clamp<int>(last_history_ind, 0, game->history.size()-1) == last_history_ind) {
      auto &last_position = game->history[game->history.size()-1-last_history_ind];
      GameUpdateCB(game, !game->history_ind, Chess::GetMoveToSquare(last_position.move), Chess::GetMoveFromSquare(last_position.move));
    }
  }

  void ReapplyPremoves(Chess::Game *game) {
    for (auto &pm : game->premove) {
      auto pm_piece = game->position.ClearSquare(Chess::GetMoveFromSquare(pm.move));
      game->position.SetSquare(Chess::GetMoveToSquare(pm.move), pm_piece);
      pm.Assign(game->position);
    }
  }

  void UndoPremove(Window *W) {
    Chess::Game *game = Top();
    if (!game) return;
    game->history_ind = 0;
    if (game->premove.size()) game->premove.pop_back();
    if (!game->premove.size() && !game->history.size()) return;
    game->position = game->premove.size() ? game->premove.back() : game->history.back();
    GameUpdateCB(game);
    app->scheduler.Wakeup(W);
  }

  void CopyPGNToClipboard() {
    Chess::Game *game = Top();
    if (!game) return;
    string text = StrCat("[Site ", FLAGS_connect.substr(0, FLAGS_connect.find(":")), "]\n[Date: ",
                         logfileday(Now()), "\n[White \"", game->p1_name, "\"]\n[Black \"", game->p2_name, "\"]\n\n");
    if (game->history.size())
      for (auto b = game->history.begin()+1, e = game->history.end(), i = b; i != e; ++i)
        StrAppend(&text, ((i - b) % 2) ? "" : StrCat((i-b)/2+1, ". "), i->name, " ");
    StrAppend(&text, "\n");
    app->SetClipboardText(text);
  }

  void StartEngine(bool black_or_white) {
    bool started = false;
    Chess::Game *game = Top();
    if (!chess_engine || game->position.flags.to_move_color != black_or_white) return;
    if (black_or_white) started = Changed(&game->engine_playing_black, true);
    else                started = Changed(&game->engine_playing_white, true);
    // if (!started) return;
    chess_engine->Analyze(game, [=](int square_from, int square_to){
      game->position = game->last_position;                    
      auto piece = game->position.ClearSquare(square_from);
      if (auto piece_type = Chess::GetPieceType(piece)) {
        game->position.SetSquare(square_to, piece);
        MakeMove(game, piece_type, square_from, square_to, true);
      }
      // INFO("bestmove ", Chess::SquareName(square_from), " ", Chess::SquareName(square_to));
    });
  }
};

void MyWindowInit(Window *W) {
  W->caption = "Chess";
  W->width = my_app->initial_board_dim.x;
  W->height = my_app->initial_board_dim.y + my_app->initial_term_dim.y * Fonts::InitFontHeight();
}

void MyWindowStart(Window *W) {
  if (FLAGS_console) W->InitConsole(Callback());
  ChessGUI *chess_gui = W->AddGUI(make_unique<ChessGUI>(W));
  chess_gui->chess_terminal = make_unique<ChessTerminalTab>
    (W, W->AddGUI(make_unique<FICSTerminal>(nullptr, W, W->default_font, chess_gui->term_dim)));

  W->reshaped_cb = bind(&ChessGUI::Reshaped, chess_gui);
  W->frame_cb = bind(&ChessGUI::Frame, chess_gui, _1, _2, _3);
  W->default_textbox = [=]{ return app->run ? chess_gui->chess_terminal->terminal : nullptr; };
  if (FLAGS_console) W->console->animating_cb = bind(&ChessGUI::ConsoleAnimatingCB, chess_gui);

  W->shell = make_unique<Shell>(W);
  W->shell->Add("games", bind(&ChessGUI::ListGames,    chess_gui));
  W->shell->Add("load",  bind(&ChessGUI::LoadPosition, chess_gui, _1));

  BindMap *binds = W->AddInputController(make_unique<BindMap>());
  binds->Add(Key::Escape,                    Bind::CB(bind(&Shell::quit,    W->shell.get(), vector<string>())));
  binds->Add('6',        Key::Modifier::Cmd, Bind::CB(bind(&Shell::console, W->shell.get(), vector<string>())));
  binds->Add(Key::Up,    Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->terminal->ScrollUp();   app->scheduler.Wakeup(W); })));
  binds->Add(Key::Down,  Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->terminal->ScrollDown(); app->scheduler.Wakeup(W); })));
  binds->Add(Key::Left,  Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->WalkHistory(1); app->scheduler.Wakeup(W); })));
  binds->Add(Key::Right, Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->WalkHistory(0); app->scheduler.Wakeup(W); })));

  if (FLAGS_engine.size()) {
    chess_gui->chess_engine = make_unique<UniversalChessInterfaceEngine>();
    CHECK(chess_gui->chess_engine->Start(Asset::FileName(FLAGS_engine), W));
  }
}

}; // namespace LFL
using namespace LFL;

extern "C" void MyAppCreate(int argc, const char* const* argv) {
  FLAGS_enable_video = FLAGS_enable_audio = FLAGS_enable_input = FLAGS_enable_network = FLAGS_console = 1;
  FLAGS_console_font = "Nobile.ttf";
  FLAGS_console_font_flag = 0;
  FLAGS_peak_fps = 20;
  FLAGS_target_fps = 0;
  app = new Application(argc, argv);
  app->focused = new Window();
  my_app = new MyAppState();
  app->name = "LChess";
  app->window_start_cb = MyWindowStart;
  app->window_init_cb = MyWindowInit;
  app->window_init_cb(app->focused);
  app->exit_cb = [](){ delete my_app; };
#ifdef LFL_MOBILE
  app->SetExtraScale(true);
  app->SetTitleBar(true);
  app->SetKeepScreenOn(false);
  app->SetAutoRotateOrientation(true);
  app->CloseTouchKeyboardAfterReturn(false);
#endif
}

extern "C" int MyAppMain() {
  if (app->Create(__FILE__)) return -1;
#ifdef WIN32
  app->asset_cache["default.vert"]                                    = app->LoadResource(200);
  app->asset_cache["default.frag"]                                    = app->LoadResource(201);
  app->asset_cache["MenuAtlas,0,255,255,255,0.0000.glyphs.matrix"]    = app->LoadResource(202);
  app->asset_cache["MenuAtlas,0,255,255,255,0.0000.png"]              = app->LoadResource(203);
  app->asset_cache["ChessPieces1,0,255,255,255,0.0000.glyphs.matrix"] = app->LoadResource(206);
  app->asset_cache["ChessPieces1,0,255,255,255,0.0000.png"]           = app->LoadResource(207);
  app->asset_cache["board1.png"]                                      = app->LoadResource(208);
  app->asset_cache["capture.wav"]                                     = app->LoadResource(209);
  app->asset_cache["illegal.wav"]                                     = app->LoadResource(210);
  app->asset_cache["lose.wav"]                                        = app->LoadResource(211);
  app->asset_cache["move.wav"]                                        = app->LoadResource(212);
  app->asset_cache["start.wav"]                                       = app->LoadResource(213);
  app->asset_cache["win.wav"]                                         = app->LoadResource(214);
  if (FLAGS_console) {
    app->asset_cache["Nobile.ttf,32,255,255,255,0.0000.glyphs.matrix"] = app->LoadResource(204);
    app->asset_cache["Nobile.ttf,32,255,255,255,0.0000.png"]           = app->LoadResource(205);
  }
#endif
  if (app->Init()) return -1;
  app->fonts->atlas_engine.get()->Init(FontDesc("ChessPieces1", "", 0, Color::white, Color::clear, 0, false));
  app->scheduler.AddMainWaitKeyboard(app->focused);
  app->scheduler.AddMainWaitMouse(app->focused);
  app->StartNewWindow(app->focused);

  ChessGUI *chess_gui = app->focused->GetOwnGUI<ChessGUI>(0);
  auto seek_command = &FLAGS_seek_command;

  // app->asset.Add(name,  texture,      scale, translate, rotate, geometry, hull,    0, 0);
  app->asset.Add("board1", "board1.png", 0,     0,         0,      nullptr,  nullptr, 0, 0);
  app->asset.Load();

  // app->soundasset.Add(name,   filename,      ringbuf, channels, sample_rate, seconds );
  app->soundasset.Add("start",   "start.wav",   nullptr, 0,        0,           0       );
  app->soundasset.Add("move",    "move.wav",    nullptr, 0,        0,           0       );
  app->soundasset.Add("capture", "capture.wav", nullptr, 0,        0,           0       );
  app->soundasset.Add("win",     "win.wav",     nullptr, 0,        0,           0       );
  app->soundasset.Add("lose",    "lose.wav",    nullptr, 0,        0,           0       );
  app->soundasset.Add("illegal", "illegal.wav", nullptr, 0,        0,           0       );
  app->soundasset.Load();

  my_app->askseek = make_unique<SystemAlertView>(AlertItemVec{
    { "style", "textinput" }, { "Seek Game", "Edit seek game criteria" }, { "Cancel", },
    { "Continue", "", bind([=](const string &a){ chess_gui->Send("seek " + (*seek_command = a)); }, _1)}
  });

  my_app->askresign = make_unique<SystemAlertView>(AlertItemVec{
    { "style", "confirm" }, { "Confirm resign", "Do you wish to resign?" }, { "No" },
    { "Yes", "", bind([=](){ chess_gui->Send("resign"); })}
  });

#ifndef LFL_MOBILE
  my_app->editmenu = SystemMenuView::CreateEditMenu(MenuItemVec{
    MenuItem{ "u", "Undo pre-move",         bind(&ChessGUI::UndoPremove, chess_gui, app->focused)},
    MenuItem{ "",  "Copy PGN to clipboard", bind(&ChessGUI::CopyPGNToClipboard, chess_gui)}
  });
  my_app->viewmenu = make_unique<SystemMenuView>("View", MenuItemVec{
    MenuItem{ "f",       "Flip board", bind(&ChessGUI::FlipBoard, chess_gui, app->focused)},
    MenuItem{ "<left>",  "Previous move" },
    MenuItem{ "<right>", "Next move" },
    MenuItem{ "<up>",    "Scroll up" },    
    MenuItem{ "<down>",  "Scroll down" }
  });
  {
    MenuItemVec gamemenu{
      MenuItem{ "s", "Seek",       bind([=](){ my_app->askseek->Show(*seek_command); })},
      MenuItem{ "d", "Offer Draw", bind([=](){ chess_gui->Send("draw"); })},
      MenuItem{ "r", "Resign",     bind([=](){ my_app->askresign->Show(""); })}
    };
    if (FLAGS_engine.size()) {
      gamemenu.push_back(MenuItem{"", "Engine play white", bind(&ChessGUI::StartEngine, chess_gui, false) });
      gamemenu.push_back(MenuItem{"", "Engine play black", bind(&ChessGUI::StartEngine, chess_gui, true) });
    }
    my_app->gamemenu = make_unique<SystemMenuView>("Game", move(gamemenu));
  }
#else
  my_app->maintoolbar = make_unique<SystemToolbarView>(MenuItemVec{ 
    MenuItem{ "\U000025C0", "", bind(&ChessGUI::WalkHistory, chess_gui, true) },
    MenuItem{ "\U000025B6", "", bind(&ChessGUI::WalkHistory, chess_gui, false) },
    MenuItem{ "seek",       "", bind([=](){ my_app->askseek->Show(*seek_command); }) },
    MenuItem{ "resign",     "", bind([=](){ my_app->askresign->Show(""); }) },
    MenuItem{ "draw",       "", bind([=](){ chess_gui->Send("draw"); }) },
    MenuItem{ "flip",       "", bind(&ChessGUI::FlipBoard,   chess_gui, app->focused) },
    MenuItem{ "undo",       "", bind(&ChessGUI::UndoPremove, chess_gui, app->focused) }
  });
  my_app->maintoolbar->Show(true);
#endif

  chess_gui->Open(FLAGS_connect);
  return app->Main();
}
