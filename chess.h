/*
 * $Id: chess.h 1309 2014-10-10 19:20:55Z justin $
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

#ifndef LFL_CHESS_CHESS_H__
#define LFL_CHESS_CHESS_H__
namespace LFL {
namespace Chess {

enum { WHITE=0, BLACK=1 };
enum { ALL=0, PAWN=1, KNIGHT=2, BISHOP=3, ROOK=4, QUEEN=5, KING=6 };

enum {
  h1=0,  g1=1,  f1=2,  e1=3,  d1=4,  c1=5,  b1=6,  a1=7,
  h2=8,  g2=9,  f2=10, e2=11, d2=12, c2=13, b2=14, a2=15,
  h3=16, g3=17, f3=18, e3=19, d3=20, c3=21, b3=22, a3=23,
  h4=24, g4=25, f4=26, e4=27, d4=28, c4=29, b4=30, a4=31,
  h5=32, g5=33, f5=34, e5=35, d5=36, c5=37, b5=38, a5=39,
  h6=40, g6=41, f6=42, e6=43, d6=44, c6=45, b6=46, a6=47,
  h7=48, g7=49, f7=50, e7=51, d7=52, c7=53, b7=54, a7=55,
  h8=56, g8=57, f8=58, e8=59, d8=60, c8=61, b8=62, a8=63
};

typedef unsigned long long BitBoard;
typedef string ByteBoard;

static const char initial_byte_board[] = 
"rnbqkbnr\n"
"pppppppp\n"
"--------\n"
"--------\n"
"--------\n"
"--------\n"
"PPPPPPPP\n"
"RNBQKBNR\n";

//                                      { 0, pawns,         knights,     bishops,     rooks,       queen,       king       };
static const BitBoard white_initial[] = { 0, 0xff00ULL,     0x42ULL,     0x24ULL,     0x81ULL,     0x10ULL,     0x8ULL     };
static const BitBoard black_initial[] = { 0, 0xff00ULL<<40, 0x42ULL<<56, 0x24ULL<<56, 0x81ULL<<56, 0x10ULL<<56, 0x8ULL<<56 };

static const char *ColorName(int n) {
  static const char *name[] = { "white", "black" };
  CHECK_RANGE(n, 0, 2);
  return name[n];
}

static const char PieceChar(int n) {
  static const char name[] = { 0, 'p', 'n', 'b', 'r', 'q', 'k' };
  CHECK_RANGE(n, 0, 7);
  return name[n];
}

static const char *PieceName(int n) {
  static const char *name[] = { "", "pawn", "knight", "bishop", "rook", "queen", "king" };
  CHECK_RANGE(n, 0, 7);
  return name[n];
}

static const char ByteBoardPieceSymbol(int n, bool color) {
  static const char white_piece_value[] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };
  static const char black_piece_value[] = { '-', 'p', 'n', 'b', 'r', 'q', 'k' };
  CHECK_RANGE(n, 0, 7);
  return color ? black_piece_value[n] : white_piece_value[n];
}

static int SquareX(int s) { return 7 - (s % 8); }
static int SquareY(int s) { return s / 8; }
static int SquareFromXY(int x, int y) { return (x<0 || y<0 || x>7 || y>7) ? -1 : (y*8 + (7-x)); }

static int SquareID(const char *s) {
  if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8') return -1;
  return 8 * (s[1] - '1') + 7 - (s[0] - 'a');
}

static const char *SquareName(int s) {
  static const char *name[] = {
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
  };
  CHECK_RANGE(s, 0, 64);
  return name[s];
};

string BitBoardToString(BitBoard b) {
  string ret;
  for (int i=7; i>=0; i--) {
    for (int j=7; j>=0; j--) StrAppend(&ret, bool(b & (1L<<(i*8+j))));
    StrAppend(&ret, "\n");
  }
  return ret;
}

BitBoard BitBoardFromString(const char *buf, char v='1') {
  BitBoard ret = 0;
  for (int i=7, bi=0; i>=0; i--, bi++) {
    for (int j=7; j>=0; j--, bi++) if (buf[bi] == v) ret |= (1L<<(i*8+j));
  }
  return ret;
}

BitBoard ByteBoardToBitBoard(const ByteBoard &buf, char piece) {
  BitBoard ret = 0;
  for (int i=7, bi=0; i>=0; i--, bi++) {
    for (int j=7; j>=0; j--, bi++) if (buf[bi] == piece) ret |= (1L<<(i*8+j));
  }
  return ret;
}

struct Position {
  bool move_color=WHITE;
  BitBoard white[7], black[7], white_moves[7], black_moves[7];
  Position(const char *b) { LoadByteBoard(b); }
  Position() { Reset(); }

  void Reset() {
    move_color = 0;
    for (int i=PAWN; i<=KING; i++) white[i] = white_initial[i];
    for (int i=PAWN; i<=KING; i++) black[i] = black_initial[i];
    SetAll(WHITE);
    SetAll(BLACK);
  }

  void LoadByteBoard(const string &b) {
    for (int i=PAWN; i<=KING; i++) white[i] = ByteBoardToBitBoard(b, ByteBoardPieceSymbol(i, WHITE));
    for (int i=PAWN; i<=KING; i++) black[i] = ByteBoardToBitBoard(b, ByteBoardPieceSymbol(i, BLACK));
    SetAll(WHITE);
    SetAll(BLACK);
  }

  void SetAll(bool color) {
    BitBoard *pieces = Pieces(color);
    pieces[ALL] = pieces[PAWN] | pieces[KNIGHT] | pieces[BISHOP] | pieces[ROOK] | pieces[QUEEN] | pieces[KING];
  }

  BitBoard AllPieces() const { return white[ALL] | black[ALL]; }

  /**/  BitBoard *Pieces(bool color)       { return color ? black       : white;       }
  /**/  BitBoard *Moves (bool color)       { return color ? black_moves : white_moves; }
  const BitBoard *Pieces(bool color) const { return color ? black       : white;       }
  const BitBoard *Moves (bool color) const { return color ? black_moves : white_moves; }

  void SetSquare(int s, const pair<bool,int> &p) {
    BitBoard mask = (static_cast<BitBoard>(1) << s);
    CHECK_RANGE(p.second, 1, 7);
    if (p.first) { black[p.second] |= mask; black[0] |= mask; }
    else         { white[p.second] |= mask; white[0] |= mask; }
  }

  pair<bool,int> ClearSquare(int s) {
    BitBoard mask = (static_cast<BitBoard>(1) << s);
    for (int i=1; i<7; i++) if (white[i] & mask) { white[0] &= ~mask; white[i] &= ~mask; return make_pair(0, i); }
    for (int i=1; i<7; i++) if (black[i] & mask) { black[0] &= ~mask; black[i] &= ~mask; return make_pair(1, i); }
    return make_pair(false, 0);
  }
};

}; // namespace Chess
}; // namespace LFL
#include "magic.h"
namespace LFL {
namespace Chess {

static BitBoard PawnMoves(const Position &in, int p, bool black) {
  return 0;
  // return pawn_occupancy_mask[p] & ~in.Pieces(black)[ALL] | pawn_attack_mask[p] & in.Pieces(!black)[ALL];
}

static BitBoard KnightMoves(const Position &in, int p, bool black) {
  return knight_occupancy_mask[p] & ~in.Pieces(black)[ALL];
}

static BitBoard BishopMoves(const Position &in, int p, bool black) {
  static MagicMoves *magic_moves = Singleton<MagicMoves>::Get();
  BitBoard blockers = in.AllPieces() & bishop_occupancy_mask[p];
  return magic_moves->BishopMoves(p, blockers, in.Pieces(black)[ALL]);
}

static BitBoard RookMoves(const Position &in, int p, bool black) {
  static MagicMoves *magic_moves = Singleton<MagicMoves>::Get();
  BitBoard blockers = in.AllPieces() & rook_occupancy_mask[p];
  int magicIndex = MagicHash(p, blockers, rook_magic_number, rook_magic_number_bits);
  return magic_moves->rook_magic_moves[p][magicIndex] & ~in.Pieces(black)[ALL];
}

static BitBoard QueenMoves(const Position &in, int p, bool black) {
  return RookMoves(in, p, black) | BishopMoves(in, p, black);
}

static BitBoard KingMoves(const Position &in, int p, bool black) {
  return king_occupancy_mask[p] & ~in.Pieces(black)[ALL];
}

static bool InCheck(const Position &in, bool color) {
  return in.Moves(!color)[ALL] & in.Pieces(color)[KING];
}

}; // namespace Chess
}; // namespace LFL
#endif // #define LFL_CHESS_CHESS_H__
