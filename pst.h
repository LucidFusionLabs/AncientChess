/*
 * $Id: magic.h 1172 2014-05-11 18:16:55Z justin $
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

#ifndef LFL_CHESS_PST_H__
#define LFL_CHESS_PST_H__
namespace LFL {
namespace Chess {

struct PieceSquareTable {
  int piece_value[END_PIECES];
  PieceSquareTable() { memzero(piece_value); }
  virtual const int *MiddleGamePieceTable(int piece) const = 0;
  virtual const int *EndGamePieceTable   (int piece) const { return MiddleGamePieceTable(piece); }
};

// http://chessprogramming.wikispaces.com/Simplified+evaluation+function
struct SimplifiedEvaluationPieceSquareTable : public PieceSquareTable {
  SimplifiedEvaluationPieceSquareTable() {
    piece_value[PAWN]   = 100;
    piece_value[KNIGHT] = 320;
    piece_value[BISHOP] = 330;
    piece_value[ROOK]   = 500;
    piece_value[QUEEN]  = 900;
    piece_value[KING]   = 20000;
  }

  const int *MiddleGamePieceTable(int piece) const override {
    static int pawn_square_table[] = {
       0,  0,   0,   0,   0,   0,  0,  0,
       5, 10,  10, -20, -20,  10, 10,  5,
       5, -5, -10,   0,   0, -10, -5,  5,
       0,  0,   0,  20,  20,   0,  0,  0,
       5,  5,  10,  25,  25,  10,  5,  5,
      10, 10,  20,  30,  30,  20, 10, 10,
      50, 50,  50,  50,  50,  50, 50, 50,
       0,  0,   0,   0,   0,   0,  0,  0,
    };

    static int knight_square_table[] = {
      -50, -40, -30, -30, -30, -30, -40, -50,
      -40, -20,   0,   5,   5,   0, -20, -40,
      -30,   5,  10,  15,  15,  10,   5, -30,
      -30,   0,  15,  20,  20,  15,   0, -30,
      -30,   5,  15,  20,  20,  15,   5, -30,
      -30,   0,  10,  15,  15,  10,   0, -30,
      -40, -20,   0,   0,   0,   0, -20, -40,
      -50, -40, -30, -30, -30, -30, -40, -50,
    };

    static int bishop_square_table[] = {
      -20, -10, -10, -10, -10, -10, -10, -20,
      -10,   5,   0,   0,   0,   0,   5, -10,
      -10,  10,  10,  10,  10,  10,  10, -10,
      -10,   0,  10,  10,  10,  10,   0, -10,
      -10,   5,   5,  10,  10,   5,   5, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -20, -10, -10, -10, -10, -10, -10, -20,
    };

    static int rook_square_table[] = {
       0,  0,  0,  5,  5,  0,  0,  0,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
       5, 10, 10, 10, 10, 10, 10,  5,
       0,  0,  0,  0,  0,  0,  0,  0,
    };

    static int queen_square_table[] = {
      -20, -10, -10, -5, -5, -10, -10, -20,
      -10,   0,   5,  0,  0,   0,   0, -10,
      -10,   5,   5,  5,  5,   5,   0, -10,
        0,   0,   5,  5,  5,   5,   0,  -5,
       -5,   0,   5,  5,  5,   5,   0,  -5,
      -10,   0,   5,  5,  5,   5,   0, -10,
      -10,   0,   0,  0,  0,   0,   0, -10,
      -20, -10, -10, -5, -5, -10, -10, -20,
    };

    static const int *ret[7] = { nullptr, pawn_square_table, knight_square_table,
      bishop_square_table, rook_square_table, queen_square_table, nullptr };

    return ret[piece];
  }
};

// http://www.talkchess.com/forum/viewtopic.php?topic_view=threads&p=551989&t=50840
struct AdamHairPieceSquareTable : public PieceSquareTable {
  AdamHairPieceSquareTable() {
    piece_value[PAWN]   = 100;
    piece_value[KNIGHT] = 300;
    piece_value[BISHOP] = 300;
    piece_value[ROOK]   = 500;
    piece_value[QUEEN]  = 950;
  }

  const int *MiddleGamePieceTable(int piece) const override {
    static int pawn_square_table[] = {
       0,   0,   0,   0,   0,   0,   0,   0,  
      -5,   3,   5, -13, -35, -11,  -7,  -1,  
      10,  -4,  -7,  -6, -19,  -6,   1,   1,  
       7,  10,   4,   5,   4,   8,  14,   1,  
      11,  17,  23,  31,  31,  23,  30,   9,  
      11,  71,  95,  77,  56,  72,  54,  21,  
      22, -16,  82, 107, 168, 173, 121, 118,  
       0,   0,   0,   0,   0,   0,   0,   0,  
    }; 

    static const int knight_square_table[] = { 
      -81, -61, -19, -29, -64, -66, -30, -99,
      -11, -42, -20,  -7,  -1, -28, -31, -56,
      -42,   3,   3,   8,  14,   0, -16, -38,
       -7,  33,  12,  19,   3,   2,   0, -14,
       43,  14,  33,  10,  33,  25,  -4, -14,
        6,  55, 143, 124,  64,  60,  18, -22,
       29,   2, 122,  60,  74,  54,  24, -34,
        0,   0,   0,   0,   0,   0,   0, -60,
    }; 

    static const int bishop_square_table[] = { 
      -67, -45,  -8, -31, -37, -8,  12,  -7, 
       15,   0,   2,   1, -10, 13,   5,  15, 
        4,   3,  -1,  10,  13, 14,  12,   5, 
        4,  17,   8,  21,  32, 23,   5,   1, 
        4,  17,  27,  37,  27, 29,  16,  -1, 
       44,  53, 108,  91,  56, 20,  27,   7, 
       11,  69,  61,  65,  58, 30, -23, -24, 
        0,   0,   0,   0,   0,  0,   0,   0, 
    }; 

    static const int rook_square_table[] = { 
       -8,  4,   1,   2,  1,  3, -1,  -2,
      -29, -1, -10,   2, -2,  2, -6, -26,
        3, 12,  -1,   8, -3,  3,  0, -16,
      -13, 13, -17,  18, 14,  8, -5,  -9,
       16, 53,  39,  53, 57, 46, 33,  19,
       75, 85, 144, 134, 75, 54, 83,  24,
      104, 70,  89,  91, 62, 64, 33,  46,
      153,  0,   0, 124, 37,  0,  0,  84,
    }; 

    static const int queen_square_table[] = { 
      -13, -83, -51, -15,  3, -11, -10,   1, 
       -2,  -7, -10,  -1,  5,   2,   3,  -7, 
       -6,   7,  11,   8,  2,  12,   0, -11, 
        4,  26,  17,  18,  9,   7,   5,  -9, 
       12,  26,   9,  32, 25,  15,   0,  -6, 
       26,  15,  30,  37, 25,  13,  10, -16, 
       57,  39,  55,  16,  0,  35,  11,   1, 
      102,   0,   0,  29,  0, -42,   6, -13, 
    }; 

    static const int king_square_table[] = { 
       0, 25, -9,  0, -9,  0,  0,  0,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
      -9, -9, -9, -9, -9, -9, -9, -9,
    }; 

    static const int *ret[7] = { nullptr, pawn_square_table, knight_square_table,
      bishop_square_table, rook_square_table, queen_square_table, nullptr };

    return ret[piece];
  }

  const int *EndGamePieceTable(int piece) const override {
    static const int pawn_square_table[] = { 
        0,   0,   0,   0,   0,   0,   0,   0,
      -17, -17, -17, -17, -17, -17, -17, -17,
      -11, -11, -11, -11, -11, -11, -11, -11,
       -7,  -7,  -7,  -7,  -7,  -7,  -7,  -7,
       16,  16,  16,  16,  16,  16,  16,  16,
       55,  55,  55,  55,  55,  55,  55,  55,
       82,  82,  82,  82,  82,  82,  82,  82,
        0,   0,   0,   0,   0,   0,   0,   0,
    }; 

    static const int knight_square_table[] = { 
      -99, -99, -94, -88, -88, -94, -99, -99,
      -81, -62, -49, -43, -43, -49, -62, -81,
      -46, -27, -15,  -9,  -9, -15, -27, -46,
      -22,  -3,  10,  16,  16,  10,  -3, -22,
       -7,  12,  25,  31,  31,  25,  12,  -7,
       -2,  17,  30,  36,  36,  30,  17,  -2,
       -7,  12,  25,  31,  31,  25,  12,  -7,
      -21,  -3,  10,  16,  16,  10,  -3, -21,
    }; 

    static const int bishop_square_table[] = { 
      -27, -21, -17, -15, -15, -17, -21, -27,
      -10,  -4,   0,   2,   2,   0,  -4, -10,
        2,   8,  12,  14,  14,  12,   8,   2,
       11,  17,  21,  23,  23,  21,  17,  11,
       14,  20,  24,  26,  26,  24,  20,  14,
       13,  19,  23,  25,  25,  23,  19,  13,
        8,  14,  18,  20,  20,  18,  14,   8,
       -2,   4,   8,  10,  10,   8,   4,  -2,
    }; 

    static const int rook_square_table[] = { 
      -32, -31, -30, -29, -29, -30, -31, -32,
      -27, -25, -24, -24, -24, -24, -25, -27,
      -15, -13, -12, -12, -12, -12, -13, -15,
        1,   2,   3,   4,   4,   3,   2,   1,
       15,  17,  18,  18,  18,  18,  17,  15,
       25,  27,  28,  28,  28,  28,  27,  25,
       27,  28,  29,  30,  30,  29,  28,  27,
       16,  17,  18,  19,  19,  18,  17,  16,
    }; 

    static const int queen_square_table[] = { 
      -61, -55, -52, -50, -50, -52, -55, -61,
      -31, -26, -22, -21, -21, -22, -26, -31,
       -8,  -3,   1,   3,   3,   1,  -3,  -8,
        9,  14,  17,  19,  19,  17,  14,   9,
       19,  24,  28,  30,  30,  28,  24,  19,
       23,  28,  32,  34,  34,  32,  28,  23,
       21,  26,  30,  31,  31,  30,  26,  21,
       12,  17,  21,  23,  23,  21,  17,  12,
    }; 

    static const int king_square_table[] = { 
      -34, -30, -28, -27, -27, -28, -30, -34,
      -17, -13, -11, -10, -10, -11, -13, -17,
       -2,   2,   4,   5,   5,   4,   2,  -2,
       11,  15,  17,  18,  18,  17,  15,  11,
       22,  26,  28,  29,  29,  28,  26,  22,
       31,  34,  37,  38,  38,  37,  34,  31,
       38,  41,  44,  45,  45,  44,  41,  38,
       42,  46,  48,  50,  50,  48,  46,  42,
    }; 

    static const int *ret[7] = { nullptr, pawn_square_table, knight_square_table,
      bishop_square_table, rook_square_table, queen_square_table, nullptr };

    return ret[piece];
  }
};

}; // namespace Chess
}; // namespace LFL
#endif // LFL_CHESS_PST_H__
