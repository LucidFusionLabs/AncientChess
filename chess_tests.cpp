#include "gtest/gtest.h"
#include "core/app/app.h"
#include "chess.h"

using namespace LFL;
using namespace LFL::Chess;

extern "C" void MyAppCreate() {
  app = new Application();
  screen = new Window();
}

extern "C" int MyAppMain(int argc, const char* const* argv) {
  if (!app) MyAppCreate();
  testing::InitGoogleTest(&argc, const_cast<char**>(argv));
  LFL::FLAGS_default_font = LFL::FakeFontEngine::Filename();
  CHECK_EQ(0, LFL::app->Create(argc, argv, __FILE__));
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
}

TEST(Boards, ByteBoard) {
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

  EXPECT_EQ(Chess::QueenMoves(Chess::Position("--------\n"
                                              "-kp-----\n"
                                              "-pp-n---\n"
                                              "p---qp--\n"
                                              "--------\n"
                                              "--PP--P-\n"
                                              "PPK-NQ--\n"
                                              "--------\n"), Chess::e5, Chess::BLACK),
            BitBoardFromString("00000001\n"
                               "00000010\n"
                               "00010100\n"
                               "01110000\n"
                               "00011100\n"
                               "00101010\n"
                               "00001000\n"
                               "00000000\n"));

  unsigned long long n = 37;
  while (n) {
    unsigned long long i = n & -n;
    // INFO("hrm i ", i);
    n ^= i;
  }
}
