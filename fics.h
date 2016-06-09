/*
 * $Id: fics.h 1309 2014-10-10 19:20:55Z justin $
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

#ifndef LFL_CHESS_FICS_H__
#define LFL_CHESS_FICS_H__
namespace LFL {

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
    move_matcher.match_cb = bind(&FICSTerminal::FICSGameUpdateCB, this, _1);
    line_buf.cb = bind(&FICSTerminal::FICSLineCB, this, _1);
    line_fb.only_grow = cmd_fb.only_grow = true;
  }

  virtual void MakeMove(const string &move) { controller->Write(StrCat(move, "\n")); }
  virtual void Input(char k) { local_cmd.Input(k); Terminal::Write(StringPiece(&k, 1)); }
  virtual void Erase      () { local_cmd.Erase();  Terminal::Write(StringPiece("\x08\x1b[1P")); }
  virtual void Enter      () {
    string cmd = String::ToUTF8(local_cmd.cmd_line.Text16());
    if (cmd == "console" && screen && screen->shell) screen->shell->console(StringVec());
    else sink->Write(StrCat(cmd, "\n"));
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

  virtual void FICSLoginCB(const string &s) {
    controller->Write("\nset seek 0\nset style 12\n");
    login_cb(s.substr(0, s.find("(")));
  }

  virtual void FICSLineCB(const string &l) {
    static const string login_prefix = "\r**** Starting FICS session as ", game_prefix = "\r{Game ";
    if (PrefixMatch(l, login_prefix)) FICSLoginCB(StringWordIter(l.substr(login_prefix.size())).NextString());
    else if (PrefixMatch(l, "\rIllegal move ") || PrefixMatch(l, "\rYou are not playing ") ||
             PrefixMatch(l, "\rIt is not your move")) illegal_move_cb();
    else if (PrefixMatch(l, game_prefix)) {
      StringWordIter words(l.data() + game_prefix.size(), l.size() - game_prefix.size());
      int game_no = atoi(words.NextString());
      string p1 = words.NextString(), res = words.NextString(), p2 = words.NextString();
      bool first_trailing = true;
      while(words.next_offset >= 0) {
        res = words.NextString();
        if (first_trailing && !(first_trailing = false) && res == "Creating") return game_start_cb(game_no);
      }
      game_over_cb(game_no, p1.substr(1), p2.substr(0, p2.size() ? p2.size()-1 : 0), res);
    }
  }

  virtual void FICSGameUpdateCB(const string &s) {
    if (s.size() < 8*9) return;
    vector<string> args;
    Split(StringPiece::FromRemaining(s, 8*9), isspace, NULL, &args);
    if (args.size() < 23) return;

    int game_no = atoi(args[7]);
    Chess::Game *game = get_game_cb(game_no);
    game->active = true;
    game->game_number = game_no;
    game->update_time = Now();
    game->position.LoadByteBoard(s);
    game->position.square_from = game->position.square_to = -1;
    game->position.move_color = args.size() && args[0] == "B";
    game->p1_name = args[8];
    game->p2_name = args[9];
    game->p1_secs = atoi(args[15]);
    game->p2_secs = atoi(args[16]);
    game->position.number = atoi(args[17]) - !game->position.move_color;

    bool black = game->position.move_color;
    const string &move = args[18];
    if (move == "none") game->position.square_from = game->position.square_to = -1;
    else if (move == "o-o") {
      if (black) { game->position.square_from = Chess::e1; game->position.square_to = Chess::g1; }
      else       { game->position.square_from = Chess::e8; game->position.square_to = Chess::g8; }
    } else if (move == "o-o-o") {
      if (black) { game->position.square_from = Chess::e1; game->position.square_to = Chess::c1; }
      else       { game->position.square_from = Chess::e8; game->position.square_to = Chess::c8; }
    } else if (move.size() < 7 || move[1] != '/' || move[4] != '-' ||
               (game->position.square_from = Chess::SquareID(&move[2])) == -1 ||
               (game->position.square_to   = Chess::SquareID(&move[5])) == -1) return ERROR("Unknown move ", move);
    game->position.name = args[20];
    game->position.capture = game->position.name.find("x") != string::npos;

    game->history_ind = 0;
    bool new_move = !game->history.size() || game->history.back().number != game->position.number ||
      game->history.back().move_color != game->position.move_color;
    if (new_move) game->HandleNewMove();

    bool my_move_now = game->position.move_color == game->my_color;
    if (new_move && my_move_now) game_update_cb(game, true, game->position.square_from, game->position.square_to);
    else                         game_update_cb(game, true, -1, -1);

    if (my_move_now && game->premove.size()) MakeMove(PopFront(game->premove).name);
  }
};

}; // namespace LFL
#endif // #define LFL_CHESS_CHESS_H__
