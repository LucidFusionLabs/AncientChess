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

//                                      { 0, pawns,         knights,     bishops,     rooks,       queen,       king       };
static const BitBoard white_initial[] = { 0, 0xff00ULL,     0x42ULL,     0x24ULL,     0x81ULL,     0x10ULL,     0x8ULL     };
static const BitBoard black_initial[] = { 0, 0xff00ULL<<40, 0x42ULL<<56, 0x24ULL<<56, 0x81ULL<<56, 0x10ULL<<56, 0x8ULL<<56 };
static const BitBoard black_castle_path = 0x600000000000000LL, black_castle_long_path = 0x7000000000000000LL;
static const BitBoard white_castle_path = 0x6LL,               white_castle_long_path = 0x70LL;

static const char initial_byte_board[] = 
"rnbqkbnr\n"
"pppppppp\n"
"--------\n"
"--------\n"
"--------\n"
"--------\n"
"PPPPPPPP\n"
"RNBQKBNR\n";

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

static int8_t PieceCharType(char c) {
  switch(c) {
    case 'p': case 'P': return PAWN;
    case 'n': case 'N': return KNIGHT;
    case 'b': case 'B': return BISHOP;
    case 'r': case 'R': return ROOK;
    case 'q': case 'Q': return QUEEN;
    case 'k': case 'K': return KING;
    default:            return 0;
  }
}

inline uint8_t GetPieceType(Piece piece) { return (piece & 7); }
inline bool GetPieceColor(Piece piece) { return piece & (1<<3); }
inline Piece GetPiece(bool color, uint8_t piece) {
  return (uint8_t(color) << 3) | (piece & 7);
}

inline uint8_t GetMovePieceType (Move move) { return (move >> 26) & 0x7; }
inline uint8_t GetMoveFromSquare(Move move) { return (move >> 20) & 0x3f; }
inline uint8_t GetMoveToSquare  (Move move) { return (move >> 14) & 0x3f; }
inline uint8_t GetMoveCapture   (Move move) { return (move >> 29) & 0x7; }
inline Move GetMove(uint8_t piece, uint8_t start_square, uint8_t end_square, uint8_t capture, uint8_t flags) {
  return ((capture & 7) << 29) | ((piece & 7) << 26) | ((start_square & 0x3f) << 20) |
    ((end_square & 0x3f) << 14);
}

inline int8_t SquareX(int s) { return 7 - (s % 8); }
inline int8_t SquareY(int s) { return s / 8; }
inline int8_t SquareFromXY(int x, int y) { return (x<0 || y<0 || x>7 || y>7) ? -1 : (y*8 + (7-x)); }
inline BitBoard SquareMask(int s) { return 1LL << s; }
inline int8_t SquareID(const char *s) {
  if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8') return ERRORv(-1, "unknown square: ", s);
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

string SquareMaskString(int s) {
  string ret;
  for (int i=7; i>=0; i--) {
    for (int j=0; j<8; j++) StrAppend(&ret, SquareY(s) == i && SquareX(s) == j);
    StrAppend(&ret, "\n");
  }
  return ret;
}

string BitBoardToString(BitBoard b) {
  string ret;
  for (int i=7; i>=0; i--) {
    for (int j=7; j>=0; j--) StrAppend(&ret, bool(b & (1ULL<<(i*8+j))));
    StrAppend(&ret, "\n");
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

struct PieceIter { 
  BitBoard pieces;
  PieceIter(BitBoard v) : pieces(v) {}
  uint8_t GetSquare() const { return ffsll(pieces) - 1; }
  operator bool() const { return pieces; }
  PieceIter& operator++() { pieces &= (pieces-1); return *this; }
};

struct MoveIter { 
  BitBoard moves;
  MoveIter(BitBoard v) : moves(v) {}
  uint8_t GetTargetSquare() const { return ffsll(moves) - 1; }
  operator bool() const { return moves; }
  MoveIter& operator++() { moves &= (moves-1); return *this; }
};

}; // namespace Chess
}; // namespace LFL
#include "magic.h"
namespace LFL {
namespace Chess {

struct PositionFlags {
  uint8_t fifty_move_rule_count:6, to_move_color:1, w_cant_castle:1, b_cant_castle:1,
          w_cant_castle_long:1, b_cant_castle_long:1, previous_move_was_double_step_pawn_advance:1;
};

struct Position {
  Move move=0;
  PositionFlags flags;
  uint16_t move_number=0;
  BitBoard white[7], black[7];

  Position() { Reset(); }
  Position(const char *b) { LoadByteBoard(b); }
  void Assign(const Position &p) { *this = p; }

  void Reset() {
    move = 0;
    move_number = 0;
    memzero(flags);
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

  void UpdateMove(bool new_move, int8_t piece_type, int8_t square_from, int8_t square_to, int8_t captured, uint8_t move_flags) {
    bool piece_color = move_number % 2 == 0;
    CHECK_EQ(GetPiece(piece_color, piece_type), GetSquare(square_to));
    move = Chess::GetMove(piece_type, square_from, square_to, captured, move_flags);
    if (!new_move) return;

    if (piece_type == PAWN && abs(SquareY(square_to) - SquareY(square_from)) == 2)
      flags.previous_move_was_double_step_pawn_advance = square_to;
    if (piece_color == WHITE) {
      if      (square_from == e1) flags.w_cant_castle = flags.w_cant_castle_long = true;
      else if (square_from == h1) flags.w_cant_castle = true;
      else if (square_from == a1) flags.w_cant_castle_long = true;
    } else {
      if      (square_from == e8) flags.b_cant_castle = flags.b_cant_castle_long = true;
      else if (square_from == h8) flags.b_cant_castle = true;
      else if (square_from == a8) flags.b_cant_castle_long = true;
    }
    if (captured || piece_type == PAWN) flags.fifty_move_rule_count = 0;
    else                                flags.fifty_move_rule_count++;
  }

  BitBoard AllPieces() const { return white[ALL] | black[ALL]; }
  const BitBoard *Pieces(bool color) const { return color ? black : white; }
  /**/  BitBoard *Pieces(bool color)       { return color ? black : white; }

  void SetAll(bool color) {
    BitBoard *pieces = Pieces(color);
    pieces[ALL] = pieces[PAWN] | pieces[KNIGHT] | pieces[BISHOP] | pieces[ROOK] | pieces[QUEEN] | pieces[KING];
  }

  void SetSquare(int s, Piece piece) {
    BitBoard mask = (static_cast<BitBoard>(1) << s);
    uint8_t piece_color = GetPieceColor(piece), piece_type = GetPieceType(piece);
    CHECK_RANGE(piece_type, PAWN, END_PIECES);
    if (piece_color) { black[piece_type] |= mask; black[0] |= mask; }
    else             { white[piece_type] |= mask; white[0] |= mask; }
  }

  Piece GetSquare(int s) const {
    BitBoard mask = (static_cast<BitBoard>(1) << s);
    for (int i=1; i<7; i++) if (white[i] & mask) return GetPiece(WHITE, i);
    for (int i=1; i<7; i++) if (black[i] & mask) return GetPiece(BLACK, i);
    return GetPiece(WHITE, 0);
  }

  Piece ClearSquare(int s, bool clear_white=true, bool clear_black=true) {
    BitBoard mask = (static_cast<BitBoard>(1) << s);
    if (clear_white) for (int i=PAWN; i!=END_PIECES; i++) if (white[i] & mask) { white[0] &= ~mask; white[i] &= ~mask; return GetPiece(WHITE, i); }
    if (clear_black) for (int i=PAWN; i!=END_PIECES; i++) if (black[i] & mask) { black[0] &= ~mask; black[i] &= ~mask; return GetPiece(BLACK, i); }
    return GetPiece(WHITE, 0);
  }
  
  BitBoard PawnEnPassant(int p, bool black) const {
    if (!flags.previous_move_was_double_step_pawn_advance ||
        SquareY(flags.previous_move_was_double_step_pawn_advance) != SquareY(p)) return 0;
    if      (flags.previous_move_was_double_step_pawn_advance + 1 == p) return SquareMask(black ? p-9 : p+7);
    else if (flags.previous_move_was_double_step_pawn_advance - 1 == p) return SquareMask(black ? p-7 : p+9);
    else                                                                return 0;
  }

  BitBoard PawnAttacks(int p, bool black) const {
    return black ? (black_pawn_attack_mask[p] & Pieces(!black)[ALL])
                 : (white_pawn_attack_mask[p] & Pieces(!black)[ALL]);
  }

  BitBoard PawnAdvances(int p, bool black) const {
    BitBoard ret = black ? (black_pawn_occupancy_mask[p] & ~AllPieces())
                         : (white_pawn_occupancy_mask[p] & ~AllPieces());
    if (black) { if (SquareY(p) == 6) ret &= ((ret >> 8) | 0xffffff00ffffffffLL); }
    else       { if (SquareY(p) == 1) ret &= ((ret << 8) | 0xffffffff00ffffffLL); }
    return ret;
  }

  BitBoard PawnMoves(int p, bool black) const {
    return PawnAdvances(p, black) | PawnAttacks(p, black) | PawnEnPassant(p, black);
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

  BitBoard KingMoves(int p, bool black) const {
    BitBoard ret = king_occupancy_mask[p] & ~Pieces(black)[ALL];
    if (black) {
      if (!flags.b_cant_castle      && !(AllPieces() & black_castle_path))      ret |= SquareMask(g8); 
      if (!flags.b_cant_castle_long && !(AllPieces() & black_castle_long_path)) ret |= SquareMask(c8); 
    } else {
      if (!flags.w_cant_castle      && !(AllPieces() & white_castle_path))      ret |= SquareMask(g1); 
      if (!flags.w_cant_castle_long && !(AllPieces() & white_castle_long_path)) ret |= SquareMask(c1); 
    }
    return ret;
  }

  BitBoard PieceMoves(int piece, int square, bool black) const {
    if      (piece == PAWN)   return PawnMoves  (square, black);
    else if (piece == KNIGHT) return KnightMoves(square, black);
    else if (piece == BISHOP) return BishopMoves(square, black);
    else if (piece == ROOK)   return RookMoves  (square, black);
    else if (piece == QUEEN)  return QueenMoves (square, black);
    else if (piece == KING)   return KingMoves  (square, black);
    else                      FATAL("unknown piece ", piece);
  }

  BitBoard PieceAttacks(int piece, int square, bool black) const {
    if      (piece == PAWN)   return PawnAttacks(square, black) | PawnEnPassant(square, black);
    else if (piece == KNIGHT) return KnightMoves(square, black);
    else if (piece == BISHOP) return BishopMoves(square, black);
    else if (piece == ROOK)   return RookMoves  (square, black);
    else if (piece == QUEEN)  return QueenMoves (square, black);
    else if (piece == KING)   return KingMoves  (square, black);
    else                      FATAL("unknown piece ", piece);
  }

  BitBoard AllAttacks(bool color) const {
    BitBoard ret = 0;
    for (int piece_type = PAWN; piece_type != END_PIECES; ++piece_type)
      for (PieceIter p(Pieces(color)[piece_type]); p; ++p)
        ret |= PieceAttacks(piece_type, p.GetSquare(), color);
    return ret;
  }

  bool InCheck(BitBoard attacks, bool color) const {
    return attacks & Pieces(color)[KING];
  }
};

struct SearchState {
  struct Total { int nodes, captures, enpassants, castles, promotions, checks, checkmates; };
  Total total;
  vector<Total> depth;
  unordered_map<Move, Total> divide;
  int max_depth=0;

  void CountMove(int di, bool capture, Total *divide=0) {
    total.nodes++;
    if (capture) total.captures++;

    if (divide && di+1 == max_depth) {
      divide->nodes++;
      if (capture) divide->captures++;
    }
    if (di >= depth.size()) depth.resize(di+1);

    depth[di].nodes++;
    if (capture) depth[di].captures++;
  }
};

vector<Position> GenerateMoves(const Position &in, bool color) {
  vector<Position> ret;
  uint8_t square_from, square_to;
  for (int piece_type = PAWN; piece_type != END_PIECES; ++piece_type)
    for (PieceIter p(in.Pieces(color)[piece_type]); p; ++p)
      for (MoveIter m(in.PieceMoves(piece_type, (square_from = p.GetSquare()), color)); m; ++m) {
        ret.emplace_back(in);
        square_to = m.GetTargetSquare();
        Position &position = ret.back();
        Piece piece          = position.ClearSquare(square_from, color == WHITE, color == BLACK);
        Piece captured_piece = position.ClearSquare(square_to,   color != WHITE, color != BLACK);
        int captured_piece_type = GetPieceType(captured_piece);
        position.move_number++;
        position.flags.to_move_color = !color;
        position.SetSquare(square_to, piece);
        position.UpdateMove(true, piece, square_from, square_to, captured_piece_type, 0);
        if (position.InCheck(position.AllAttacks(!color), color)) ret.pop_back();
      }
  return ret;
}

void FullSearch(Position in, bool color, SearchState *count, int depth=0, SearchState::Total *divide=0) {
  auto moves = GenerateMoves(in, color);
  for (auto &m : moves) {
    unsigned char move_from = GetMoveFromSquare(m.move), move_to = GetMoveToSquare(m.move);
    if (!depth) divide = &count->divide[GetMove(GetMovePieceType(m.move), move_from, move_to, 0, 0)];
    if (count) count->CountMove(depth, GetMoveCapture(m.move), divide);
    // INFO(string(depth*4, ' '), SquareName(move_from), "-", SquareName(move_to));
    if (depth+1 >= count->max_depth) continue;
    FullSearch(m, !color, count, depth+1, divide);
  }
}

struct GamePosition : public Position {
  string name;
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
    history.push_back(position);
    last_position = position;
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
    string text;
    if (game->history.size())
      for (auto b = game->history.begin()+1, e = game->history.end(), i = b; i != e; ++i)
        StrAppend(&text, i->name.substr(1), " ");
    result_cb.emplace_back(move(callback));
    CHECK(FWriteSuccess(process.out, StrCat("ucinewgame\nposition startpos moves ", text,
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

  StringCB login_cb;
  Callback illegal_move_cb;
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
