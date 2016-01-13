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

AssetMap asset;
SoundAssetMap soundasset;
Chess::Position position;
Time move_time;
int move_square_from = -1, move_square_to = -1;
bool console_animating = 0;

struct ChessTerminal : public Terminal {
  bool logged_in = false;
  unsigned char newline = '\n';
  string erasechar = "\xff\xf7";
  AhoCorasickFSM<char> login_fsm, move_fsm;
  StringMatcher<char> login_matcher, move_matcher;
  ChessTerminal(ByteSink *O, Window *W, Font *F) :
    Terminal(O, W, F), login_fsm({"\rfics%"}), move_fsm({"\r<12> "}), login_matcher(&login_fsm), move_matcher(&move_fsm) {
    move_matcher.match_end_condition = &isint<'\r'>;
  }

  virtual void Enter() override { sink->WriteChar(newline); }
  virtual void Erase() override { sink->Write(erasechar.data(), erasechar.size()); }
  virtual void Write(const StringPiece &b, bool update_fb=true, bool release_fb=true) {
    string s = b.str();
    if (!logged_in) login_matcher.Match(s);
    string filtered;
    move_matcher.Match(s, &filtered);
    Terminal::Write(filtered, update_fb, release_fb);
  }
};

struct ChessTerminalWindow : public TerminalWindowT<ChessTerminal> {
  ChessTerminalWindow(const string &hostport) :
    TerminalWindowT(new NetworkTerminalController(Singleton<HTTPClient>::Get(), hostport)) {
    controller->frame_on_keyboard_input = true;
    controller->local_echo = true;
  }
};

struct ChessGUI : public GUI {
  point term_dim;
  Box win, term;
  ChessTerminalWindow *chess_terminal=0;
  ChessGUI() : GUI(screen), term_dim(0, 10), term(0, 0, 0, term_dim.y * Video::InitFontHeight()) {}

  void Open(const string &hostport) {
    Layout();
    CheckNullAssign(&chess_terminal, new ChessTerminalWindow(hostport));
    chess_terminal->Open(term_dim.x, term_dim.y, FLAGS_default_font_size);
    chess_terminal->terminal->SetColors(Singleton<TextGUI::StandardVGAColors>::Get());
    chess_terminal->terminal->login_matcher.match_cb = bind(&ChessGUI::LoginCB, this, _1);
    chess_terminal->terminal->move_matcher.match_cb = bind(&ChessGUI::GameUpdateCB, this, _1);
  }

  Box SquareCoords(int p) const {
    static const int border = 5;
    int w = screen->width-2*border, h = screen->height-2*border - term.h;
    return Box(border+Chess::SquareX(p)/8.0*w, border+Chess::SquareY(p)/8.0*h + term.h, 1/8.0*w, 1/8.0*h);
  }

  void LoginCB(const string&) {
    unsigned char nl = chess_terminal->terminal->newline;
    string setup_command = StrCat(nl, "set style 12", nl);
    chess_terminal->terminal->logged_in = true;
    chess_terminal->controller->Write(setup_command.data(), setup_command.size());
  }

  void GameUpdateCB(const string &s) {
    if (s.size() < 8*9) return;
    vector<string> args;
    Split(StringPiece::FromRemaining(s, 8*9), isspace, NULL, &args);
    position.LoadByteBoard(s, args.size() && args[0] == "B");
    move_square_from = move_square_to = -1;
    if (args.size() < 23) return;

    const string &move = args[18];
    if (move == "o-o") {
      if (position.move_color) { move_square_from = Chess::e1; move_square_to = Chess::g1; }
      else                     { move_square_from = Chess::e8; move_square_to = Chess::g8; }
    } else if (move == "o-o-o") {
      if (position.move_color) { move_square_from = Chess::e1; move_square_to = Chess::c1; }
      else                     { move_square_from = Chess::e8; move_square_to = Chess::c8; }
    } else if (move.size() < 7 || move[1] != '/' || move[4] != '-' ||
               (move_square_from = Chess::SquareID(&move[2])) == -1 ||
               (move_square_to   = Chess::SquareID(&move[5])) == -1) return ERROR("Unknown move ", move);
    move_time = Now();
  }

  void Layout() {
    win = screen->Box();
    term.w = win.w;
    term_dim.x = win.w / Video::InitFontWidth();
    MinusPlus(&win.h, &win.y, term.h);
    Texture *board_tex = &asset("board")->tex;
    child_box.PushBack(win, Drawable::Attr(board_tex), board_tex);
  }

  int Frame(LFL::Window *W, unsigned clicks, int flag) {
    Time now = Now();
    chess_terminal->ReadAndUpdateTerminalFramebuffer();
    Draw();

    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = Fonts::Get("ChessPieces1");
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(position.white[i], bits); for (int *b = bits; *b != -1; b++) { Box w=SquareCoords(*b); pieces->DrawGlyph(black_font_index[i]+6, w); }
      Bit::Indices(position.black[i], bits); for (int *b = bits; *b != -1; b++) { Box w=SquareCoords(*b); pieces->DrawGlyph(black_font_index[i],   w); }
    }

    if (move_square_from != -1 && move_square_to != -1) {
      screen->gd->SetColor(Color(85, 85,  255)); BoxOutline(1).Draw(SquareCoords(move_square_from));
      screen->gd->SetColor(Color(85, 255, 255)); BoxOutline(1).Draw(SquareCoords(move_square_to));
      screen->gd->SetColor(Color::white);
    }

    W->gd->DisableBlend();
    chess_terminal->terminal->Draw(term);

    screen->DrawDialogs();
    return 0;
  }
} *chess_gui;

void UpdateAnimating() { app->scheduler.SetAnimating(console_animating); }
void OnConsoleAnimating() { console_animating = screen->lfapp_console->animating; UpdateAnimating(); }

}; // namespace LFL
using namespace LFL;

extern "C" void LFAppCreateCB() {
  app->logfilename = StrCat(LFAppDownloadDir(), "chess.txt");
  FLAGS_lfapp_video = FLAGS_lfapp_input = FLAGS_lfapp_network = FLAGS_lfapp_console = 1;
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
  if (screen->lfapp_console) screen->lfapp_console->animating_cb = OnConsoleAnimating;
  Singleton<AtlasFontEngine>::Get()->Init(FontDesc("ChessPieces1", "", 0, Color::white, Color::clear, 0, false));

  // asset.Add(Asset(name, texture,     scale, translate, rotate, geometry, 0, 0, 0));
  asset.Add(Asset("board", "board.png", 0,     0,         0,      0,        0, 0, 0));
  asset.Load();
  app->shell.assets = &asset;

  // soundasset.Add(SoundAsset(name, filename,   ringbuf, channels, sample_rate, seconds ));
  soundasset.Add(SoundAsset("draw",  "Draw.wav", 0,       0,        0,           0       ));
  soundasset.Load();
  app->shell.soundassets = &soundasset;

  BindMap *binds = screen->binds = new BindMap();
  binds->Add(Bind('6',       Key::Modifier::Cmd, Bind::CB(bind(&Shell::console, &app->shell, vector<string>()))));
  binds->Add(Bind(Key::Escape,                   Bind::CB(bind(&Shell::quit,    &app->shell, vector<string>()))));
  binds->Add(Bind(Key::Up,   Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->ScrollHistory(1); }))));
  binds->Add(Bind(Key::Down, Key::Modifier::Cmd, Bind::CB(bind([=](){ chess_gui->chess_terminal->ScrollHistory(0); }))));

  chess_gui->Open(FLAGS_connect);
  return app->Main();
}
