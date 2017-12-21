#ifndef BB_H
#define BB_H
#include <vector>
#include <set>
#include "types.hpp"
#include "MagicMoves.hpp"

using namespace Types;
typedef unsigned long long U64;
const U64 Rank1 = 0xFF;
const U64 Rank2 = 0xFF00;
const U64 Rank3 = 0xFF0000;
const U64 Rank4 = 0xFF000000;
const U64 Rank5 = 0xFF00000000;
const U64 Rank6 = 0xFF0000000000;
const U64 Rank7 = 0xFF000000000000;
const U64 Rank8 = 0xFF00000000000000;
const U64 AFile = 0x0101010101010101;
const U64 BFile = 0x0202020202020202;
const U64 CFile = 0x0404040404040404;
const U64 DFile = 0x0808080808080808;
const U64 EFile = 0x1010101010101010;
const U64 FFile = 0x2020202020202020;
const U64 GFile = 0x4040404040404040;
const U64 HFile = 0x8080808080808080;
U64 pawn_attacks(U64 pawns, Color c);

U64 single_pawn_moves(U64 pawns, Color c);

U64 double_pawn_moves(U64 pawns, Color c);
// Takes a bitboard corresponding to knight locations. Returns the set of squares the knights can move to.
U64 knight_attacks(U64 knights);


// Takes a bitboard holding the locations of bishops. Returns a bitboard holding all the squares that
// are attacked by those bishops.
U64 bishop_attacks(U64 bishops, U64 occupied);

// Takes a bitboard holding rook locations. Returns the set of squares the rooks can move to.
U64 rook_attacks(U64 rooks, U64 occupied);

// Takes a bitboard holding queen locations. Returns the set of squares the queens can move to.
U64 queen_attacks(U64 queen, U64 occupied);

// Takes a bitboard holding king locations. Returns the set of squares the kings can move to.
U64 king_attacks(U64 king);
#endif /*BB_H*/
