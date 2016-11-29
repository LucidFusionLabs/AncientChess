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

typedef uint8_t Piece;
typedef uint32_t Move;
typedef uint64_t BitBoard;
typedef string ByteBoard;

enum { WHITE=0, BLACK=1 };
enum { ALL=0, PAWN=1, KNIGHT=2, BISHOP=3, ROOK=4, QUEEN=5, KING=6, END_PIECES=7 };
enum { h1=0,  g1=1,  f1=2,  e1=3,  d1=4,  c1=5,  b1=6,  a1=7,
       h2=8,  g2=9,  f2=10, e2=11, d2=12, c2=13, b2=14, a2=15,
       h3=16, g3=17, f3=18, e3=19, d3=20, c3=21, b3=22, a3=23,
       h4=24, g4=25, f4=26, e4=27, d4=28, c4=29, b4=30, a4=31,
       h5=32, g5=33, f5=34, e5=35, d5=36, c5=37, b5=38, a5=39,
       h6=40, g6=41, f6=42, e6=43, d6=44, c6=45, b6=46, a6=47,
       h7=48, g7=49, f7=50, e7=51, d7=52, c7=53, b7=54, a7=55,
       h8=56, g8=57, f8=58, e8=59, d8=60, c8=61, b8=62, a8=63 };
struct MoveFlag { enum { Castle=1, CastleLong=2, Check=4, Checkmate=8, DoubleStepPawn=16, EnPassant=32 }; };

static int no_special_moves[] = { 0 };
static int king_special_moves [] = { MoveFlag::Castle, MoveFlag::CastleLong, 0 };
static int pawn_special_moves [] = { MoveFlag::DoubleStepPawn, MoveFlag::EnPassant, 0 };
static int *special_moves[] = { nullptr, pawn_special_moves, no_special_moves, no_special_moves, no_special_moves, no_special_moves, king_special_moves };

//                                      { 0, pawns,         knights,     bishops,     rooks,       queen,       king       };
static const BitBoard white_initial[] = { 0, 0xff00ULL,     0x42ULL,     0x24ULL,     0x81ULL,     0x10ULL,     0x8ULL     };
static const BitBoard black_initial[] = { 0, 0xff00ULL<<40, 0x42ULL<<56, 0x24ULL<<56, 0x81ULL<<56, 0x10ULL<<56, 0x8ULL<<56 };
static const BitBoard black_castle_path = 0x600000000000000LL, black_castle_long_clear = 0x7000000000000000LL, black_castle_long_path = 0x3000000000000000LL;
static const BitBoard white_castle_path = 0x6LL,               white_castle_long_clear = 0x70LL,               white_castle_long_path = 0x30LL;

static const char initial_byte_board[] = 
"rnbqkbnr\n"
"pppppppp\n"
"--------\n"
"--------\n"
"--------\n"
"--------\n"
"PPPPPPPP\n"
"RNBQKBNR\n";

static const char kiwipete_byte_board[] =
"r---k--r\n"
"p-ppqpb-\n"
"bn--pnp-\n"
"---PN---\n"
"-p--P---\n"
"--N--Q-p\n"
"PPPBBPPP\n"
"R---K--R\n";

static const char perft_pos3_fen[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
static const char perft_pos4_fen[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
static const char perft_pos4_mirror_fen[] = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1";
static const char perft_pos5_fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
static const char perft_pos6_fen[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

static const char *ColorName(int n) {
  static const char *name[] = { "white", "black" };
  CHECK_RANGE(n, 0, 2);
  return name[n];
}

static char PieceChar(int n) {
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

static int8_t PieceCharType(char c, bool *color=0) {
  switch(c) {
    case 'P': if (color) *color = WHITE; return PAWN;
    case 'p': if (color) *color = BLACK; return PAWN;
    case 'N': if (color) *color = WHITE; return KNIGHT;
    case 'n': if (color) *color = BLACK; return KNIGHT;
    case 'B': if (color) *color = WHITE; return BISHOP;
    case 'b': if (color) *color = BLACK; return BISHOP;
    case 'R': if (color) *color = WHITE; return ROOK;
    case 'r': if (color) *color = BLACK; return ROOK;
    case 'Q': if (color) *color = WHITE; return QUEEN;
    case 'q': if (color) *color = BLACK; return QUEEN;
    case 'K': if (color) *color = WHITE; return KING;
    case 'k': if (color) *color = BLACK; return KING;
    default:  if (color) *color = 0;     return 0;
  }
}

inline uint8_t GetPieceType(Piece piece) { return (piece & 7); }
inline bool GetPieceColor(Piece piece) { return piece & (1<<3); }
inline Piece GetPiece(bool color, uint8_t piece) {
  return (uint8_t(color) << 3) | (piece & 7);
}

inline uint8_t GetMovePieceType (Move move) { return (move >> 23) & 0x7; }
inline uint8_t GetMoveFromSquare(Move move) { return (move >> 17) & 0x3f; }
inline uint8_t GetMoveToSquare  (Move move) { return (move >> 11) & 0x3f; }
inline uint8_t GetMoveCapture   (Move move) { return (move >> 26) & 0x7; }
inline uint8_t GetMovePromotion (Move move) { return (move >> 29) & 0x7; }

inline Move GetMove(uint8_t piece, uint8_t start_square, uint8_t end_square, uint8_t capture, uint8_t promote, uint8_t flags) {
  return ((promote & 7) << 29) | ((capture & 7) << 26) | ((piece & 7) << 23) | ((start_square & 0x3f) << 17) |
    ((end_square & 0x3f) << 11) | flags;
}

inline int8_t SquareX(int s) { return 7 - (s % 8); }
inline int8_t SquareY(int s) { return s / 8; }
inline int8_t SquareFromXY(int x, int y) { return (x<0 || y<0 || x>7 || y>7) ? -1 : (y*8 + (7-x)); }
inline BitBoard SquareMask(int s) { return 1LL << s; }
inline int8_t SquareID(const char *s) {
  if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8') return ERRORv(-1, "unknown square: ", s);
  return 8 * (s[1] - '1') + 7 - (s[0] - 'a');
}

struct SquareIter { 
  BitBoard data;
  SquareIter(BitBoard v) : data(v) {}
  uint8_t GetSquare() const { return ffsll(data) - 1; }
  operator bool() const { return data; }
  SquareIter& operator++() { data &= (data-1); return *this; }
};

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

string EmptyByteBoard() {
  string ret(8*9, '-');
  for (int i=0; i<8; i++) ret[8 + i*9] = '\n';
  return ret;
}

string EmptyBitBoardString() {
  string ret(8*9, '0');
  for (int i=0; i<8; i++) ret[8 + i*9] = '\n';
  return ret;
}

string SquareMaskString(int s) {
  string ret = EmptyBitBoardString();
  ret[(7-SquareY(s))*9 + SquareX(s)] = '1';
  return ret;
}

string BitBoardToString(BitBoard b) {
  string ret = EmptyBitBoardString();
  for (SquareIter si(b); si; ++si) {
    uint8_t s = si.GetSquare();
    ret[(7-SquareY(s))*9 + SquareX(s)] = '1';
  }
  return ret;
}

BitBoard BitBoardFromString(const char *buf, char v='1') {
  BitBoard ret = 0;
  for (int i=7, bi=0; i>=0; i--, bi++) {
    for (int j=7; j>=0; j--, bi++) if (buf[bi] == v) ret |= (1ULL<<(i*8+j));
  }
  return ret;
}

BitBoard ByteBoardToBitBoard(const ByteBoard &buf, char piece) {
  BitBoard ret = 0;
  for (int i=7, bi=0; i>=0; i--, bi++) {
    for (int j=7; j>=0; j--, bi++) if (buf[bi] == piece) ret |= (1ULL<<(i*8+j));
  }
  return ret;
}

}; // namespace Chess
}; // namespace LFL
#include "magic.h"
namespace LFL {
namespace Chess {
  
struct PieceCount { 
  uint8_t pawn_count:4, knight_count:4, bishop_count:4, rook_count:4, queen_count:4, king_count:4;
  PieceCount() { Clear(); }
  void Clear() { pawn_count=knight_count=bishop_count=rook_count=queen_count=king_count=0; }
  void Add(uint8_t piece) {
    switch(piece) {
      case PAWN:   ++pawn_count;   break;
      case KNIGHT: ++knight_count; break;
      case BISHOP: ++bishop_count; break;
      case ROOK:   ++rook_count;   break;
      case QUEEN:  ++queen_count;  break;
      case KING:   ++king_count;   break;
      default:     FATAL("unknown piece ", int(piece));
    }
  }            
};

struct PositionFlags {
  uint8_t fifty_move_rule_count:6, to_move_color:1, w_cant_castle:1, b_cant_castle:1,
          w_cant_castle_long:1, b_cant_castle_long:1;
};

struct Position {
  Move move=0;
  PositionFlags flags;
  uint16_t move_number=0;
  BitBoard white[7], black[7];

  Position() { Reset(); }
  Position(const string &b) { if (!LoadFEN(b)) Reset(); }
  void Assign(const Position &p) { *this = p; }
  static Position FromByteBoard(const string &b) { Position p; p.LoadByteBoard(b); return p; }

  void Reset() {
    move = 0;
    move_number = 0;
    memzero(flags);
    for (int i=PAWN; i!=END_PIECES; ++i) white[i] = white_initial[i];
    for (int i=PAWN; i!=END_PIECES; ++i) black[i] = black_initial[i];
    SetAll(WHITE);
    SetAll(BLACK);
  }

  void LoadByteBoard(const string &b) {
    for (int i=PAWN; i!=END_PIECES; ++i) white[i] = ByteBoardToBitBoard(b, ByteBoardPieceSymbol(i, WHITE));
    for (int i=PAWN; i!=END_PIECES; ++i) black[i] = ByteBoardToBitBoard(b, ByteBoardPieceSymbol(i, BLACK));
    SetAll(WHITE);
    SetAll(BLACK);
  }

  int NextMoveNumber() const { return move_number ? ((move_number + 2) / 2) : 1; }
  int StandardMoveNumber() const { return move_number ? ((move_number + 1) / 2) : 1; }
  const char *StandardMoveSuffix() const { return flags.to_move_color ? "" : "..."; }

  string GetByteBoard() const {
    string ret = EmptyByteBoard();
    for (int8_t color=0; color<2; color++)
      for (int8_t piece_type = PAWN; piece_type != END_PIECES; ++piece_type)
        for (SquareIter p(Pieces(color)[piece_type]); p; ++p) {
          int8_t s = p.GetSquare();
          ret[(7-SquareY(s))*9 + SquareX(s)] =
            color ? PieceChar(piece_type) : std::toupper(PieceChar(piece_type));
        }
    return ret;
  }

  string GetFEN() const {
    char null_count = 0;
    string ret, byteboard = GetByteBoard(), castle, enpassant;
    for (auto &c : byteboard) {
      if      (c == '-') null_count++;
      else if (c == '\n')  { if (null_count) ret.push_back('0'+null_count); null_count=0; ret.push_back('/'); }
      else if (isalpha(c)) { if (null_count) ret.push_back('0'+null_count); null_count=0; ret.push_back(c); }
      else return string();
    }
    if (!flags.w_cant_castle)      castle.push_back('K');
    if (!flags.w_cant_castle_long) castle.push_back('Q');
    if (!flags.b_cant_castle)      castle.push_back('k');
    if (!flags.b_cant_castle_long) castle.push_back('q');
    if (!ret.size() || ret.back() != '/') return string();
    ret.pop_back();
    return StrCat(ret, flags.to_move_color ? " b " : " w ", castle.size()?castle:"-", " ",
                  enpassant.size()?enpassant:"-", " ", int(flags.fifty_move_rule_count), " ",
                  NextMoveNumber());
  }

  bool LoadFEN(const string &b) {
    memzero(white);
    memzero(black);
    bool piece_color;
    int8_t piece_type, x=7, y=7;
    auto p = b.begin(), e = b.end();
    for (; p != e; ++p) {
      if (y < 0 || x < -1) return false;
      if      (*p == ' ')   break;
      else if (*p == '/')   { y--; x=7; }
      else if (isdigit(*p)) x -= (*p - '0');
      else if (x < 0)       return false;
      else if (!(piece_type = PieceCharType(*p, &piece_color))) ERROR("unknown piece '", *p, "'");
      else if (piece_color) black[piece_type] |= (1ULL << (y*8 + x--));
      else                  white[piece_type] |= (1ULL << (y*8 + x--));
    }
    if (y != 0 || x != -1 || p == e) return false;
    vector<string> args;
    int ind = p - b.begin();
    Split(StringPiece(b.data() + ind, b.size() - ind), isspace, nullptr, &args);
    flags.to_move_color = args.size() > 0 && args[0] == "b";
    flags.w_cant_castle_long = !(args.size() > 1 && strchr(args[1].data(), 'Q'));
    flags.b_cant_castle_long = !(args.size() > 1 && strchr(args[1].data(), 'q'));
    flags.w_cant_castle      = !(args.size() > 1 && strchr(args[1].data(), 'K'));
    flags.b_cant_castle      = !(args.size() > 1 && strchr(args[1].data(), 'k'));
    flags.fifty_move_rule_count = args.size() > 3 ? atoi(args[3]) : 0;
    move_number = args.size() > 4 ? (atoi(args[4])*2 - !flags.to_move_color - 1) : 0;
    move = 0;
    SetAll(WHITE);
    SetAll(BLACK);
    return true;
  }

  void UpdateMove(bool new_move, int8_t piece_type, int8_t square_from, int8_t square_to, int8_t captured,
                  uint8_t promotion, uint8_t move_flags) {
    bool piece_color = (move_number % 2) == 0;
    if (piece_type == PAWN && abs(SquareY(square_to) - SquareY(square_from)) == 2)
      move_flags |= MoveFlag::DoubleStepPawn;
    CHECK_EQ(GetPiece(piece_color, promotion ? promotion : piece_type), GetSquare(square_to));
    move = Chess::GetMove(piece_type, square_from, square_to, captured, promotion, move_flags);
    if (!new_move) return;

    if (piece_color == WHITE) {
      if      (square_from == e1) flags.w_cant_castle = flags.w_cant_castle_long = true;
      else if (square_from == h1) flags.w_cant_castle = true;
      else if (square_from == a1) flags.w_cant_castle_long = true;
      if      (square_to   == a8) flags.b_cant_castle_long = true;
      else if (square_to   == h8) flags.b_cant_castle = true;
    } else {
      if      (square_from == e8) flags.b_cant_castle = flags.b_cant_castle_long = true;
      else if (square_from == h8) flags.b_cant_castle = true;
      else if (square_from == a8) flags.b_cant_castle_long = true;
      if      (square_to   == a1) flags.w_cant_castle_long = true;
      else if (square_to   == h1) flags.w_cant_castle = true;
    }
    if (captured || piece_type == PAWN) flags.fifty_move_rule_count = 0;
    else                                flags.fifty_move_rule_count++;
  }

  void UpdateCastles(bool color, int8_t square_to) {
    uint8_t rook_from, rook_to; 
    switch(square_to) {
      case g1: rook_from = h1; rook_to = f1; break;
      case g8: rook_from = h8; rook_to = f8; break;
      case c1: rook_from = a1; rook_to = d1; break;
      case c8: rook_from = a8; rook_to = d8; break;
      default: FATAL("invalid castle");      break;
    }
    uint8_t rook = ClearSquare(rook_from, color == WHITE, color == BLACK);
    CHECK_EQ(int(GetPiece(color, ROOK)), int(rook));
    CHECK_EQ(int(GetPiece(WHITE, 0)), int(GetSquare(rook_to)));
    SetSquare(rook_to, rook);
  }

  BitBoard AllPieces() const { return white[ALL] | black[ALL]; }
  const BitBoard *Pieces(bool color) const { return color ? black : white; }
  /**/  BitBoard *Pieces(bool color)       { return color ? black : white; }

  void SetAll(bool color) {
    BitBoard *pieces = Pieces(color);
    pieces[ALL] = pieces[PAWN] | pieces[KNIGHT] | pieces[BISHOP] | pieces[ROOK] | pieces[QUEEN] | pieces[KING];
  }

  void SetSquare(int s, Piece piece) {
    BitBoard mask = SquareMask(s);
    uint8_t piece_color = GetPieceColor(piece), piece_type = GetPieceType(piece);
    CHECK_RANGE(piece_type, PAWN, END_PIECES);
    if (piece_color) { black[piece_type] |= mask; black[0] |= mask; }
    else             { white[piece_type] |= mask; white[0] |= mask; }
  }

  Piece GetSquare(int s) const {
    BitBoard mask = SquareMask(s);
    for (int i=1; i<7; i++) if (white[i] & mask) return GetPiece(WHITE, i);
    for (int i=1; i<7; i++) if (black[i] & mask) return GetPiece(BLACK, i);
    return GetPiece(WHITE, 0);
  }

  Piece ClearSquare(int s, bool clear_white=true, bool clear_black=true) {
    BitBoard mask = SquareMask(s);
    if (clear_white) for (int i=PAWN; i!=END_PIECES; i++) if (white[i] & mask) { white[0] &= ~mask; white[i] &= ~mask; return GetPiece(WHITE, i); }
    if (clear_black) for (int i=PAWN; i!=END_PIECES; i++) if (black[i] & mask) { black[0] &= ~mask; black[i] &= ~mask; return GetPiece(BLACK, i); }
    return GetPiece(WHITE, 0);
  }
  
  BitBoard PawnEnPassant(int p, bool black) const {
    uint8_t square_to;
    if (!(move & MoveFlag::DoubleStepPawn) ||
        SquareY((square_to = GetMoveToSquare(move))) != SquareY(p)) return 0;
    if      (square_to + 1 == p) return SquareMask(black ? p-9 : p+7);
    else if (square_to - 1 == p) return SquareMask(black ? p-7 : p+9);
    else                         return 0;
  }

  BitBoard PawnAttacks(int p, bool black) const {
    return black ? black_pawn_attack_mask[p] : white_pawn_attack_mask[p];
  }

  BitBoard PawnCaptures(int p, bool black) const {
    return black ? (black_pawn_attack_mask[p] & Pieces(!black)[ALL])
                 : (white_pawn_attack_mask[p] & Pieces(!black)[ALL]);
  }

  BitBoard SingleStepPawnAdvances(int p, bool black) const {
    return (black ? (SquareMask(p) >> 8) : (SquareMask(p) << 8)) & ~AllPieces();
  }

  BitBoard DoubleStepPawnAdvances(int p, bool black) const {
    if (black) { if (SquareY(p) == 6) if ((black_pawn_occupancy_mask[p] & AllPieces()) == 0) return SquareMask(p-16); }
    else       { if (SquareY(p) == 1) if ((white_pawn_occupancy_mask[p] & AllPieces()) == 0) return SquareMask(p+16); }
    return 0;
  }

  BitBoard PawnAdvances(int p, bool black) const {
    return SingleStepPawnAdvances(p, black) | DoubleStepPawnAdvances(p, black);
  }

  BitBoard PawnMoves(int p, bool black) const {
    return PawnAdvances(p, black) | PawnCaptures(p, black) | PawnEnPassant(p, black);
  }

  BitBoard KnightMoves(int p, bool black) const {
    return knight_occupancy_mask[p] & ~Pieces(black)[ALL];
  }

  BitBoard BishopMoves(int p, bool black) const {
    static MagicMoves *magic_moves = Singleton<MagicMoves>::Get();
    BitBoard blockers = AllPieces() & bishop_occupancy_mask[p];
    return magic_moves->BishopMoves(p, blockers, Pieces(black)[ALL]);
  }

  BitBoard RookMoves(int p, bool black) const {
    static MagicMoves *magic_moves = Singleton<MagicMoves>::Get();
    BitBoard blockers = AllPieces() & rook_occupancy_mask[p];
    int magic_index = MagicMoves::MagicHash(p, blockers, rook_magic_number, rook_magic_number_bits);
    return magic_moves->rook_magic_moves[p][magic_index] & ~Pieces(black)[ALL];
  }

  BitBoard QueenMoves(int p, bool black) const {
    return RookMoves(p, black) | BishopMoves(p, black);
  }

  BitBoard KingCastles(int p, bool black, BitBoard attacked) const {
    if (SquareMask(p) & attacked) return 0;
    if (black) { if (!flags.b_cant_castle && !(AllPieces() & black_castle_path) && !(attacked & black_castle_path)) return SquareMask(g8); }
    else       { if (!flags.w_cant_castle && !(AllPieces() & white_castle_path) && !(attacked & white_castle_path)) return SquareMask(g1); }
    return 0;
  }

  BitBoard KingLongCastles(int p, bool black, BitBoard attacked) const {
    if (SquareMask(p) & attacked) return 0;
    if (black) { if (!flags.b_cant_castle_long && !(AllPieces() & black_castle_long_clear) && !(attacked & black_castle_long_path)) return SquareMask(c8); }
    else       { if (!flags.w_cant_castle_long && !(AllPieces() & white_castle_long_clear) && !(attacked & white_castle_long_path)) return SquareMask(c1); }
    return 0;
  }

  BitBoard NormalKingMoves(int p, bool black, BitBoard attacked) const {
    return king_occupancy_mask[p] & ~Pieces(black)[ALL] & ~attacked;
  }

  BitBoard KingMoves(int p, bool black, BitBoard attacked) const {
    return (king_occupancy_mask[p] & ~Pieces(black)[ALL] & ~attacked)
      | KingCastles(p, black, attacked) | KingLongCastles(p, black, attacked);
  }

  BitBoard PieceMoves(int piece, int square, bool black, BitBoard attacked=0) const {
    if      (piece == PAWN)   return PawnMoves  (square, black);
    else if (piece == KNIGHT) return KnightMoves(square, black);
    else if (piece == BISHOP) return BishopMoves(square, black);
    else if (piece == ROOK)   return RookMoves  (square, black);
    else if (piece == QUEEN)  return QueenMoves (square, black);
    else if (piece == KING)   return KingMoves  (square, black, attacked);
    else                      FATAL("unknown piece ", piece);
  }

  BitBoard PieceMovesOfType(int piece, int square, bool black, int type, BitBoard attacked=0) const {
    if (piece == PAWN) {
      if      (type == MoveFlag::EnPassant)      return PawnEnPassant(square, black);
      else if (type == MoveFlag::DoubleStepPawn) return DoubleStepPawnAdvances(square, black);
      else return SingleStepPawnAdvances(square, black) | PawnCaptures(square, black);
    } else if (piece == KING) {
      if      (type == MoveFlag::Castle)     return KingCastles(square, black, attacked);
      else if (type == MoveFlag::CastleLong) return KingLongCastles(square, black, attacked);
      else                                   return NormalKingMoves(square, black, attacked);
    } else if (piece == KNIGHT) return KnightMoves(square, black);
    else if (piece == BISHOP) return BishopMoves(square, black);
    else if (piece == ROOK)   return RookMoves  (square, black);
    else if (piece == QUEEN)  return QueenMoves (square, black);
    else                      FATAL("unknown piece ", piece);
  }

  BitBoard PieceAttacks(int piece, int square, bool black, BitBoard attacked=0) const {
    if      (piece == PAWN)   return PawnAttacks(square, black) | PawnEnPassant(square, black);
    else if (piece == KNIGHT) return KnightMoves(square, black);
    else if (piece == BISHOP) return BishopMoves(square, black);
    else if (piece == ROOK)   return RookMoves  (square, black);
    else if (piece == QUEEN)  return QueenMoves (square, black);
    else if (piece == KING)   return KingMoves  (square, black, attacked);
    else                      FATAL("unknown piece ", piece);
  }

  BitBoard AllAttacks(bool color, BitBoard attacked=0) const {
    BitBoard ret = 0;
    for (int piece_type = PAWN; piece_type != END_PIECES; ++piece_type)
      for (SquareIter p(Pieces(color)[piece_type]); p; ++p)
        ret |= PieceAttacks(piece_type, p.GetSquare(), color);
    return ret;
  }

  bool InCheck(BitBoard attacks, bool color) const {
    return attacks & Pieces(color)[KING];
  }
};

struct SearchState {
  struct Total {
    int nodes=0, captures=0, enpassants=0, castles=0, promotions=0, checks=0, checkmates=0;
    void CountMove(bool capture) { nodes++; if (capture) captures++; }
  };
  Total total;
  vector<Total> depth;
  unordered_map<Move, Total> divide;
  int max_depth=0;

  void CountMove(int di, bool capture, Total *divide=0) {
    total.CountMove(capture);
    if (divide && di+1 == max_depth) divide->CountMove(capture);
    if (di >= depth.size()) depth.resize(di+1);
    depth[di].CountMove(capture);
  }
};

vector<Position> GenerateMoves(const Position &in, bool color, PieceCount *piece_count=0) {
  vector<Position> ret;
  uint8_t square_from, square_to;
  BitBoard attacked = in.AllAttacks(!color);
  for (int piece_type = PAWN; piece_type != END_PIECES; ++piece_type)
    for (SquareIter p(in.Pieces(color)[piece_type]); p; ++p) {
      square_from = p.GetSquare();
      if (piece_count) piece_count->Add(piece_type);
      for (auto *move_type = special_moves[piece_type]; /**/; move_type++) {
        for (SquareIter m(in.PieceMovesOfType(piece_type, square_from, color, *move_type, attacked)); m; ++m) {
          ret.emplace_back(in);
          square_to = m.GetSquare();
          Position &position = ret.back();
          uint8_t capture_square = ((*move_type) & MoveFlag::EnPassant) ? (square_to + 8 * (color ? 1 : -1)) : square_to;
          Piece piece          = position.ClearSquare(square_from,    color == WHITE, color == BLACK);
          Piece captured_piece = position.ClearSquare(capture_square, color != WHITE, color != BLACK);
          uint8_t captured_piece_type = GetPieceType(captured_piece);
          position.move_number++;
          position.flags.to_move_color = !color;

          if (piece_type == KING && abs(SquareX(square_to) - SquareX(square_from)) > 1) 
            position.UpdateCastles(color, square_to);
          if (piece_type == PAWN && SquareY(square_to) == (color ? 0 : 7)) {
            Position promote_pos = PopBack(ret);
            for (uint8_t promotion = QUEEN; promotion > PAWN; --promotion) {
              Position pos = promote_pos;
              pos.SetSquare(square_to, GetPiece(color, promotion));
              pos.UpdateMove(true, GetPieceType(piece), square_from, square_to, captured_piece_type, promotion, 0);
              if (!pos.InCheck(pos.AllAttacks(!color), color)) ret.push_back(pos);
            }
          } else {
            position.SetSquare(square_to, piece);
            position.UpdateMove(true, GetPieceType(piece), square_from, square_to, captured_piece_type, 0, *move_type);
            if (position.InCheck(position.AllAttacks(!color), color)) ret.pop_back();
          }
        }
        if (!*move_type) break;
      }
    }
  return ret;
}

float StaticEvaluation(Position in) {
  static float king_weight=200, queen_weight=9, rook_weight=5, knight_weight=3, bishop_weight=3,
               pawn_weight=1, mobility_weight=.1;
  PieceCount my_material, opponent_material;
  bool my_color = in.flags.to_move_color;
  auto my_moves = GenerateMoves(in, my_color, &my_material);
  in.move_number++;
  auto opponent_moves = GenerateMoves(in, !my_color, &opponent_material);
  auto &white_moves = my_color ? opponent_moves : my_moves;
  auto &black_moves = my_color ? my_moves : opponent_moves;
  auto &white_material = my_color ? opponent_material : my_material;
  auto &black_material = my_color ? my_material : opponent_material;
  return
    king_weight     * (white_material.king_count   - black_material.king_count)   +
    queen_weight    * (white_material.queen_count  - black_material.queen_count)  +
    rook_weight     * (white_material.rook_count   - black_material.rook_count)   +
    knight_weight   * (white_material.knight_count - black_material.knight_count) +
    bishop_weight   * (white_material.bishop_count - black_material.bishop_count) +
    pawn_weight     * (white_material.pawn_count   - black_material.pawn_count)   +
    mobility_weight * (white_moves.size()          - black_moves.size());
}

void FullSearch(Position in, bool color, SearchState *count, int depth=0, SearchState::Total *divide=0) {
  auto moves = GenerateMoves(in, color);
  for (auto &m : moves) {
    unsigned char move_from = GetMoveFromSquare(m.move), move_to = GetMoveToSquare(m.move);
    if (!depth) divide = &count->divide[GetMove(GetMovePieceType(m.move), move_from, move_to, 0, 0, 0)];
    if (count) count->CountMove(depth, GetMoveCapture(m.move), divide);
    if (depth+1 >= count->max_depth) continue;
    FullSearch(m, !color, count, depth+1, divide);
  }
}

pair<Move, float> AlphaBetaNegamaxSearch(Position in, bool color, float alpha, float beta, int depth) {
  if (!depth) return make_pair(in.move, StaticEvaluation(in) * (color ? -1 : 1));
  float v;
  pair<Move, float> best(0, -INFINITY);
  auto moves = GenerateMoves(in, color);
  for (auto &m : moves) {
    v = -AlphaBetaNegamaxSearch(m, !color, -beta, -alpha, depth-1).second;
    if (Max(&best.second, v)) best.first = m.move;
    if ((alpha = max(alpha, v)) >= beta) break;
  }
  return best;
}

struct GamePosition : public Position {
  string name;
  BitBoard white_attacks[7], black_attacks[7];
  using Position::Position;
};

struct Game {
  string p1_name, p2_name;
  GamePosition position, last_position;
  vector<GamePosition> history;
  deque<GamePosition> premove;
  bool active = 1, my_color = 0, flip_board = 0, engine_playing_white = 0, engine_playing_black = 0;
  int game_number = 0, history_ind = 0, p1_secs = 0, p2_secs = 0, last_p1_secs = 0, last_p2_secs = 0;
  int move_animate_from = -1, move_animate_to = -1;
  Time update_time, move_animation_start;
  Piece moving_piece, animating_piece;

  void Reset() { *this = Game(); }
  void AddNewMove() {
    if (history.empty()) history.push_back(last_position);
    history.push_back(position);
    last_position = position;
  }

  string LongAlgebraicMoveList() const {
    string text;
    if (!history.size()) return text;
    for (auto b = history.begin()+1, e = history.end(), i = b; i != e; ++i)
      StrAppend(&text, SquareName(GetMoveFromSquare(i->move)), SquareName(GetMoveToSquare(i->move)), " ");
    return text;
  }
};

struct Engine {
  Game game;
  StringCB write_cb;
  Engine(StringCB w_cb) : write_cb(move(w_cb)) {}

  void LineCB(const string &text) {
    if      (text == "uci")        write_cb("uciok\n");
    else if (text == "isready")    write_cb("readyok\n");
    else if (text == "ucinewgame") game = Game();
    else if (PrefixMatch(text, "position ")) {
      StringWordIter words(StringPiece::FromRemaining(text, 9));
      string type = words.NextString();
      if (type == "print") write_cb(StrCat(game.position.GetFEN(), "\n"));
      else if (type == "fen" && words.Next()) {
        string fen = text.substr(9 + words.CurrentOffset());
        if (!game.position.LoadFEN(fen)) ERROR("Load FEN '", fen, "'");
      } else if (type == "startpos") {
        game.position.Reset();
      } else ERROR("unknown position type '", type, "'");
    } else if (text == "go" || PrefixMatch(text, "go ")) {
      auto move = AlphaBetaNegamaxSearch(game.position, game.position.flags.to_move_color,
                                         -INFINITY, INFINITY, 3);
      string text = StrCat(SquareName(GetMoveFromSquare(move.first)),
                           SquareName(GetMoveToSquare(move.first)));
      INFO("bestmove ", text, " ", move.second);
      write_cb(StrCat("bestmove ", text, "\n"));
    }
  }
};

}; // namespace Chess

struct UniversalChessInterfaceEngine {
  ProcessPipe process;
  Window *window=0;
  string readbuf;
  NextRecordDispatcher linebuf;
  deque<IntIntCB> result_cb;
  int movesecs=0;

  UniversalChessInterfaceEngine() { linebuf.cb = bind(&UniversalChessInterfaceEngine::LineCB, this, _1); }
  virtual ~UniversalChessInterfaceEngine() { if (process.in) app->scheduler.DelMainWaitSocket(window, fileno(process.in)); }

  bool Start(const string &bin, Window *w=0) {
    if (process.in) return false;
    vector<const char*> argv{ bin.c_str(), nullptr };
    if (process.Open(argv.data(), app->startdir.c_str())) return false;
    Socket fd = fileno(process.in);
    SystemNetwork::SetSocketBlocking(fd, false);
    if ((window = w)) app->scheduler.AddMainWaitSocket(window, fd, SocketSet::READABLE, bind(&UniversalChessInterfaceEngine::ReadCB, this));
    CHECK(FWriteSuccess(process.out, "uci\nisready\n" ));
    return true;
  }

  void Close() {
    if (process.in) {
      if (window) app->scheduler.DelMainWaitSocket(window, fileno(process.in));
      process.Close();
    }
  }

  void Analyze(Chess::Game *game, IntIntCB callback) {
    result_cb.emplace_back(move(callback));
    // string position = StrCat("startpos moves ", game->LongAlgebraicMoveList());
    string position = StrCat("fen ", game->position.GetFEN());
    CHECK(FWriteSuccess(process.out, StrCat("ucinewgame\nposition ", position,
                                            "\ngo movetime ", movesecs, "\n")));
  }

  bool ReadCB() {
    readbuf.resize(16384);
    if (NBRead(fileno(process.in), &readbuf) < 0) { ERROR("UniversalChessInterfaceEngine::ReadCB"); Close(); return true; }
    if (readbuf.size()) { linebuf.AddData(readbuf, false); return true; }
    return false;
  }

  void LineCB(const StringPiece &linebuf) {
    string line = linebuf.str();
    // INFO("UniversalChessInterfaceEngine '", line, "'");
    if (PrefixMatch(line, "bestmove ") && line.size() >= 13) {
      if (result_cb.size()) {
        if (result_cb.front()) result_cb.front()(Chess::SquareID(line.substr(9, 2).c_str()),
                                                 Chess::SquareID(line.substr(11, 2).c_str()));
        result_cb.pop_front();
      }
    }
  }
};

#ifdef LFL_CORE_APP_GUI_H__
struct ChessTerminal : public Terminal {
  Terminal::Controller *controller=0;
  function<Chess::Game*(int)> get_game_cb;
  UnbackedTextBox local_cmd;
  string my_name;

  Callback login_cb, illegal_move_cb;
  function<void(int)> game_start_cb;
  function<void(int, const string&, const string&, const string&)> game_over_cb;
  function<void(Chess::Game*, bool, int, int)> game_update_cb;

  ChessTerminal(ByteSink *O, Window *W, const FontRef &F, const point &dim) : Terminal(O, W, F, dim), local_cmd(F) {}

  virtual void Input(char k) { local_cmd.Input(k); Terminal::Write(StringPiece(&k, 1)); }
  virtual void Erase      () { local_cmd.Erase();  Terminal::Write(StringPiece("\x08\x1b[1P")); }
  virtual void Enter      () {
    string cmd = String::ToUTF8(local_cmd.cmd_line.Text16());
    if (cmd == "console" && root && root->shell) root->shell->console(StringVec());
    else if (sink) sink->Write(StrCat(cmd, "\n"));
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

  virtual void Send(const string &) = 0;
  virtual void MakeMove(const string&) = 0;
};
#endif // LFL_CORE_APP_GUI_H__

}; // namespace LFL
#endif // LFL_CHESS_CHESS_H__
