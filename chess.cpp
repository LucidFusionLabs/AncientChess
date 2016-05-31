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
#include "term/term.h"

namespace LFL {
DEFINE_string(connect, "freechess.org:5000", "Connect to server");
DEFINE_bool(print_board_updates, false, "Print board updatees");

struct MyAppState {
  AssetMap asset;
  SoundAssetMap soundasset;
  point initial_term_dim = point(0, 10);
} *my_app;

struct ChessTerminal : public Terminal {
  using Terminal::Terminal;
  virtual void MakeMove(Terminal::Controller*, const string&) = 0;
  virtual void SetGameCallbacks(const StringCB &line_cb, const StringCB &update_cb) = 0;
};

struct FICSTerminal : public ChessTerminal {
  string prompt = "fics%", filter_prompt = StrCat("\n\r", prompt, " ");
  UnbackedTextBox local_cmd;
  NextRecordDispatcher line_buf;
  AhoCorasickFSM<char> move_fsm;
  StringMatcher<char> move_matcher;

  FICSTerminal(ByteSink *O, GraphicsDevice *D, const FontRef &F, const point &dim) :
    ChessTerminal(O, D, F, dim), local_cmd(F), move_fsm({"\r<12> "}),
    move_matcher(&move_fsm) {
    move_matcher.match_end_condition = &isint<'\r'>;
  }

  virtual void MakeMove(Terminal::Controller *controller, const string &move) {
    controller->Write(StrCat(move, "\n"));
  }

  virtual void SetGameCallbacks(const StringCB &line_cb, const StringCB &update_cb) {
    line_buf.cb = line_cb;
    move_matcher.match_cb = update_cb;
  }

  virtual void Input(char k) { local_cmd.Input(k); Terminal::Write(StringPiece(&k, 1)); }
  virtual void Erase      () { local_cmd.Erase();  Terminal::Write(StringPiece("\x08\x1b[1P")); }
  virtual void Enter      () {
    sink->Write(StrCat(String::ToUTF8(local_cmd.cmd_line.Text16()), "\n"));
    Terminal::Write(StringPiece("\r\n", 2));
    local_cmd.AssignInput("");
  }
  virtual void Tab        () {}
  virtual void Escape     () {}
  virtual void HistUp     () {}
  virtual void HistDown   () {}
  virtual void CursorRight() {}
  virtual void CursorLeft () {}
  virtual void PageUp     () {}
  virtual void PageDown   () {}
  virtual void Home       () {}
  virtual void End        () {}

  virtual void Write(const StringPiece &b, bool update_fb=true, bool release_fb=true) {
    if (line_buf.cb) line_buf.AddData(b);
    string s = b.str(), move_filtered;
    move_matcher.Match(s, &move_filtered);
    int suppressed_prompt = SuffixMatch(move_filtered, filter_prompt) ? filter_prompt.size() : 0;
    Terminal::Write(StringPiece(move_filtered.data(), move_filtered.size()-suppressed_prompt), update_fb, release_fb);
    if (suppressed_prompt && !PrefixMatch(String::ToUTF8(line.Back()->Text16()), prompt)) Terminal::Write(filter_prompt);
  }
};

typedef TerminalWindowT<ChessTerminal> ChessTerminalWindow;

struct ChessGUI : public GUI {
  unique_ptr<ChessTerminalWindow> chess_terminal;
  point term_dim=my_app->initial_term_dim;
  v2 square_dim;
  Box win, board, term;
  Widget::Divider divider;
  Time update_time;
  Chess::Move position;
  vector<Chess::Move> history;
  deque<Chess::Move> premove;
  string my_name, p1_name, p2_name;
  bool my_color = 0, flip_board = 0, title_changed = 0, console_animating = 0;
  int game_number = 0, history_ind = 0, p1_secs = 0, p2_secs = 0, last_p1_secs = 0, last_p2_secs = 0;
  int move_animate_from = -1, move_animate_to = -1;
  Time move_animation_start, move_animation_time = Time(200);
  pair<bool, int> dragging_piece, animating_piece;
  DragTracker drag_tracker;
  ChessGUI() : divider(this, true, term_dim.y * Fonts::InitFontHeight()) {}

  Box SquareCoords(int p) const {
    int sx = Chess::SquareX(p), sy = Chess::SquareY(p);
    return Box(board.x + (flip_board ? (7-sx) : sx) * square_dim.x,
               board.y + (flip_board ? (7-sy) : sy) * square_dim.y, square_dim.x, square_dim.y, true);
  }

  int SquareFromCoords(const point &p) const {
    int sx = p.x / square_dim.x, sy = p.y / square_dim.y;
    return Chess::SquareFromXY(flip_board ? (7-sx) : sx, flip_board ? (7-sy) : sy);
  }

  void Open(const string &hostport) {
    Activate();
    auto c = make_unique<NetworkTerminalController>(app->net->tcp_client.get(), hostport,
                                                    bind(&ChessGUI::ClosedCB, this));
    c->frame_on_keyboard_input = true;
    chess_terminal->ChangeController(move(c));
    chess_terminal->terminal->SetColors(Singleton<Terminal::StandardVGAColors>::Get());
    chess_terminal->terminal->SetGameCallbacks(bind(&ChessGUI::FICSLineCB, this, _1),
                                               bind(&ChessGUI::FICSGameUpdateCB, this, _1));
  }

  void FlipBoard(Window *w, const vector<string>&) { flip_board = !flip_board; app->scheduler.Wakeup(w); }
  void UpdateAnimating(Window *w) { app->scheduler.SetAnimating(w, (move_animate_from != -1) | console_animating); }
  void ClosedCB() {}

  void ConsoleAnimatingCB() {
    console_animating = screen ? screen->console->animating : 0;
    UpdateAnimating(screen);
  }

  void FICSLoginCB(const string &s) {
    my_name = s.substr(0, s.find("("));
    chess_terminal->controller->Write("\nset seek 0\nset style 12\n");
    screen->SetCaption(StrCat(my_name, " @ ", FLAGS_connect));
  }

  void FICSLineCB(const string &l) {
    static const string login_prefix = "\r**** Starting FICS session as ", game_prefix = "\r{Game ";
    if (PrefixMatch(l, login_prefix)) FICSLoginCB(StringWordIter(l.substr(login_prefix.size())).NextString());
    else if (PrefixMatch(l, "\rIllegal move ") || PrefixMatch(l, "\rYou are not playing ") ||
             PrefixMatch(l, "\rIt is not your move")) IllegalMoveCB();
    else if (PrefixMatch(l, game_prefix)) {
      StringWordIter words(l.data() + game_prefix.size(), l.size() - game_prefix.size());
      int game_no = atoi(words.NextString());
      string p1 = words.NextString(), res = words.NextString(), p2 = words.NextString();
      bool first_trailing = true;
      while(words.next_offset >= 0) {
        res = words.NextString();
        if (first_trailing && !(first_trailing = false) && res == "Creating") return GameStartCB();
      }
      GameOverCB(game_no, p1.substr(1), p2.substr(0, p2.size() ? p2.size()-1 : 0), res);
    }
  }

  void FICSGameUpdateCB(const string &s) {
    if (s.size() < 8*9) return;
    if (FLAGS_print_board_updates) INFO("GameUpdateCB: ", s.substr(8*9));

    update_time = Now();
    position.LoadByteBoard(s);
    position.square_from = position.square_to = -1;

    vector<string> args;
    Split(StringPiece::FromRemaining(s, 8*9), isspace, NULL, &args);
    if (args.size() < 23) return;

    position.move_color = args.size() && args[0] == "B";
    game_number = atoi(args[7]);
    p1_name = args[8];
    p2_name = args[9];
    p1_secs = atoi(args[15]);
    p2_secs = atoi(args[16]);

    position.number = atoi(args[17]) - !position.move_color;
    if (position.number == 0) {
      if      (my_name == p1_name) my_color = Chess::WHITE;
      else if (my_name == p2_name) my_color = Chess::BLACK;
      flip_board = my_color == Chess::BLACK;
    }

    const string &move = args[18];
    if (move == "none") {}
    else if (move == "o-o") {
      if (position.move_color) { position.square_from = Chess::e1; position.square_to = Chess::g1; }
      else                     { position.square_from = Chess::e8; position.square_to = Chess::g8; }
    } else if (move == "o-o-o") {
      if (position.move_color) { position.square_from = Chess::e1; position.square_to = Chess::c1; }
      else                     { position.square_from = Chess::e8; position.square_to = Chess::c8; }
    } else if (move.size() < 7 || move[1] != '/' || move[4] != '-' ||
               (position.square_from = Chess::SquareID(&move[2])) == -1 ||
               (position.square_to   = Chess::SquareID(&move[5])) == -1) return ERROR("Unknown move ", move);
    position.name = args[20];
    position.capture = position.name.find("x") != string::npos;

    history_ind = 0;
    bool new_move = !history.size() || history.back().number != position.number ||
      history.back().move_color != position.move_color;
    if (new_move) history.push_back(position);

    ReapplyPremoves();
    bool my_move_now = position.move_color == my_color;
    if (my_move_now && premove.size()) {
      Chess::Move pm = PopFront(premove);
      chess_terminal->terminal->MakeMove(chess_terminal->controller.get(), pm.name);
    }

    if (new_move && my_move_now) PositionUpdatedCB(position.square_from, position.square_to);
    else                         PositionUpdatedCB();
  }

  void GameStartCB() {
    history.clear();
    premove.clear();
    history_ind = 0;
    if (FLAGS_enable_audio) {
      static SoundAsset *start_sound = my_app->soundasset("start");
      app->PlaySoundEffect(start_sound);
    }
  }

  void PositionUpdatedCB(int animate_from = -1, int animate_to = -1) {
    title_changed = true;
    if (FLAGS_enable_audio) {
      static SoundAsset *move_sound = my_app->soundasset("move"), *capture_sound = my_app->soundasset("capture");
      app->PlaySoundEffect(position.capture ? capture_sound : move_sound);
    }
    if (animate_from != -1 && animate_to != -1 && (animating_piece = position.ClearSquare(animate_to)).second) {
      move_animate_from = animate_from;
      move_animate_to = animate_to;
      move_animation_start = Now();
    } else move_animate_from = -1;
    UpdateAnimating(screen);
  }
  
  void GameOverCB(int game_no, const string &p1, const string &p2, const string &result) {
    game_number = 0;
    bool lose = (my_name == p1 && result == "0-1") || (my_name == p2 && result == "1-0");
    if (FLAGS_enable_audio) {
      static SoundAsset *win_sound = my_app->soundasset("win"), *lose_sound = my_app->soundasset("lose");
      app->PlaySoundEffect(lose ? lose_sound : win_sound);
    }
  }

  void IllegalMoveCB() {
    if (FLAGS_enable_audio) {
      static SoundAsset *illegal_sound = my_app->soundasset("illegal");
      app->PlaySoundEffect(illegal_sound);
    }
  }

  void DragCB(int button, int x, int y, int down) {
    int square;
    point p = point(x, y) - board.Position();
    if ((square = SquareFromCoords(p)) < 0) return;
    bool start = drag_tracker.Update(p, down);
    if (start) dragging_piece = position.ClearSquare(square);
    if (!dragging_piece.second || down) return;
    position.SetSquare(square, dragging_piece);
    int start_square = SquareFromCoords(drag_tracker.beg_click);
    if (start_square == square) return;
    string move = StrCat(Chess::PieceChar(dragging_piece.second),
                         Chess::SquareName(start_square), Chess::SquareName(square));
    if (position.move_color == my_color)
      chess_terminal->terminal->MakeMove(chess_terminal->controller.get(), move);
    else {
      auto &pm = PushBack(premove, position);
      pm.name = move;
      pm.square_from = start_square;
      pm.square_to = square;
    }
  }

  void Layout() {
    Reset();
    win = screen->Box();
    term.w = win.w;
    term_dim.x = win.w / Fonts::InitFontWidth();
    divider.LayoutDivideBottom(win, &win, &term);
    board = Box::DelBorder(win, Border(5,5,5,5));
    square_dim = v2(board.w/8.0, board.h/8.0);
    mouse.AddDragBox(board, MouseController::CoordCB(bind(&ChessGUI::DragCB, this, _1, _2, _3, _4)));

    Texture *board_tex = &my_app->asset("board")->tex;
    child_box.PushBack(win, Drawable::Attr(board_tex), board_tex);
  }

  int Frame(LFL::Window *W, unsigned clicks, int flag) {
    chess_terminal->ReadAndUpdateTerminalFramebuffer();

    Time now = Now();
    if (game_number || title_changed) {
      if (game_number) {
        int secs = position.number ? ToSeconds(now - update_time).count() : 0;
        last_p1_secs = p1_secs - (position.move_color ? 0 : secs);
        last_p2_secs = p2_secs - (position.move_color ? secs : 0);
      }
      string title = StringPrintf("%s %d:%02d vs %s %d:%02d",
                                 p1_name.c_str(), last_p1_secs/60, last_p1_secs%60,
                                 p2_name.c_str(), last_p2_secs/60, last_p2_secs%60);
      if (position.number)
        StringAppendf(&title, ": %d%s: %s",
                      position.number, position.move_color ? "" : "...",
                      position.name.c_str());

      W->SetCaption(title);
      title_changed = false;
    }

    if (divider.changed) Layout();
    Draw();

    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = app->fonts->Get("ChessPieces1");
    Drawable::Attr draw_attr(pieces);
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(position.white[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i]+6, SquareCoords(*b));
      Bit::Indices(position.black[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i],   SquareCoords(*b));
    }

    if (position.square_from != -1 && position.square_to != -1) {
      W->gd->SetColor(Color(85, 85,  255)); BoxOutline().Draw(SquareCoords(position.square_from));
      W->gd->SetColor(Color(85, 255, 255)); BoxOutline().Draw(SquareCoords(position.square_to));
    }

    for (auto &pm : premove) {
      W->gd->SetColor(Color(255, 85,  85)); BoxOutline().Draw(SquareCoords(pm.square_from));
      W->gd->SetColor(Color(255, 255, 85)); BoxOutline().Draw(SquareCoords(pm.square_to));
    }
    W->gd->SetColor(Color::white);

    if (drag_tracker.changing && dragging_piece.second) {
      int glyph_index = black_font_index[dragging_piece.second] + 6*(!dragging_piece.first);
      int start_square = SquareFromCoords(drag_tracker.beg_click);
      pieces->DrawGlyph(glyph_index, SquareCoords(start_square) + (drag_tracker.end_click - drag_tracker.beg_click));
    }

    if (move_animate_from != -1) {
      if (move_animation_start + move_animation_time < now) {
        position.SetSquare(move_animate_to, animating_piece);
        move_animate_from = -1;
        UpdateAnimating(W);
      } 
      int glyph_index = black_font_index[animating_piece.second] + 6*(!animating_piece.first);
      Box start_square = SquareCoords(move_animate_from), end_square = SquareCoords(move_animate_to);
      float percent = min(1.0f, float((now - move_animation_start).count()) / move_animation_time.count());
      point slope(end_square.centerX() - start_square.centerX(), end_square.centerY() - start_square.centerY());
      point pos = start_square.center() + slope * percent;
      pieces->DrawGlyph(glyph_index, Box(pos.x - start_square.w/2, pos.y - start_square.h/2, start_square.w, start_square.h));
    }

    W->gd->DisableBlend();
    chess_terminal->terminal->Draw(term);
    if (divider.changing) BoxOutline().Draw(Box::DelBorder(term, Border(1,1,1,1)));

    W->DrawDialogs();
    return 0;
  }

  void WalkHistory(bool backwards) {
    if (!history.size()) return;
    int last_history_ind = history_ind, ind;
    if (backwards) history_ind = min<int>(history.size() - 1, history_ind + 1);
    else           history_ind = max<int>(0,                  history_ind - 1);
    if (history_ind == last_history_ind) return;
    position = history[history.size()-1-history_ind];
    if (!history_ind) ReapplyPremoves();

    if (!backwards) PositionUpdatedCB(position.square_from, position.square_to);
    else if (Clamp<int>(last_history_ind, 0, history.size()-1) == last_history_ind) {
      auto &last_position = history[history.size()-1-last_history_ind];
      PositionUpdatedCB(last_position.square_to, last_position.square_from);
    }
  }

  void ReapplyPremoves() {
    for (auto &pm : premove) {
      auto pm_piece = position.ClearSquare(pm.square_from);
      position.SetSquare(pm.square_to, pm_piece);
      pm.Assign(position);
    }
  }

  void UndoPremove(Window *W) {
    history_ind = 0;
    if (premove.size()) premove.pop_back();
    if (!premove.size() && !history.size()) return;
    position = premove.size() ? premove.back() : history.back();
    PositionUpdatedCB();
    app->scheduler.Wakeup(W);
  }

  void CopyPGNToClipboard() {
    string text = StrCat("[Site ", FLAGS_connect.substr(0, FLAGS_connect.find(":")), "]\n[Date: ",
                         logfileday(Now()), "\n[White \"", p1_name, "\"]\n[Black \"", p2_name, "\"]\n\n");
    if (history.size())
      for (auto b = history.begin()+1, e = history.end(), i = b; i != e; ++i)
        StrAppend(&text, ((i - b) % 2) ? "" : StrCat((i-b)/2+1, ". "), i->name, " ");
    StrAppend(&text, "\n");
    app->SetClipboardText(text);
  }
};

void MyWindowInit(Window *W) {
  screen->caption = "Chess";
  screen->width = 630;
  screen->height = 570 + my_app->initial_term_dim.y * Fonts::InitFontHeight();
}

void MyWindowStart(Window *W) {
  ChessGUI *chess_gui = W->AddGUI(make_unique<ChessGUI>());
  chess_gui->chess_terminal = make_unique<ChessTerminalWindow>
    (W->AddGUI(make_unique<FICSTerminal>(nullptr, W->gd, W->default_font, chess_gui->term_dim)));

  W->frame_cb = bind(&ChessGUI::Frame, chess_gui, _1, _2, _3);
  W->default_textbox = [=]{ return app->run ? chess_gui->chess_terminal->terminal : nullptr; };
  if (FLAGS_console) W->InitConsole(bind(&ChessGUI::ConsoleAnimatingCB, chess_gui));

  W->shell = make_unique<Shell>(&my_app->asset, &my_app->soundasset, nullptr);
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
  FLAGS_font_flag = FLAGS_console_font_flag = 0;
  FLAGS_console_font = "Nobile.ttf";
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
  if (app->Create(__FILE__)) return -1;
  if (app->Init()) return -1;

  app->fonts->atlas_engine.get()->Init(FontDesc("ChessPieces1", "", 0, Color::white, Color::clear, 0, false));
  app->scheduler.AddWaitForeverKeyboard(screen);
  app->scheduler.AddWaitForeverMouse(screen);
  app->StartNewWindow(screen);

  // my_app->asset.Add(name, texture,     scale, translate, rotate, geometry, hull,    0, 0);
  my_app->asset.Add("board", "board.png", 0,     0,         0,      nullptr,  nullptr, 0, 0);
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

  screen->GetGUI<ChessGUI>(0)->Open(FLAGS_connect);
  return app->Main();
}
