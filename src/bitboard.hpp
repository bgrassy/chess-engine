#ifndef BITBOARD
#define BITBOARD

#include "types.hpp"

const U64 AFile = 0x0101010101010101ULL;
const U64 BFile = AFile << 1;
const U64 CFile = AFile << 2;
const U64 DFile = AFile << 3;
const U64 EFile = AFile << 4;
const U64 FFile = AFile << 5;
const U64 GFile = AFile << 6;
const U64 HFile = AFile << 7;

const U64 Rank1 = 0xFFULL;
const U64 Rank2 = Rank1 << 8;
const U64 Rank3 = Rank1 << (8 * 2);
const U64 Rank4 = Rank1 << (8 * 3);
const U64 Rank5 = Rank1 << (8 * 4);
const U64 Rank6 = Rank1 << (8 * 5);
const U64 Rank7 = Rank1 << (8 * 6);
const U64 Rank8 = Rank1 << (8 * 7);

extern U64 squareBB[64];
extern U64 pawnAttacks[2][64];
extern U64 knightAttacks[64];
extern U64 kingAttacks[64];

namespace Bitboard {
    void printBB(U64 b);
    void init();
}

#endif /* ifndef BITBOARD */
