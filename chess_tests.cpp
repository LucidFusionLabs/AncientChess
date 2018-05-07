#include "gtest/gtest.h"
#include "core/app/app.h"
#include "core/app/ipc.h"

namespace LFL {
Application *app;
};

#include "chess.h"

using namespace LFL;
using namespace LFL::Chess;

extern "C" LFApp *MyAppCreate(int argc, const char* const* argv) {
  app = CreateApplication(argc, argv).release();
  app->focused = CreateWindow(app).release();
  return app;
}

extern "C" int MyAppMain() {
  testing::InitGoogleTest(&app->argc, const_cast<char**>(app->argv));
  LFL::FLAGS_font = LFL::FakeFontEngine::Filename();
  CHECK_EQ(0, LFL::app->Create(__FILE__));
  exit(RUN_ALL_TESTS());
}

TEST(SquaresTests, Names) {
  EXPECT_EQ(a1, SquareID("a1"));  EXPECT_EQ(b1, SquareID("b1"));  EXPECT_EQ(c1, SquareID("c1"));  EXPECT_EQ(d1, SquareID("d1"));
  EXPECT_EQ(a2, SquareID("a2"));  EXPECT_EQ(b2, SquareID("b2"));  EXPECT_EQ(c2, SquareID("c2"));  EXPECT_EQ(d2, SquareID("d2"));
  EXPECT_EQ(a3, SquareID("a3"));  EXPECT_EQ(b3, SquareID("b3"));  EXPECT_EQ(c3, SquareID("c3"));  EXPECT_EQ(d3, SquareID("d3"));
  EXPECT_EQ(a4, SquareID("a4"));  EXPECT_EQ(b4, SquareID("b4"));  EXPECT_EQ(c4, SquareID("c4"));  EXPECT_EQ(d4, SquareID("d4"));
  EXPECT_EQ(a5, SquareID("a5"));  EXPECT_EQ(b5, SquareID("b5"));  EXPECT_EQ(c5, SquareID("c5"));  EXPECT_EQ(d5, SquareID("d5"));
  EXPECT_EQ(a6, SquareID("a6"));  EXPECT_EQ(b6, SquareID("b6"));  EXPECT_EQ(c6, SquareID("c6"));  EXPECT_EQ(d6, SquareID("d6"));
  EXPECT_EQ(a7, SquareID("a7"));  EXPECT_EQ(b7, SquareID("b7"));  EXPECT_EQ(c7, SquareID("c7"));  EXPECT_EQ(d7, SquareID("d7"));
  EXPECT_EQ(a8, SquareID("a8"));  EXPECT_EQ(b8, SquareID("b8"));  EXPECT_EQ(c8, SquareID("c8"));  EXPECT_EQ(d8, SquareID("d8"));
  EXPECT_EQ(e1, SquareID("e1"));  EXPECT_EQ(f1, SquareID("f1"));  EXPECT_EQ(g1, SquareID("g1"));  EXPECT_EQ(h1, SquareID("h1"));
  EXPECT_EQ(e2, SquareID("e2"));  EXPECT_EQ(f2, SquareID("f2"));  EXPECT_EQ(g2, SquareID("g2"));  EXPECT_EQ(h2, SquareID("h2"));
  EXPECT_EQ(e3, SquareID("e3"));  EXPECT_EQ(f3, SquareID("f3"));  EXPECT_EQ(g3, SquareID("g3"));  EXPECT_EQ(h3, SquareID("h3"));
  EXPECT_EQ(e4, SquareID("e4"));  EXPECT_EQ(f4, SquareID("f4"));  EXPECT_EQ(g4, SquareID("g4"));  EXPECT_EQ(h4, SquareID("h4"));
  EXPECT_EQ(e5, SquareID("e5"));  EXPECT_EQ(f5, SquareID("f5"));  EXPECT_EQ(g5, SquareID("g5"));  EXPECT_EQ(h5, SquareID("h5"));
  EXPECT_EQ(e6, SquareID("e6"));  EXPECT_EQ(f6, SquareID("f6"));  EXPECT_EQ(g6, SquareID("g6"));  EXPECT_EQ(h6, SquareID("h6"));
  EXPECT_EQ(e7, SquareID("e7"));  EXPECT_EQ(f7, SquareID("f7"));  EXPECT_EQ(g7, SquareID("g7"));  EXPECT_EQ(h7, SquareID("h7"));
  EXPECT_EQ(e8, SquareID("e8"));  EXPECT_EQ(f8, SquareID("f8"));  EXPECT_EQ(g8, SquareID("g8"));  EXPECT_EQ(h8, SquareID("h8"));
}

TEST(SquaresTests, MaskString) {
  EXPECT_EQ("10000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(a8));
  EXPECT_EQ("01000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(b8));
  EXPECT_EQ("00100000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(c8));
  EXPECT_EQ("00010000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(d8));
  EXPECT_EQ("00001000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(e8));
  EXPECT_EQ("00000100\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(f8));
  EXPECT_EQ("00000010\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(g8));
  EXPECT_EQ("00000001\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(h8));

  EXPECT_EQ("00000000\n10000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(a7));
  EXPECT_EQ("00000000\n01000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(b7));
  EXPECT_EQ("00000000\n00100000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(c7));
  EXPECT_EQ("00000000\n00010000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(d7));
  EXPECT_EQ("00000000\n00001000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(e7));
  EXPECT_EQ("00000000\n00000100\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(f7));
  EXPECT_EQ("00000000\n00000010\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(g7));
  EXPECT_EQ("00000000\n00000001\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(h7));

  EXPECT_EQ("00000000\n00000000\n10000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(a6));
  EXPECT_EQ("00000000\n00000000\n01000000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(b6));
  EXPECT_EQ("00000000\n00000000\n00100000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(c6));
  EXPECT_EQ("00000000\n00000000\n00010000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(d6));
  EXPECT_EQ("00000000\n00000000\n00001000\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(e6));
  EXPECT_EQ("00000000\n00000000\n00000100\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(f6));
  EXPECT_EQ("00000000\n00000000\n00000010\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(g6));
  EXPECT_EQ("00000000\n00000000\n00000001\n00000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(h6));

  EXPECT_EQ("00000000\n00000000\n00000000\n10000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(a5));
  EXPECT_EQ("00000000\n00000000\n00000000\n01000000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(b5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00100000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(c5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00010000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(d5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00001000\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(e5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000100\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(f5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000010\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(g5));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000001\n00000000\n00000000\n00000000\n00000000\n", SquareMaskString(h5));
                                 
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n10000000\n00000000\n00000000\n00000000\n", SquareMaskString(a4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n01000000\n00000000\n00000000\n00000000\n", SquareMaskString(b4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00100000\n00000000\n00000000\n00000000\n", SquareMaskString(c4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00010000\n00000000\n00000000\n00000000\n", SquareMaskString(d4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00001000\n00000000\n00000000\n00000000\n", SquareMaskString(e4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000100\n00000000\n00000000\n00000000\n", SquareMaskString(f4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000010\n00000000\n00000000\n00000000\n", SquareMaskString(g4));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000001\n00000000\n00000000\n00000000\n", SquareMaskString(h4));
                                           
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n10000000\n00000000\n00000000\n", SquareMaskString(a3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n01000000\n00000000\n00000000\n", SquareMaskString(b3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00100000\n00000000\n00000000\n", SquareMaskString(c3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00010000\n00000000\n00000000\n", SquareMaskString(d3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00001000\n00000000\n00000000\n", SquareMaskString(e3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000100\n00000000\n00000000\n", SquareMaskString(f3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000010\n00000000\n00000000\n", SquareMaskString(g3));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000001\n00000000\n00000000\n", SquareMaskString(h3));
                                           
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n10000000\n00000000\n", SquareMaskString(a2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n01000000\n00000000\n", SquareMaskString(b2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00100000\n00000000\n", SquareMaskString(c2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00010000\n00000000\n", SquareMaskString(d2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00001000\n00000000\n", SquareMaskString(e2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000100\n00000000\n", SquareMaskString(f2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000010\n00000000\n", SquareMaskString(g2));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000001\n00000000\n", SquareMaskString(h2));
                                           
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n10000000\n", SquareMaskString(a1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n01000000\n", SquareMaskString(b1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00100000\n", SquareMaskString(c1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00010000\n", SquareMaskString(d1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00001000\n", SquareMaskString(e1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000100\n", SquareMaskString(f1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000010\n", SquareMaskString(g1));
  EXPECT_EQ("00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000001\n", SquareMaskString(h1));
}

TEST(SquaresTests, Mask) {
  for (int p=0; p<64; p++)
    EXPECT_EQ(SquareMaskString(p), BitBoardToString(SquareMask(p)));
}

TEST(OccupancyMaskTest, Rook) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    for (int i=p+8; i<56;                    i+=8) mask |= (1ULL<<i);
    for (int i=p-8; i>=8;                    i-=8) mask |= (1ULL<<i);
    for (int i=p+1; i%8!=7 && i%8!=0;         i++) mask |= (1ULL<<i);
    for (int i=p-1; i%8!=7 && i%8!=0 && i>=0; i--) mask |= (1ULL<<i);
    EXPECT_EQ(rook_occupancy_mask[p], mask);
  }
}

TEST(OccupancyMaskTest, Bishop) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    for (int i=p+9; i%8!=7 && i%8!=0 && i< 56; i+=9) mask |= (1ULL<<i);
    for (int i=p-9; i%8!=7 && i%8!=0 && i>= 8; i-=9) mask |= (1ULL<<i);
    for (int i=p+7; i%8!=7 && i%8!=0 && i< 56; i+=7) mask |= (1ULL<<i);
    for (int i=p-7; i%8!=7 && i%8!=0 && i>= 8; i-=7) mask |= (1ULL<<i);
    EXPECT_EQ(bishop_occupancy_mask[p], mask);
  }
}

TEST(OccupancyMaskTest, Knight) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (p%8!=6 && p%8!=7 && p+10 < 64) mask |= (1ULL<<(p+10));
    if (          p%8!=7 && p+17 < 64) mask |= (1ULL<<(p+17));
    if (p%8!=1 && p%8!=0 && p+ 6 < 64) mask |= (1ULL<<(p+ 6));
    if (          p%8!=0 && p+15 < 64) mask |= (1ULL<<(p+15));
    if (p%8!=1 && p%8!=0 && p-10 >= 0) mask |= (1ULL<<(p-10));
    if (          p%8!=0 && p-17 >= 0) mask |= (1ULL<<(p-17));
    if (p%8!=6 && p%8!=7 && p- 6 >= 0) mask |= (1ULL<<(p- 6));
    if (          p%8!=7 && p-15 >= 0) mask |= (1ULL<<(p-15));
    EXPECT_EQ(knight_occupancy_mask[p], mask);
  }
}

TEST(OccupancyMaskTest, King) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (            p+8 < 64) mask |= (1ULL<<(p+8));
    if (            p-8 >= 0) mask |= (1ULL<<(p-8));
    if (p%8 != 0 && p-1 >= 0) mask |= (1ULL<<(p-1));
    if (p%8 != 0 && p-9 >= 0) mask |= (1ULL<<(p-9));
    if (p%8 != 0 && p+7 < 64) mask |= (1ULL<<(p+7));
    if (p%8 != 7 && p+1 < 64) mask |= (1ULL<<(p+1));
    if (p%8 != 7 && p+9 < 64) mask |= (1ULL<<(p+9));
    if (p%8 != 7 && p-7 >= 0) mask |= (1ULL<<(p-7));
    EXPECT_EQ(king_occupancy_mask[p], mask);
  }
  EXPECT_EQ(SquareMask(f1) | SquareMask(g1), white_castle_path);
  EXPECT_EQ(SquareMask(c1) | SquareMask(d1), white_castle_long_path);
  EXPECT_EQ(SquareMask(f8) | SquareMask(g8), black_castle_path);
  EXPECT_EQ(SquareMask(c8) | SquareMask(d8), black_castle_long_path);
}

TEST(OccupancyMaskTest, WhitePawn) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (          p+8 < 64) mask |= (1ULL<<(p+8));
    if (p >= 8 && p   < 16) mask |= (1ULL<<(p+16));
    EXPECT_EQ(white_pawn_occupancy_mask[p], mask);
  }
}

TEST(OccupancyMaskTest, BlackPawn) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (p-8 >= 0)            mask |= (1ULL<<(p-8));
    if (p   >= 48 && p < 56) mask |= (1ULL<<(p-16));
    EXPECT_EQ(black_pawn_occupancy_mask[p], mask);
  }
}

TEST(AttackMaskTest, WhitePawn) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (p%8 != 7 && p+9 < 64) mask |= (1ULL<<(p+9));
    if (p%8 != 0 && p+7 < 64) mask |= (1ULL<<(p+7));
    EXPECT_EQ(white_pawn_attack_mask[p], mask);
  }
}

TEST(AttackMaskTest, BlackPawn) {
  for (int p=0; p<64; p++) {
    BitBoard mask = 0;
    if (p%8 != 0 && p-9 >= 0) mask |= (1ULL<<(p-9));
    if (p%8 != 7 && p-7 >= 0) mask |= (1ULL<<(p-7));
    EXPECT_EQ(black_pawn_attack_mask[p], mask);
  }
}

TEST(BoardTest, ByteBoard) {
  for (int i=0; i<64; i++) {
    EXPECT_EQ(  rook_occupancy_mask[i], BitBoardFromString(BitBoardToString(  rook_occupancy_mask[i]).c_str()));
    EXPECT_EQ(bishop_occupancy_mask[i], BitBoardFromString(BitBoardToString(bishop_occupancy_mask[i]).c_str()));
  }

  EXPECT_EQ(white_initial[Chess::PAWN],   ByteBoardToBitBoard(initial_byte_board, 'P'));
  EXPECT_EQ(white_initial[Chess::KNIGHT], ByteBoardToBitBoard(initial_byte_board, 'N'));
  EXPECT_EQ(white_initial[Chess::BISHOP], ByteBoardToBitBoard(initial_byte_board, 'B'));
  EXPECT_EQ(white_initial[Chess::ROOK],   ByteBoardToBitBoard(initial_byte_board, 'R'));
  EXPECT_EQ(white_initial[Chess::QUEEN],  ByteBoardToBitBoard(initial_byte_board, 'Q'));
  EXPECT_EQ(white_initial[Chess::KING],   ByteBoardToBitBoard(initial_byte_board, 'K'));

  EXPECT_EQ(black_initial[Chess::PAWN],   ByteBoardToBitBoard(initial_byte_board, 'p'));
  EXPECT_EQ(black_initial[Chess::KNIGHT], ByteBoardToBitBoard(initial_byte_board, 'n'));
  EXPECT_EQ(black_initial[Chess::BISHOP], ByteBoardToBitBoard(initial_byte_board, 'b'));
  EXPECT_EQ(black_initial[Chess::ROOK],   ByteBoardToBitBoard(initial_byte_board, 'r'));
  EXPECT_EQ(black_initial[Chess::QUEEN],  ByteBoardToBitBoard(initial_byte_board, 'q'));
  EXPECT_EQ(black_initial[Chess::KING],   ByteBoardToBitBoard(initial_byte_board, 'k'));

  Chess::Position position = Chess::Position::FromByteBoard
    ("--------\n"
     "-kp-----\n"                   
     "-pp-n---\n"                   
     "p---qp--\n"                   
     "--------\n"                   
     "--PP--P-\n"                   
     "PPK-NQ--\n"                   
     "--------\n");

  EXPECT_EQ(BitBoardFromString
            ("00000001\n"
             "00000010\n"
             "00010100\n"
             "01110000\n"
             "00011100\n"
             "00101010\n"
             "00001000\n"
             "00000000\n"), position.QueenMoves(Chess::e5, Chess::BLACK));
}

TEST(BoardTest, ForsythEdwardsNotation) {
  Chess::Position initial, position;
  EXPECT_EQ(true, position.LoadFEN(initial_fen));
  EXPECT_EQ(initial_byte_board, position.GetByteBoard());
  EXPECT_EQ(initial_fen, position.GetFEN()); 
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(0, position.flags.w_cant_castle_long);
  EXPECT_EQ(0, position.flags.b_cant_castle_long);
  EXPECT_EQ(0, position.flags.w_cant_castle);
  EXPECT_EQ(0, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(0, position.move_number);

  EXPECT_EQ(true, position.LoadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"));
  EXPECT_EQ(kiwipete_byte_board, position.GetByteBoard());
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(0, position.flags.w_cant_castle_long);
  EXPECT_EQ(0, position.flags.b_cant_castle_long);
  EXPECT_EQ(0, position.flags.w_cant_castle);
  EXPECT_EQ(0, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(0, position.move_number);

  EXPECT_EQ(true, position.LoadFEN(perft_pos3_fen));
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(1, position.flags.w_cant_castle_long);
  EXPECT_EQ(1, position.flags.b_cant_castle_long);
  EXPECT_EQ(1, position.flags.w_cant_castle);
  EXPECT_EQ(1, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(0, position.move_number);

  EXPECT_EQ(true, position.LoadFEN(perft_pos4_fen));
  EXPECT_EQ(perft_pos4_fen, position.GetFEN()); 
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(1, position.flags.w_cant_castle_long);
  EXPECT_EQ(0, position.flags.b_cant_castle_long);
  EXPECT_EQ(1, position.flags.w_cant_castle);
  EXPECT_EQ(0, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(0, position.move_number);

  EXPECT_EQ(true, position.LoadFEN(perft_pos4_mirror_fen));
  EXPECT_EQ(perft_pos4_mirror_fen, position.GetFEN()); 
  EXPECT_EQ(BLACK, position.flags.to_move_color);
  EXPECT_EQ(0, position.flags.w_cant_castle_long);
  EXPECT_EQ(1, position.flags.b_cant_castle_long);
  EXPECT_EQ(0, position.flags.w_cant_castle);
  EXPECT_EQ(1, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(1, position.move_number);

  EXPECT_EQ(true, position.LoadFEN(perft_pos5_fen));
  EXPECT_EQ(perft_pos5_fen, position.GetFEN()); 
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(0, position.flags.w_cant_castle_long);
  EXPECT_EQ(1, position.flags.b_cant_castle_long);
  EXPECT_EQ(0, position.flags.w_cant_castle);
  EXPECT_EQ(1, position.flags.b_cant_castle);
  EXPECT_EQ(1, position.flags.fifty_move_rule_count);
  EXPECT_EQ(14, position.move_number);

  EXPECT_EQ(true, position.LoadFEN(perft_pos6_fen));
  EXPECT_EQ(perft_pos6_fen, position.GetFEN()); 
  EXPECT_EQ(WHITE, position.flags.to_move_color);
  EXPECT_EQ(1, position.flags.w_cant_castle_long);
  EXPECT_EQ(1, position.flags.b_cant_castle_long);
  EXPECT_EQ(1, position.flags.w_cant_castle);
  EXPECT_EQ(1, position.flags.b_cant_castle);
  EXPECT_EQ(0, position.flags.fifty_move_rule_count);
  EXPECT_EQ(18, position.move_number);
}

TEST(MoveTest, Encoding) {
  for (int i=0; i<128; i++) {
    uint8_t from_square = Rand(0, 64-1), to_square = Rand(0, 64-1);
    uint8_t piece = Rand(0, int(END_PIECES)-1), capture = Rand(0, int(END_PIECES)-1), promote = Rand(0, int(END_PIECES)-1);
    bool killer = Rand(0, 1), check = Rand(0, 1), castle = Rand(0, 1), double_step = Rand(0, 1), enpassant = Rand(0,1);
    int flags = 0 | (killer ? MoveFlag::Killer : 0) | (check ? MoveFlag::Check : 0) | (castle ? MoveFlag::Castle : 0) 
      | (double_step ? MoveFlag::DoubleStepPawn : 0) | (enpassant ? MoveFlag::EnPassant : 0);

    Chess::Move move = GetMove(piece, from_square, to_square, capture, promote, flags);
    EXPECT_EQ(piece,       GetMovePieceType(move));
    EXPECT_EQ(from_square, GetMoveFromSquare(move));
    EXPECT_EQ(to_square,   GetMoveToSquare(move));
    EXPECT_EQ(capture,     GetMoveCapture(move));
    EXPECT_EQ(promote,     GetMovePromotion(move));
    EXPECT_EQ(killer,      ((move & MoveFlag::Killer)         != 0));
    EXPECT_EQ(check,       ((move & MoveFlag::Check)          != 0));
    EXPECT_EQ(castle,      ((move & MoveFlag::Castle)         != 0));
    EXPECT_EQ(double_step, ((move & MoveFlag::DoubleStepPawn) != 0));
    EXPECT_EQ(enpassant,   ((move & MoveFlag::EnPassant)      != 0));
  }
}

#if 0
TEST(MoveTest, Sorting) {
  Position position;
  EXPECT_EQ(true, position.LoadFEN("5r2/p1NP1ppn/5kn1/K1Q2N2/4br2/2p2p2/2B5/2B1R1R1 w - - 0 40"));
  auto moves = GenerateMoves(position, position.flags.to_move_color);
  sort(moves.begin(), moves.end(), MoveSort);
  for (auto &m : moves) INFO(GetLongMoveName(m));
}
#endif

TEST(MoveTest, Hashing) {
  Position position;
  static const vector<ZobristHasher::Hash> &zobrist = Singleton<ZobristHasher>::Get()->data;
  EXPECT_EQ(Singleton<ZobristHasher>::Get()->initial_position_hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, e2, e4, 0, 0, MoveFlag::DoubleStepPawn));
  EXPECT_EQ(Position("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, e7, e5, 0, 0, MoveFlag::DoubleStepPawn));
  EXPECT_EQ(Position("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KNIGHT, g1, f3, 0, 0, 0));
  EXPECT_EQ(Position("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 0 2").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KNIGHT, b8, c6, 0, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 2").hash, position.hash);
  position.ApplyValidatedMove(GetMove(BISHOP, f1, b5, 0, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 3").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, a7, a6, 0, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1ppp1ppp/p1n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 3").hash, position.hash);
  position.ApplyValidatedMove(GetMove(BISHOP, b5, c6, KNIGHT, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1ppp1ppp/p1B5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, d7, c6, BISHOP, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p1p5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 4").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KING, e1, g1, 0, 0, MoveFlag::Castle));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p1p5/4p3/4P3/5N2/PPPP1PPP/RNBQ1RK1 b kq - 0 5").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, c6, c5, 0, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p7/2p1p3/4P3/5N2/PPPP1PPP/RNBQ1RK1 w kq - 0 5").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KNIGHT, f3, e5, PAWN, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p7/2p1N3/4P3/8/PPPP1PPP/RNBQ1RK1 b kq - 0 6").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, c5, c4, 0, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p7/4N3/2p1P3/8/PPPP1PPP/RNBQ1RK1 w kq - 0 6").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, d2, d4, 0, 0, MoveFlag::DoubleStepPawn));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p7/4N3/2pPP3/8/PPP2PPP/RNBQ1RK1 b kq - 0 7").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, c4, d3, PAWN, 0, MoveFlag::EnPassant));
  EXPECT_EQ(Position("r1bqkbnr/1pp2ppp/p7/4N3/4P3/3p4/PPP2PPP/RNBQ1RK1 w kq - 0 7").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KNIGHT, e5, f7, PAWN, 0, 0));
  EXPECT_EQ(Position("r1bqkbnr/1pp2Npp/p7/8/4P3/3p4/PPP2PPP/RNBQ1RK1 b kq - 0 8").hash, position.hash);
  position.ApplyValidatedMove(GetMove(KING, e8, f7, KNIGHT, 0, 0));
  EXPECT_EQ(Position("r1bq1bnr/1pp2kpp/p7/8/4P3/3p4/PPP2PPP/RNBQ1RK1 w - - 0 8").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, e4, e5, 0, 0, 0));
  EXPECT_EQ(Position("r1bq1bnr/1pp2kpp/p7/4P3/8/3p4/PPP2PPP/RNBQ1RK1 b - - 0 9").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, d3, c2, PAWN, 0, 0));
  EXPECT_EQ(Position("r1bq1bnr/1pp2kpp/p7/4P3/8/8/PPp2PPP/RNBQ1RK1 w - - 0 9").hash, position.hash);
  position.ApplyValidatedMove(GetMove(QUEEN, d1, d8, QUEEN, 0, 0));
  EXPECT_EQ(Position("r1bQ1bnr/1pp2kpp/p7/4P3/8/8/PPp2PPP/RNB2RK1 b - - 0 10").hash, position.hash);
  position.ApplyValidatedMove(GetMove(PAWN, c2, b1, KNIGHT, QUEEN, 0));
  EXPECT_EQ(Position("r1bQ1bnr/1pp2kpp/p7/4P3/8/8/PP3PPP/RqB2RK1 w - - 0 10").hash, position.hash);
}

#define CHESS_PERFT_TESTS
#ifdef  CHESS_PERFT_TESTS
TEST(Perft, InitialPosition) {
  // Depth Nodes     Captures E.p. Castles Promotions Checks Checkmates
  // 1     20        0        0    0       0          0      0
  // 2     400       0        0    0       0          0      0
  // 3     8902      34       0    0       0          12     0
  // 4     197281    1576     0    0       0          469    8
  // 5     4865609   82719    258  0       0          27351  347
  // 6     119060324 2812008  5248 0       0          809099 10828
  vector<SearchStats::Total> depth_total;
  Position position;
  SearchStats search;
  search.max_depth = 6;
  search.depth_total = &depth_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(6, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(20,        d->nodes); EXPECT_EQ(0,       d->captures); EXPECT_EQ(0,    d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(0,      d->checks); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(400,       d->nodes); EXPECT_EQ(0,       d->captures); EXPECT_EQ(0,    d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(0,      d->checks); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(8902,      d->nodes); EXPECT_EQ(34,      d->captures); EXPECT_EQ(0,    d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(12,     d->checks); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(197281,    d->nodes); EXPECT_EQ(1576,    d->captures); EXPECT_EQ(0,    d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(469,    d->checks); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(4865609,   d->nodes); EXPECT_EQ(82719,   d->captures); EXPECT_EQ(258,  d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(27351,  d->checks); }
  if (auto d = VectorGet(depth_total, 5)) { EXPECT_EQ(119060324, d->nodes); EXPECT_EQ(2812008, d->captures); EXPECT_EQ(5248, d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0, d->promotions); EXPECT_EQ(809099, d->checks); }
}

TEST(Perft, Kiwipete) {
  // Depth Nodes     Captures E.p.  Castles Promotions Checks  Checkmates
  // 1     48        8        0     2       0          0       0
  // 2     2039      351      1     91      0          3       0
  // 3     97862     17102    45    3162    0          993     1
  // 4     4085603   757163   1929  128013  15172      25523   43
  // 5     193690690 35043416 73365 4993637 8392       3309887 30171
  vector<SearchStats::Total> depth_total;
  unordered_map<Chess::Move, SearchStats::Total> divide_total;
  Position position = Position::FromByteBoard(kiwipete_byte_board);
  SearchStats search;
  search.max_depth = 5;
  search.depth_total = &depth_total;
  // search.divide_total = &divide_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(5, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(48,        d->nodes); EXPECT_EQ(8,        d->captures); EXPECT_EQ(0    , d->enpassants); EXPECT_EQ(2      , d->castles); EXPECT_EQ(0    , d->promotions); EXPECT_EQ(0      , d->checks); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(2039,      d->nodes); EXPECT_EQ(351,      d->captures); EXPECT_EQ(1    , d->enpassants); EXPECT_EQ(91     , d->castles); EXPECT_EQ(0    , d->promotions); EXPECT_EQ(3      , d->checks); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(97862,     d->nodes); EXPECT_EQ(17102,    d->captures); EXPECT_EQ(45   , d->enpassants); EXPECT_EQ(3162   , d->castles); EXPECT_EQ(0    , d->promotions); EXPECT_EQ(993    , d->checks); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(4085603,   d->nodes); EXPECT_EQ(757163,   d->captures); EXPECT_EQ(1929 , d->enpassants); EXPECT_EQ(128013 , d->castles); EXPECT_EQ(15172, d->promotions); EXPECT_EQ(25523  , d->checks); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(193690690, d->nodes); EXPECT_EQ(35043416, d->captures); EXPECT_EQ(73365, d->enpassants); EXPECT_EQ(4993637, d->castles); EXPECT_EQ(8392 , d->promotions); EXPECT_EQ(3309887, d->checks); }
  if (search.divide_total) for (auto &d : *search.divide_total) INFO(GetLongMoveName(d.first), ": ", d.second.nodes);
}

TEST(Perft, Position3) {
  // Depth Nodes     Captures E.p.   Castles Promotions Checks   Checkmates
  // 1     14        1        0      0       0          2        0
  // 2     191       14       0      0       0          10       0
  // 3     2812      209      2      0       0          267      0
  // 4     43238     3348     123    0       0          1680     17
  // 5     674624    52051    1165   0       0          52950    0
  // 6     11030083  940350   33325  0       7552       452473   2733
  // 7     178633661 14519036 294874 0       140024     12797406 87
  vector<SearchStats::Total> depth_total;
  unordered_map<Chess::Move, SearchStats::Total> divide_total;
  Position position(perft_pos3_fen);
  SearchStats search;
  search.max_depth = 7;
  search.depth_total = &depth_total;
  // search.divide_total = &divide_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(7, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(14,        d->nodes); EXPECT_EQ(1,        d->captures); EXPECT_EQ(0     , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0     , d->promotions); EXPECT_EQ(2       , d->checks); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(191,       d->nodes); EXPECT_EQ(14,       d->captures); EXPECT_EQ(0     , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0     , d->promotions); EXPECT_EQ(10      , d->checks); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(2812,      d->nodes); EXPECT_EQ(209,      d->captures); EXPECT_EQ(2     , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0     , d->promotions); EXPECT_EQ(267     , d->checks); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(43238,     d->nodes); EXPECT_EQ(3348,     d->captures); EXPECT_EQ(123   , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0     , d->promotions); EXPECT_EQ(1680    , d->checks); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(674624,    d->nodes); EXPECT_EQ(52051,    d->captures); EXPECT_EQ(1165  , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(0     , d->promotions); EXPECT_EQ(52950   , d->checks); }
  if (auto d = VectorGet(depth_total, 5)) { EXPECT_EQ(11030083,  d->nodes); EXPECT_EQ(940350,   d->captures); EXPECT_EQ(33325 , d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(7552  , d->promotions); EXPECT_EQ(452473  , d->checks); }
  if (auto d = VectorGet(depth_total, 6)) { EXPECT_EQ(178633661, d->nodes); EXPECT_EQ(14519036, d->captures); EXPECT_EQ(294874, d->enpassants); EXPECT_EQ(0, d->castles); EXPECT_EQ(140024, d->promotions); EXPECT_EQ(12797406, d->checks); }
  if (search.divide_total) for (auto &d : *search.divide_total) INFO(GetLongMoveName(d.first), ": ", d.second.nodes);
}

TEST(Perft, Position4) {
  // Depth Nodes     Captures  E.p. Castles  Promotions Checks   Checkmates
  // 1     6         0         0    0        0          0        0
  // 2     264       87        0    6        48         10       0
  // 3     9467      1021      4    0        120        38       22
  // 4     422333    131393    0    7795     60032      15492    5
  // 5     15833292  2046173   6512 0        329464     200568   50562
  // 6     706045033 210369132 212  10882006 81102984   26973664 81076
  vector<SearchStats::Total> depth_total;
  Position position(perft_pos4_fen);
  SearchStats search;
  search.max_depth = 6;
  search.depth_total = &depth_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(6, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(6,         d->nodes); EXPECT_EQ(0,         d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(0       , d->promotions); EXPECT_EQ(0       , d->checks); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(264,       d->nodes); EXPECT_EQ(87,        d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(6       , d->castles); EXPECT_EQ(48      , d->promotions); EXPECT_EQ(10      , d->checks); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(9467,      d->nodes); EXPECT_EQ(1021,      d->captures); EXPECT_EQ(4   , d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(120     , d->promotions); EXPECT_EQ(38      , d->checks); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(422333,    d->nodes); EXPECT_EQ(131393 ,   d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(7795    , d->castles); EXPECT_EQ(60032   , d->promotions); EXPECT_EQ(15492   , d->checks); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(15833292,  d->nodes); EXPECT_EQ(2046173,   d->captures); EXPECT_EQ(6512, d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(329464  , d->promotions); EXPECT_EQ(200568  , d->checks); }
  if (auto d = VectorGet(depth_total, 5)) { EXPECT_EQ(706045033, d->nodes); EXPECT_EQ(210369132, d->captures); EXPECT_EQ(212 , d->enpassants); EXPECT_EQ(10882006, d->castles); EXPECT_EQ(81102984, d->promotions); EXPECT_EQ(26973664, d->checks); }
}

TEST(Perft, Position4Mirror) {
  vector<SearchStats::Total> depth_total;
  Position position(perft_pos4_mirror_fen);
  SearchStats search;
  search.max_depth = 6;
  search.depth_total = &depth_total;
  FullSearch(position, BLACK, &search);
  EXPECT_EQ(6, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(6,         d->nodes); EXPECT_EQ(0,         d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(0       , d->promotions); EXPECT_EQ(0       , d->checks); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(264,       d->nodes); EXPECT_EQ(87,        d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(6       , d->castles); EXPECT_EQ(48      , d->promotions); EXPECT_EQ(10      , d->checks); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(9467,      d->nodes); EXPECT_EQ(1021,      d->captures); EXPECT_EQ(4   , d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(120     , d->promotions); EXPECT_EQ(38      , d->checks); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(422333,    d->nodes); EXPECT_EQ(131393 ,   d->captures); EXPECT_EQ(0   , d->enpassants); EXPECT_EQ(7795    , d->castles); EXPECT_EQ(60032   , d->promotions); EXPECT_EQ(15492   , d->checks); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(15833292,  d->nodes); EXPECT_EQ(2046173,   d->captures); EXPECT_EQ(6512, d->enpassants); EXPECT_EQ(0       , d->castles); EXPECT_EQ(329464  , d->promotions); EXPECT_EQ(200568  , d->checks); }
  if (auto d = VectorGet(depth_total, 5)) { EXPECT_EQ(706045033, d->nodes); EXPECT_EQ(210369132, d->captures); EXPECT_EQ(212 , d->enpassants); EXPECT_EQ(10882006, d->castles); EXPECT_EQ(81102984, d->promotions); EXPECT_EQ(26973664, d->checks); }
}

TEST(Perft, Position5) {
  // Depth Nodes
  // 1     44
  // 2     1486
  // 3     62379
  // 4     2103487
  // 5     89941194
  vector<SearchStats::Total> depth_total;
  Position position(perft_pos5_fen);
  SearchStats search;
  search.max_depth = 5;
  search.depth_total = &depth_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(5, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(44,       d->nodes); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(1486,     d->nodes); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(62379,    d->nodes); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(2103487,  d->nodes); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(89941194, d->nodes); }
}

TEST(Perft, Position6) {
  // Depth Nodes
  // 1     46
  // 2     2079
  // 3     89890
  // 4     3894594
  // 5     164075551
  // 6     6923051137
  // 7     287188994746
  // 8     11923589843526
  // 9     490154852788714
  vector<SearchStats::Total> depth_total;
  Position position(perft_pos6_fen);
  SearchStats search;
  search.max_depth = 5;
  search.depth_total = &depth_total;
  FullSearch(position, WHITE, &search);
  EXPECT_EQ(5, depth_total.size());
  if (auto d = VectorGet(depth_total, 0)) { EXPECT_EQ(46,        d->nodes); }
  if (auto d = VectorGet(depth_total, 1)) { EXPECT_EQ(2079,      d->nodes); }
  if (auto d = VectorGet(depth_total, 2)) { EXPECT_EQ(89890,     d->nodes); }
  if (auto d = VectorGet(depth_total, 3)) { EXPECT_EQ(3894594,   d->nodes); }
  if (auto d = VectorGet(depth_total, 4)) { EXPECT_EQ(164075551, d->nodes); }
}

TEST(Perft, PerftSuite) {
  unique_ptr<File> testfile(app->OpenFile("perftsuite.epd"));
  if (!testfile || !testfile->Opened()) { EXPECT_TRUE(false); return; }
  FileLineIter tests(testfile.get());
  vector<SearchStats::Total> depth_total;
  Chess::Position position;
  vector<string> arg;
  int count=0;
  for (const char *line = tests.Next(); line; line = tests.Next()) {
    Split(StringPiece(line, tests.CurrentLength()), isint<';'>, nullptr, &arg);
    if (arg.size() < 2) { EXPECT_TRUE(false); continue; }
    if (!position.LoadFEN(arg[0])) { EXPECT_TRUE(false); continue; }
    vector<int> depth;
    for (int i=1, l=arg.size(); i != l; ++i) {
      EXPECT_TRUE(PrefixMatch(arg[i], StrCat("D", i, " ")));
      depth.push_back(LFL::atoi(arg[i].data() + 3));
    }
    INFO("PerftSuite[", ++count, "]: depth=", depth.size(), " ", position.GetFEN());
    SearchStats search;
    search.max_depth = depth.size();
    (search.depth_total = &depth_total)->clear();
    FullSearch(position, position.flags.to_move_color, &search);
    if (depth.size() != depth_total.size()) { EXPECT_TRUE(false); continue; }
    for (int i=0, l=depth.size(); i != l; ++i) { EXPECT_EQ(depth[i], depth_total[i].nodes); }
  }
}
#endif // CHESS_PERFT_TESTS

TEST(EvaluationTest, Static) {
  Position position;
  EXPECT_FLOAT_EQ(0, StaticEvaluation(position)); 

  EXPECT_EQ(true, position.LoadFEN("4k3/3PP3/4K3/8/8/8/8/8 b - - 0 40"));
  EXPECT_GT(StaticEvaluation(position), 100);

  EXPECT_EQ(true, position.LoadFEN("4k3/4P3/3PK3/8/8/8/8/8 b - - 0 40"));
  EXPECT_FLOAT_EQ(0, StaticEvaluation(position));

  EXPECT_EQ(true, position.LoadFEN("8/8/8/8/8/4k3/3pp3/4K3 w - - 0 40"));
  EXPECT_LT(StaticEvaluation(position), -100);

  EXPECT_EQ(true, position.LoadFEN("8/8/8/8/8/3pk3/4p3/4K3 w - - 0 40"));
  EXPECT_FLOAT_EQ(0, StaticEvaluation(position));
}

TEST(EvaluationTest, Search) {
  Position position;

  // white to play, mate in 1
  EXPECT_EQ(true, position.LoadFEN("4k3/4P3/3PK3/8/8/8/8/8 w - - 0 40"));
  auto move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 1);
  EXPECT_EQ("d6d7", GetLongMoveName(move.first));
  EXPECT_GT(move.second, 100);

  // black to play, mate in 1
  EXPECT_EQ(true, position.LoadFEN("8/8/8/8/8/3pk3/4p3/4K3 b - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 1);
  EXPECT_EQ("d3d2", GetLongMoveName(move.first));
  EXPECT_GT(move.second, 100);

  // white to play, mate in 2
  EXPECT_EQ(true, position.LoadFEN("r1bq1r1k/1pppNppp/p7/4R2Q/n7/8/PPPP1PPP/R1B3K1 w - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 3);
  EXPECT_GT(move.second, 100);
  EXPECT_EQ("h5h7", GetLongMoveName(move.first));

  // black to play, mate in 2
  EXPECT_EQ(true, position.LoadFEN("Q7/ppp2k1p/3p2p1/5b2/4P1nq/2P4P/PP1P1bP1/RNB2R1K b - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 3);
  EXPECT_GT(move.second, 100);
  EXPECT_EQ("h4h3", GetLongMoveName(move.first));

  // white to play, mate in 3
  EXPECT_EQ(true, position.LoadFEN("r4r2/1q3pkp/p1b1p1n1/1p4QP/4P3/1BP3P1/P4P2/R2R2K1 w - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 5);
  EXPECT_GT(move.second, 100);
  EXPECT_EQ("h5h6", GetLongMoveName(move.first));

  // black to play, mate in 3
  EXPECT_EQ(true, position.LoadFEN("r7/2p2pk1/p5p1/1p1Q2Kp/1P6/2P1N2P/1P2n1P1/R5b1 b - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 5);
  EXPECT_GT(move.second, 100);
  EXPECT_EQ("f7f6", GetLongMoveName(move.first));

  // white to play, mate in 4
  EXPECT_EQ(true, position.LoadFEN("rnb3kr/ppp2ppp/1b6/3q4/3pN3/Q4N2/PPP2KPP/R1B1R3 w - - 0 40"));
  move = AlphaBetaNegamaxSearch(position, position.flags.to_move_color, -INFINITY, INFINITY, 7);
  EXPECT_GT(move.second, 100);
  EXPECT_EQ("e4f6", GetLongMoveName(move.first));
}
