#ifndef BB_H
#define BB_H
#include <vector>
#include <set>
#include <stdint.h>
typedef uint64_t U64;
U64 pawn_attacks(U64 pawns);

U64 pawn_moves(U64 pawns);

// Takes a bitboard corresponding to knight locations. Returns the set of squares the knights can move to.
U64 knight_attacks(U64 knights);


// Takes a bitboard holding the locations of bishops. Returns a bitboard holding all the squares that
// are attacked by those bishops.
U64 bishop_attacks(U64 b);

// Takes a bitboard holding rook locations. Returns the set of squares the rooks can move to.
U64 rook_attacks(U64 rooks);

// Takes a bitboard holding queen locations. Returns the set of squares the queens can move to.
U64 queen_attacks(U64 queen);

// Takes a bitboard holding king locations. Returns the set of squares the kings can move to.
U64 king_attacks(U64 king);
#endif /*BB_H*/
