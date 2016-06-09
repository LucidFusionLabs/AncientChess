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
#include "chess.h"
#include "fics.h"
#include "term/term.h"

namespace LFL {
DEFINE_string(connect, "freechess.org:5000", "Connect to server");
DEFINE_bool(click_or_drag_pieces, MOBILE, "Move by clicking or dragging");
DEFINE_bool(auto_close_old_games, true, "Close old games whenever new game starts");

struct MyAppState {
  AssetMap asset;
  SoundAssetMap soundasset;
  point initial_term_dim = point(0, 10);
} *my_app;

typedef TerminalWindowT<ChessTerminal> ChessTerminalWindow;

struct ChessGUI : public GUI {
  unique_ptr<ChessTerminalWindow> chess_terminal;
  point term_dim=my_app->initial_term_dim;
  v2 square_dim;
  string my_name;
  Box win, board, term;
  Widget::Divider divider;
  Chess::Game *top_game=0;
  unordered_map<int, Chess::Game> game_map;
  bool title_changed = 0, console_animating = 0;
  Time move_animation_time = Time(200);
  DragTracker drag_tracker;
  ChessGUI() : divider(this, true, screen->width) { (top_game = &game_map[0])->active = 0; }

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
    INFO("connecting to ", hostport);
    auto c = make_unique<NetworkTerminalController>(app->net->tcp_client.get(), hostport,
                                                    bind(&ChessGUI::ClosedCB, this));
    c->frame_on_keyboard_input = true;
    chess_terminal->ChangeController(move(c));

    auto t = chess_terminal->terminal;
    t->controller      = chess_terminal->controller.get();
    t->get_game_cb     = [=](int game_no){ return &game_map[game_no]; };
    t->illegal_move_cb = bind(&ChessGUI::IllegalMoveCB, this);
    t->login_cb        = bind(&ChessGUI::LoginCB,       this, _1);
    t->game_start_cb   = bind(&ChessGUI::GameStartCB,   this, _1);
    t->game_over_cb    = bind(&ChessGUI::GameOverCB,    this, _1, _2, _3, _4);
    t->game_update_cb  = bind(&ChessGUI::GameUpdateCB,  this, _1, _2, _3, _4);
    t->SetColors(Singleton<Terminal::StandardVGAColors>::Get());
  }

  void ListGames() { for (auto &i : game_map) INFO(i.first, " ", i.second.p1_name, " vs ", i.second.p2_name); }
  void FlipBoard(Window *w, const vector<string>&) { if (auto g = Top()) g->flip_board = !g->flip_board; app->scheduler.Wakeup(w); }
  void UpdateAnimating(Window *w) { app->scheduler.SetAnimating(w, (Top() && Top()->move_animate_from != -1) | console_animating); }

  void ConsoleAnimatingCB() {
    console_animating = screen ? screen->console->animating : 0;
    UpdateAnimating(screen);
  }

  void ClosedCB() {}
  void LoginCB(const string &n) { screen->SetCaption(StrCat((my_name = n), " @ ", FLAGS_connect)); }

  void GameStartCB(int game_no) {
    Chess::Game *game = top_game = &game_map[game_no];
    game->Reset();
    game->game_number = game_no;
    if (FLAGS_enable_audio) {
      static SoundAsset *start_sound = my_app->soundasset("start");
      app->PlaySoundEffect(start_sound);
    }
    if (FLAGS_auto_close_old_games) FilterValues<unordered_map<int, Chess::Game>>
      (&game_map, [](const pair<const int, Chess::Game> &x){ return !x.second.active; });
  }

  void GameOverCB(int game_no, const string &p1, const string &p2, const string &result) {
    bool lose = (my_name == p1 && result == "0-1") || (my_name == p2 && result == "1-0");
    if (FLAGS_enable_audio) {
      static SoundAsset *win_sound = my_app->soundasset("win"), *lose_sound = my_app->soundasset("lose");
      app->PlaySoundEffect(lose ? lose_sound : win_sound);
    }
    game_map[game_no].active = false;
  }

  void GameUpdateCB(Chess::Game *game, bool reapply_premove = false, int animate_from = -1, int animate_to = -1) {
    top_game = game;
    title_changed = true;
    if (FLAGS_enable_audio) {
      static SoundAsset *move_sound = my_app->soundasset("move"), *capture_sound = my_app->soundasset("capture");
      app->PlaySoundEffect(game->position.capture ? capture_sound : move_sound);
    }
    if (game->position.number == 0) {
      game->my_color = my_name == game->p2_name ? Chess::BLACK : Chess::WHITE;
      game->flip_board = game->my_color == Chess::BLACK;
    }
    if (reapply_premove) ReapplyPremoves(game);
    if (animate_from != -1 && animate_to != -1 &&
        (game->animating_piece = game->position.ClearSquare(animate_to)).second) {
      game->move_animate_from = animate_from;
      game->move_animate_to = animate_to;
      game->move_animation_start = Now();
    } else game->move_animate_from = -1;
    UpdateAnimating(screen);
  }

  void IllegalMoveCB() {
    if (FLAGS_enable_audio) {
      static SoundAsset *illegal_sound = my_app->soundasset("illegal");
      app->PlaySoundEffect(illegal_sound);
    }
  }

  void ClickCB(int button, int x, int y, int down) {
    Chess::Game *game = Top();
    if (!game || !down) return;
    point p = point(x, y) - board.Position();
    int square, start_square;
    pair<bool, int> moved_piece;
    if ((square = SquareFromCoords(p, game->flip_board)) < 0) return;
    if (!game->moving_piece.second) {
      drag_tracker.beg_click = drag_tracker.end_click = p;
      game->moving_piece = game->position.ClearSquare(square);
      return;
    }
    swap(game->moving_piece, moved_piece);
    game->position.SetSquare(square, moved_piece);
    if ((start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board)) == square) return;
    MakeMove(game, moved_piece.second, start_square, square);
  }

  void DragCB(int button, int x, int y, int down) {
    Chess::Game *game = Top();
    if (!game) return;
    point p = point(x, y) - board.Position();
    int square, start_square;
    pair<bool, int> moved_piece;
    if ((square = SquareFromCoords(p, game->flip_board)) < 0) return;
    if (drag_tracker.Update(p, down)) game->moving_piece = game->position.ClearSquare(square);
    if (!game->moving_piece.second || down) return;
    swap(game->moving_piece, moved_piece);
    game->position.SetSquare(square, moved_piece);
    if ((start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board)) == square) return;
    MakeMove(game, moved_piece.second, start_square, square);
  }

  void MakeMove(Chess::Game *game, int piece, int start_square, int end_square) {
    string move = StrCat(Chess::PieceChar(piece), Chess::SquareName(start_square), Chess::SquareName(end_square));
    if (game->position.move_color == game->my_color)
      chess_terminal->terminal->MakeMove(move);
    else {
      auto &pm = PushBack(game->premove, game->position);
      pm.name = move;
      pm.square_from = start_square;
      pm.square_to = end_square;
    }
  }

  void Reshaped() { divider.size = screen->width; }
  void Layout() {
    ResetGUI();
    win = screen->Box();
    term.w = win.w;
    term_dim.x = win.w / Fonts::InitFontWidth();
    divider.max_size = win.w;
    divider.LayoutDivideTop(win, &win, &term);
    CHECK_LE(win.h, win.w);
    if (int d = win.w - win.h) { win.w -= d; win.x += d/2; }
    CHECK_EQ(win.w, win.h);
    board = Box::DelBorder(win, Border(5,5,5,5));
    square_dim = v2(board.w/8.0, board.h/8.0);
    if (FLAGS_click_or_drag_pieces) mouse.AddClickBox(board, MouseController::CoordCB(bind(&ChessGUI::ClickCB, this, _1, _2, _3, _4)));
    else                            mouse.AddDragBox (board, MouseController::CoordCB(bind(&ChessGUI::DragCB,  this, _1, _2, _3, _4)));

    Texture *board_tex = &my_app->asset("board1")->tex;
    child_box.PushBack(win, Drawable::Attr(board_tex), board_tex);
  }

  int Frame(LFL::Window *W, unsigned clicks, int flag) {
    chess_terminal->ReadAndUpdateTerminalFramebuffer();
    Chess::Game *game = Top();
    Time now = Now();

    if (game && (title_changed || game->active)) {
      if (game && game->active) {
        int secs = game->position.number ? ToSeconds(now - game->update_time).count() : 0;
        game->last_p1_secs = game->p1_secs - (game->position.move_color ? 0 : secs);
        game->last_p2_secs = game->p2_secs - (game->position.move_color ? secs : 0);
      }
      string title = StringPrintf("%s %d:%02d vs %s %d:%02d",
                                 game->p1_name.c_str(), game->last_p1_secs/60, game->last_p1_secs%60,
                                 game->p2_name.c_str(), game->last_p2_secs/60, game->last_p2_secs%60);
      if (game->position.number)
        StringAppendf(&title, ": %d%s: %s",
                      game->position.number, game->position.move_color ? "" : "...",
                      game->position.name.c_str());

      W->SetCaption(title);
      title_changed = false;
    }

    if (divider.changed) Layout();
    if (win.w != screen->width)
    { ScopedFillColor sfc(screen->gd, Color::grey70); Box(screen->x, win.y, screen->width, win.h).Draw(); }
    Draw();
    DrawGame(W, game ? game : Singleton<Chess::Game>::Get(), now);

    W->gd->DisableBlend();
    { Scissor s(W->gd, term); chess_terminal->terminal->Draw(term); }
    if (divider.changing) BoxOutline().Draw(Box::DelBorder(term, Border(1,1,1,1)));

    W->DrawDialogs();
    return 0;
  }

  void DrawGame(Window *W, Chess::Game *game, Time now) {
    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = app->fonts->Get("ChessPieces1");
    Drawable::Attr draw_attr(pieces);
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(game->position.white[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i]+6, SquareCoords(*b, game->flip_board));
      Bit::Indices(game->position.black[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i],   SquareCoords(*b, game->flip_board));
    }

    if (game->position.square_from != -1 && game->position.square_to != -1) {
      W->gd->SetColor(Color(85, 85,  255)); BoxOutline().Draw(SquareCoords(game->position.square_from, game->flip_board));
      W->gd->SetColor(Color(85, 255, 255)); BoxOutline().Draw(SquareCoords(game->position.square_to,   game->flip_board));
    }

    for (auto &pm : game->premove) {
      W->gd->SetColor(Color(255, 85,  85)); BoxOutline().Draw(SquareCoords(pm.square_from, game->flip_board));
      W->gd->SetColor(Color(255, 255, 85)); BoxOutline().Draw(SquareCoords(pm.square_to,   game->flip_board));
    }

    if (game->moving_piece.second) {
      int start_square = SquareFromCoords(drag_tracker.beg_click, game->flip_board);
      Chess::BitBoard moves = PieceMoves(game->position, game->moving_piece.second, start_square, game->moving_piece.first);
      Bit::Indices(moves, bits);
      W->gd->SetColor(Color(255, 85, 255));
      for (int *b = bits; *b != -1; b++)   BoxOutline().Draw(SquareCoords(*b, game->flip_board));
      W->gd->SetColor(Color(170, 0, 170)); BoxOutline().Draw(SquareCoords(start_square, game->flip_board));
      W->gd->SetColor(Color::white);

      int glyph_index = black_font_index[game->moving_piece.second] + 6*(!game->moving_piece.first);
      pieces->DrawGlyph(glyph_index, SquareCoords(start_square, game->flip_board) + (drag_tracker.end_click - drag_tracker.beg_click));
    } else W->gd->SetColor(Color::white);

    if (game->move_animate_from != -1) {
      if (game->move_animation_start + move_animation_time < now) {
        game->position.SetSquare(game->move_animate_to, game->animating_piece);
        game->move_animate_from = -1;
        UpdateAnimating(W);
      } 
      int glyph_index = black_font_index[game->animating_piece.second] + 6*(!game->animating_piece.first);
      Box start_square = SquareCoords(game->move_animate_from, game->flip_board);
      Box end_square   = SquareCoords(game->move_animate_to,   game->flip_board);
      float percent = min(1.0f, float((now - game->move_animation_start).count()) / move_animation_time.count());
      point slope(end_square.centerX() - start_square.centerX(), end_square.centerY() - start_square.centerY());
      point pos = start_square.center() + slope * percent;
      pieces->DrawGlyph(glyph_index, Box(pos.x - start_square.w/2, pos.y - start_square.h/2, start_square.w, start_square.h));
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

    if (!backwards) GameUpdateCB(game, !game->history_ind, game->position.square_from, game->position.square_to);
    else if (Clamp<int>(last_history_ind, 0, game->history.size()-1) == last_history_ind) {
      auto &last_position = game->history[game->history.size()-1-last_history_ind];
      GameUpdateCB(game, !game->history_ind, last_position.square_to, last_position.square_from);
    }
  }

  void ReapplyPremoves(Chess::Game *game) {
    for (auto &pm : game->premove) {
      auto pm_piece = game->position.ClearSquare(pm.square_from);
      game->position.SetSquare(pm.square_to, pm_piece);
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
};

void MyWindowInit(Window *W) {
  screen->caption = "Chess";
  screen->width = 630;
  screen->height = 630 + my_app->initial_term_dim.y * Fonts::InitFontHeight();
}

void MyWindowStart(Window *W) {
  if (FLAGS_console) W->InitConsole(Callback());
  ChessGUI *chess_gui = W->AddGUI(make_unique<ChessGUI>());
  chess_gui->chess_terminal = make_unique<ChessTerminalWindow>
    (W->AddGUI(make_unique<FICSTerminal>(nullptr, W->gd, W->default_font, chess_gui->term_dim)));

  W->reshaped_cb = bind(&ChessGUI::Reshaped, chess_gui);
  W->frame_cb = bind(&ChessGUI::Frame, chess_gui, _1, _2, _3);
  W->default_textbox = [=]{ return app->run ? chess_gui->chess_terminal->terminal : nullptr; };
  if (FLAGS_console) W->console->animating_cb = bind(&ChessGUI::ConsoleAnimatingCB, chess_gui);

  W->shell = make_unique<Shell>(&my_app->asset, &my_app->soundasset, nullptr);
  W->shell->Add("games",       bind(&ChessGUI::ListGames, chess_gui));
  W->shell->Add("flip",        bind(&ChessGUI::FlipBoard, chess_gui, W, _1));
  W->shell->Add("undopremove", bind(&ChessGUI::UndoPremove, chess_gui, W));
  W->shell->Add("copypgn",     bind(&ChessGUI::CopyPGNToClipboard, chess_gui));

  BindMap *binds = W->AddInputController(make_unique<BindMap>());
  binds->Add(Key::Escape,                    Bind::CB(bind(&Shell::quit,    W->shell.get(), vector<string>())));
  binds->Add('6',        Key::Modifier::Cmd, Bind::CB(bind(&Shell::console, W->shell.get(), vector<string>())));
  binds->Add(Key::Up,    Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->terminal->ScrollUp();   app->scheduler.Wakeup(W); })));
  binds->Add(Key::Down,  Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->terminal->ScrollDown(); app->scheduler.Wakeup(W); })));
  binds->Add(Key::Left,  Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->WalkHistory(1); app->scheduler.Wakeup(W); })));
  binds->Add(Key::Right, Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->WalkHistory(0); app->scheduler.Wakeup(W); })));
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
  screen = new Window();
  my_app = new MyAppState();
  app->name = "LChess";
  app->window_start_cb = MyWindowStart;
  app->window_init_cb = MyWindowInit;
  app->window_init_cb(screen);
  app->exit_cb = [](){ delete my_app; };
}

extern "C" int MyAppMain() {
#ifdef LFL_MOBILE
  app->SetExtraScale(true);
#endif
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
  app->scheduler.AddWaitForeverKeyboard(screen);
  app->scheduler.AddWaitForeverMouse(screen);
  app->StartNewWindow(screen);

  // my_app->asset.Add(name,  texture,      scale, translate, rotate, geometry, hull,    0, 0);
  my_app->asset.Add("board1", "board1.png", 0,     0,         0,      nullptr,  nullptr, 0, 0);
  my_app->asset.Load();

  // my_app->soundasset.Add(name,   filename,      ringbuf, channels, sample_rate, seconds );
  my_app->soundasset.Add("start",   "start.wav",   nullptr, 0,        0,           0       );
  my_app->soundasset.Add("move",    "move.wav",    nullptr, 0,        0,           0       );
  my_app->soundasset.Add("capture", "capture.wav", nullptr, 0,        0,           0       );
  my_app->soundasset.Add("win",     "win.wav",     nullptr, 0,        0,           0       );
  my_app->soundasset.Add("lose",    "lose.wav",    nullptr, 0,        0,           0       );
  my_app->soundasset.Add("illegal", "illegal.wav", nullptr, 0,        0,           0       );
  my_app->soundasset.Load();

  vector<MenuItem> file_menu{ MenuItem{ "q", "Quit LChess", "quit" } };
  vector<MenuItem> view_menu{ MenuItem{ "f", "Flip board", "flip" },
    MenuItem{ "<left>", "Previous move", "" }, MenuItem{ "<right>", "Next move", "" },
    MenuItem{ "<up>", "Scroll up", "" }, MenuItem{ "<down>", "Scroll down", "" }, };
  vector<MenuItem> edit_menu{ MenuItem{ "u", "Undo pre-move", "undopremove" },
    MenuItem{ "", "Copy PGN to clipboard", "copypgn" } };
  app->AddNativeMenu("LChess", file_menu);
  app->AddNativeEditMenu(edit_menu);
  app->AddNativeMenu("View", view_menu);

  screen->GetOwnGUI<ChessGUI>(0)->Open(FLAGS_connect);
  return app->Main();
}
