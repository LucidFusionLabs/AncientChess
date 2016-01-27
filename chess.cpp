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

#include "lfapp/lfapp.h"
#include "lfapp/dom.h"
#include "lfapp/css.h"
#include "lfapp/flow.h"
#include "lfapp/gui.h"
#include "chess.h"
#include "../term/term.h"

namespace LFL {
DEFINE_string(connect, "freechess.org:5000", "Connect to server");
DEFINE_bool(print_board_updates, false, "Print board updatees");

AssetMap asset;
SoundAssetMap soundasset;
Chess::Position position;
bool console_animating = 0;

struct ChessTerminal : public Terminal {
  string prompt = "fics%", filter_prompt = StrCat("\n\r", prompt, " ");
  bool logged_in = false;
  UnbackedTextGUI local_cmd;
  AhoCorasickFSM<char> login_fsm, move_fsm;
  StringMatcher<char> login_matcher, move_matcher;

  ChessTerminal(ByteSink *O, Window *W, Font *F) :
    Terminal(O, W, F), local_cmd(Fonts::Default()),
    login_fsm({"\r**** Starting FICS session as "}), move_fsm({"\r<12> "}),
    login_matcher(&login_fsm), move_matcher(&move_fsm) {
    move_matcher.match_end_condition = &isint<'\r'>;
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
    string s = b.str();
    if (!logged_in) login_matcher.Match(s);
    string move_filtered;
    move_matcher.Match(s, &move_filtered);
    int suppressed_prompt = SuffixMatch(move_filtered, filter_prompt) ? filter_prompt.size() : 0;
    Terminal::Write(StringPiece(move_filtered.data(), move_filtered.size()-suppressed_prompt), update_fb, release_fb);
    if (suppressed_prompt && !PrefixMatch(String::ToUTF8(line.Back()->Text16()), prompt)) Terminal::Write(filter_prompt);
  }
};

struct ChessTerminalWindow : public TerminalWindowT<ChessTerminal> {
  ChessTerminalWindow(const string &hostport) :
    TerminalWindowT(new NetworkTerminalController(Singleton<HTTPClient>::Get(), hostport)) {
    controller->frame_on_keyboard_input = true;
  }
};

struct ChessGUI : public GUI {
  point term_dim, square_dim;
  Box win, board, term;
  DragTracker drag_tracker;
  pair<bool, int> dragging_piece;
  ChessTerminalWindow *chess_terminal=0;
  int move_square_from = -1, move_square_to = -1;
  bool move_capture = 0, flip_board = 0;
  Time move_time;
  string my_name;
  ChessGUI() : GUI(screen), term_dim(0, 10), term(0, 0, 0, term_dim.y * Video::InitFontHeight()) {}

  void Open(const string &hostport) {
    Activate();
    Layout();
    CheckNullAssign(&chess_terminal, new ChessTerminalWindow(hostport));
    chess_terminal->Open(term_dim.x, term_dim.y, FLAGS_default_font_size);
    chess_terminal->terminal->SetColors(Singleton<TextGUI::StandardVGAColors>::Get());
    chess_terminal->terminal->login_matcher.match_cb = bind(&ChessGUI::LoginCB, this, _1);
    chess_terminal->terminal->move_matcher.match_cb = bind(&ChessGUI::GameUpdateCB, this, _1);
  }

  Box SquareCoords(int p) const {
    int sx = Chess::SquareX(p), sy = Chess::SquareY(p);
    return Box(board.x + (flip_board ? (7-sx) : sx) * square_dim.x,
               board.y + (flip_board ? (7-sy) : sy) * square_dim.y, square_dim.x, square_dim.y);
  }

  int SquareFromCoords(const point &p) const {
    int sx = p.x / square_dim.x, sy = p.y / square_dim.y;
    return Chess::SquareFromXY(flip_board ? (7-sx) : sx, flip_board ? (7-sy) : sy);
  }

  void LoginCB(const string &s) {
    my_name = s.substr(0, s.find("("));
    chess_terminal->terminal->logged_in = true;
    chess_terminal->controller->Write("\nset style 12\n");
    screen->SetCaption(StrCat(my_name, " @ ", FLAGS_connect));
  }

  void GameUpdateCB(const string &s) {
    if (s.size() < 8*9) return;
    if (FLAGS_print_board_updates) INFO("GameUpdateCB: ", s.substr(8*9));

    position.LoadByteBoard(s);
    move_square_from = move_square_to = -1;

    vector<string> args;
    Split(StringPiece::FromRemaining(s, 8*9), isspace, NULL, &args);
    if (args.size() < 23) return;

    position.move_color = args.size() && args[0] == "B";

    const string &p1_name = args[8], &p2_name = args[9];
    int p1_secs = atoi(args[15]), p2_secs = atoi(args[16]);
    screen->SetCaption(StringPrintf("%s %d:%02d vs %s %d:%02d",
                                    p1_name.c_str(), p1_secs/60, p1_secs%60,
                                    p2_name.c_str(), p2_secs/60, p2_secs%60));

    int move_number = atoi(args[17]);
    if (move_number == 1) {
      if      (my_name == p1_name) flip_board = 0;
      else if (my_name == p2_name) flip_board = 1;
    }

    const string &move = args[18];
    if (move == "none") {}
    else if (move == "o-o") {
      if (position.move_color) { move_square_from = Chess::e1; move_square_to = Chess::g1; }
      else                     { move_square_from = Chess::e8; move_square_to = Chess::g8; }
    } else if (move == "o-o-o") {
      if (position.move_color) { move_square_from = Chess::e1; move_square_to = Chess::c1; }
      else                     { move_square_from = Chess::e8; move_square_to = Chess::c8; }
    } else if (move.size() < 7 || move[1] != '/' || move[4] != '-' ||
               (move_square_from = Chess::SquareID(&move[2])) == -1 ||
               (move_square_to   = Chess::SquareID(&move[5])) == -1) return ERROR("Unknown move ", move);
    move_capture = args[20].find("x") != string::npos;
    move_time = Now();

    if (FLAGS_lfapp_audio) {
      static SoundAsset *move_sound = soundasset("move"), *capture_sound = soundasset("capture");
      app->PlaySoundEffect(move_capture ? capture_sound : move_sound);
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
    string move = StrCat(Chess::PieceChar(dragging_piece.second),
                         Chess::SquareName(start_square), Chess::SquareName(square));
    chess_terminal->controller->Write(StrCat(move, "\n"));
  }

  void Layout() {
    win = screen->Box();
    term.w = win.w;
    term_dim.x = win.w / Video::InitFontWidth();
    MinusPlus(&win.h, &win.y, term.h);
    board = Box::DelBorder(win, Border(5,5,5,5));
    square_dim = point(board.w/8, board.h/8);
    AddDragBox(board, MouseController::CoordCB(bind(&ChessGUI::DragCB, this, _1, _2, _3, _4)));

    Texture *board_tex = &asset("board")->tex;
    child_box.PushBack(win, Drawable::Attr(board_tex), board_tex);
  }

  int Frame(LFL::Window *W, unsigned clicks, int flag) {
    Time now = Now();
    chess_terminal->ReadAndUpdateTerminalFramebuffer();
    Draw();

    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = Fonts::Get("ChessPieces1");
    Drawable::Attr draw_attr(pieces);
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(position.white[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i]+6, SquareCoords(*b));
      Bit::Indices(position.black[i], bits); for (int *b = bits; *b != -1; b++) pieces->DrawGlyph(black_font_index[i],   SquareCoords(*b));
    }

    if (move_square_from != -1 && move_square_to != -1) {
      screen->gd->SetColor(Color(85, 85,  255)); BoxOutline(1).Draw(SquareCoords(move_square_from));
      screen->gd->SetColor(Color(85, 255, 255)); BoxOutline(1).Draw(SquareCoords(move_square_to));
      screen->gd->SetColor(Color::white);
    }

    if (drag_tracker.changing && dragging_piece.second) {
      int glyph_index = black_font_index[dragging_piece.second] + 6*(!dragging_piece.first);
      int start_square = SquareFromCoords(drag_tracker.beg_click);
      pieces->DrawGlyph(glyph_index, SquareCoords(start_square) + (drag_tracker.end_click - drag_tracker.beg_click));
    }

    W->gd->DisableBlend();
    chess_terminal->terminal->Draw(term);

    screen->DrawDialogs();
    return 0;
  }
} *chess_gui;

void UpdateAnimating() { app->scheduler.SetAnimating(console_animating); }
void OnConsoleAnimating() { console_animating = screen->lfapp_console->animating; UpdateAnimating(); }

void MyFlipBoardCmd(const vector<string>&) {
  chess_gui->flip_board = !chess_gui->flip_board;
  app->scheduler.Wakeup(0);
}

}; // namespace LFL
using namespace LFL;

extern "C" void LFAppCreateCB() {
#ifdef LFL_DEBUG
  app->logfilename = StrCat(LFAppDownloadDir(), "chess.txt");
#endif
  FLAGS_lfapp_video = FLAGS_lfapp_audio = FLAGS_lfapp_input = FLAGS_lfapp_network = FLAGS_lfapp_console = 1;
  FLAGS_default_font_flag = FLAGS_lfapp_console_font_flag = 0;
  FLAGS_lfapp_console_font = "Nobile.ttf";
  screen->caption = "Chess";
  screen->width = 630;
  chess_gui = new ChessGUI();
  screen->height = 570 + chess_gui->term.h;
  screen->frame_cb = bind(&ChessGUI::Frame, chess_gui, _1, _2, _3);
}

extern "C" int main(int argc, const char *argv[]) {
  if (app->Create(argc, argv, __FILE__, LFAppCreateCB)) { app->Free(); return -1; }
  if (app->Init())                                      { app->Free(); return -1; }
  app->scheduler.AddWaitForeverKeyboard();
  app->scheduler.AddWaitForeverMouse();
  app->shell.command.push_back(Shell::Command("flip", bind(&MyFlipBoardCmd, _1)));
  if (screen->lfapp_console) screen->lfapp_console->animating_cb = OnConsoleAnimating;
  Singleton<AtlasFontEngine>::Get()->Init(FontDesc("ChessPieces1", "", 0, Color::white, Color::clear, 0, false));

  // asset.Add(Asset(name, texture,     scale, translate, rotate, geometry, 0, 0, 0));
  asset.Add(Asset("board", "board.png", 0,     0,         0,      0,        0, 0, 0));
  asset.Load();
  app->shell.assets = &asset;

  // soundasset.Add(SoundAsset(name,   filename,      ringbuf, channels, sample_rate, seconds ));
  soundasset.Add(SoundAsset("move",    "move.wav",    0,       0,        0,           0       ));
  soundasset.Add(SoundAsset("capture", "capture.wav", 0,       0,        0,           0       ));
  soundasset.Load();
  app->shell.soundassets = &soundasset;

  BindMap *binds = screen->binds = new BindMap();
  binds->Add(Bind(Key::Escape,                   Bind::CB(bind(&Shell::quit,    &app->shell, vector<string>()))));
  binds->Add(Bind('6',       Key::Modifier::Cmd, Bind::CB(bind(&Shell::console, &app->shell, vector<string>()))));
  binds->Add(Bind(Key::Up,   Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->ScrollHistory(1); }))));
  binds->Add(Bind(Key::Down, Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->ScrollHistory(0); }))));
  binds->Add(Bind('f',       Key::Modifier::Cmd, Bind::CB(bind([=](){ app->shell.console(vector<string>(1, "flip")); }))));

  vector<MenuItem> file_menu{ MenuItem{ "q", "Quit LChess", "quit" } };
  vector<MenuItem> view_menu{ MenuItem{ "f", "Flip board", "flip" } };
  app->AddNativeMenu("LChess", file_menu);
  app->AddNativeEditMenu();
  app->AddNativeMenu("View", view_menu);

  chess_gui->Open(FLAGS_connect);
  return app->Main();
}
