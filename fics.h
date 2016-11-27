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
  NextRecordDispatcher line_buf;
  AhoCorasickFSM<char> move_fsm;
  StringMatcher<char> move_matcher;

  FICSTerminal(ByteSink *O, Window *W, const FontRef &F, const point &dim) :
    ChessTerminal(O, W, F, dim), move_fsm({"\r<12> "}), move_matcher(&move_fsm) {
    move_matcher.match_end_condition = &isint<'\r'>;
    move_matcher.match_cb = bind(&FICSTerminal::FICSGameUpdateCB, this, _1);
    line_buf.cb = bind(&FICSTerminal::FICSLineCB, this, _1);
    line_fb.only_grow = cmd_fb.only_grow = true;
  }

  virtual void Send    (const string &text) { controller->Write(StrCat(text, "\n")); }
  virtual void MakeMove(const string &move) { controller->Write(StrCat(move, "\n")); }
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
    my_name = s.substr(0, s.find("("));
    login_cb();
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
    game->history_ind = 0;
    game->p1_name = args[8];
    game->p2_name = args[9];
    game->p1_secs = atoi(args[15]);
    game->p2_secs = atoi(args[16]);
    game->position.LoadByteBoard(s);
    game->position.flags.to_move_color = (args.size() && args[0] == "B");
    game->position.move_number = atoi(args[17])*2 - !game->position.flags.to_move_color - 1;

    const string &move = args[18];
    int8_t piece_type, square_from, square_to;
    bool piece_color = !game->position.flags.to_move_color;
    bool new_move = !game->history.size() || game->history.back().move_number != game->position.move_number;
    CHECK_EQ(piece_color, game->position.move_number % 2 == 0);

    if (move == "none") piece_type = square_from = square_to = 0;
    else if (move == "o-o") {
      piece_type = Chess::KING;
      if (!piece_color) { square_from = Chess::e1; square_to = Chess::g1; }
      else              { square_from = Chess::e8; square_to = Chess::g8; }
    } else if (move == "o-o-o") {
      piece_type = Chess::KING;
      if (!piece_color) { square_from = Chess::e1; square_to = Chess::c1; }
      else              { square_from = Chess::e8; square_to = Chess::c8; }
    } else if (move.size() < 7 || move[1] != '/' || move[4] != '-' ||
               !(piece_type = Chess::PieceCharType(move[0])) ||
               (square_from = Chess::SquareID(&move[2])) == -1 ||
               (square_to   = Chess::SquareID(&move[5])) == -1) return ERROR("Unknown move ", move);

    game->position.name = args[20];
    if (game->position.move_number) {
      uint8_t promotion = move.size() >= 2 && move[move.size()-2] == '=' ? Chess::PieceCharType(move.back()) : 0;
      game->position.UpdateMove(new_move, piece_type, square_from, square_to,
                                game->position.name.find("x") != string::npos, promotion, 0);
      if (new_move) game->AddNewMove();
    }

    bool my_game = game->p1_name == my_name || game->p2_name == my_name;
    bool my_move_now = game->position.flags.to_move_color == game->my_color;
    if (new_move && (!my_game || my_move_now)) game_update_cb(game, true, square_from, square_to);
    else                                       game_update_cb(game, true, -1, -1);

    if (my_move_now && game->premove.size()) MakeMove(PopFront(game->premove).name);
  }
};

}; // namespace LFL
#endif // #define LFL_CHESS_CHESS_H__
