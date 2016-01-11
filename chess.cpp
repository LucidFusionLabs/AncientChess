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
AssetMap asset;
SoundAssetMap soundasset;
Chess::Position position;
bool console_animating = 0;
DEFINE_string(connect, "freechess.org:5000", "Connect to server");

struct ChessTerminal : public Terminal {
  using Terminal::Terminal;
  virtual void Enter() override { sink->WriteChar(Key::CtrlModified('j')); }
};

struct ChessTerminalWindow : public TerminalWindow {
  ChessTerminalWindow(const string &hostport) :
    TerminalWindow(new NetworkTerminalController(Singleton<HTTPClient>::Get(), hostport)) {
    controller->frame_on_keyboard_input = true;
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
    chess_terminal->OpenT<ChessTerminal>(term_dim.x, term_dim.y, FLAGS_default_font_size);
    chess_terminal->terminal->SetColors(Singleton<TextGUI::StandardVGAColors>::Get());
  }

  Box SquareCoords(int p) const {
    static const int border = 5;
    int w = screen->width-2*border, h = screen->height-2*border - term.h;
    return Box(border+Chess::SquareX(p)/8.0*w, border+Chess::SquareY(p)/8.0*h + term.h, 1/8.0*w, 1/8.0*h);
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
    chess_terminal->ReadAndUpdateTerminalFramebuffer();
    Draw();

    int black_font_index[7] = { 0, 3, 2, 0, 5, 4, 1 }, bits[65];
    static Font *pieces = Fonts::Get("ChessPieces1");
    for (int i=Chess::PAWN; i <= Chess::KING; i++) {
      Bit::Indices(position.white[i], bits); for (int *b = bits; *b != -1; b++) { Box w=SquareCoords(*b); pieces->DrawGlyph(black_font_index[i]+6, w); }
      Bit::Indices(position.black[i], bits); for (int *b = bits; *b != -1; b++) { Box w=SquareCoords(*b); pieces->DrawGlyph(black_font_index[i],   w); }
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
  binds->Add(Bind('6', Key::Modifier::Cmd, Bind::CB(bind(&Shell::console,         &app->shell, vector<string>()))));
  binds->Add(Bind(Key::Escape,             Bind::CB(bind(&Shell::quit,            &app->shell, vector<string>()))));

  chess_gui->Open(FLAGS_connect);
  return app->Main();
}
