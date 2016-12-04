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
  virtual const int *PieceTable(int piece) const = 0;
};

struct SimplifiedEvaluationPieceSquareTable : public PieceSquareTable {
  const int *PieceTable(int piece) const override {
    static int pawn_square_table[] = {
       0,  0,   0,   0,   0,   0,  0,  0,
      50, 50,  50,  50,  50,  50, 50, 50,
      10, 10,  20,  30,  30,  20, 10, 10,
       5,  5,  10,  25,  25,  10,  5,  5,
       0,  0,   0,  20,  20,   0,  0,  0,
       5, -5, -10,   0,   0, -10, -5,  5,
       5, 10,  10, -20, -20,  10, 10,  5,
       0,  0,   0,   0,   0,   0,  0,  0 
    };

    static int knight_square_table[] = {
      -50, -40, -30, -30, -30, -30, -40, -50,
      -40, -20,   0,   0,   0,   0, -20, -40,
      -30,   0,  10,  15,  15,  10,   0, -30,
      -30,   5,  15,  20,  20,  15,   5, -30,
      -30,   0,  15,  20,  20,  15,   0, -30,
      -30,   5,  10,  15,  15,  10,   5, -30,
      -40, -20,   0,   5,   5,   0, -20, -40,
      -50, -40, -30, -30, -30, -30, -40, -50,
    };

    static int bishop_square_table[] = {
      -20, -10, -10, -10, -10, -10, -10, -20,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   5,   5,  10,  10,   5,   5, -10,
      -10,   0,  10,  10,  10,  10,   0, -10,
      -10,  10,  10,  10,  10,  10,  10, -10,
      -10,   5,   0,   0,   0,   0,   5, -10,
      -20, -10, -10, -10, -10, -10, -10, -20,
    };

    static int rook_square_table[] = {
       0,  0,  0,  0,  0,  0,  0,  0,
       5, 10, 10, 10, 10, 10, 10,  5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
       0,  0,  0,  5,  5,  0,  0,  0
    };

    static int queen_square_table[] = {
      -20, -10, -10, -5, -5, -10, -10, -20,
      -10,   0,   0,  0,  0,   0,   0, -10,
      -10,   0,   5,  5,  5,   5,   0, -10,
       -5,   0,   5,  5,  5,   5,   0,  -5,
        0,   0,   5,  5,  5,   5,   0,  -5,
      -10,   5,   5,  5,  5,   5,   0, -10,
      -10,   0,   5,  0,  0,   0,   0, -10,
      -20, -10, -10, -5, -5, -10, -10, -20
    };

    switch(piece) {
      case PAWN:   return pawn_square_table;
      case KNIGHT: return knight_square_table;
      case BISHOP: return bishop_square_table;
      case ROOK:   return rook_square_table;
      case QUEEN:  return queen_square_table;
      default:     return nullptr;
    }
  }
};

}; // namespace Chess
}; // namespace LFL
#endif // LFL_CHESS_PST_H__
