#include "bitboard.hpp"

extern const Bitboard AFile = 0x0101010101010101;
extern const Bitboard BFile = (AFile << 1);      
extern const Bitboard CFile = (AFile << 2);      
extern const Bitboard DFile = (AFile << 3);      
extern const Bitboard EFile = (AFile << 4);      
extern const Bitboard FFile = (AFile << 5);      
extern const Bitboard GFile = (AFile << 6);      
extern const Bitboard HFile = (AFile << 7);      
                                                 
extern const Bitboard Rank1 = 0xFF;              
extern const Bitboard Rank2 = (Rank1 << 8);      
extern const Bitboard Rank3 = (Rank2 << 8);      
extern const Bitboard Rank4 = (Rank3 << 8);      
extern const Bitboard Rank5 = (Rank4 << 8);      
extern const Bitboard Rank6 = (Rank5 << 8);
extern const Bitboard Rank7 = (Rank6 << 8);      
extern const Bitboard Rank8 = (Rank7 << 8);      

const Bitboard sqToBB[64] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
    4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152,
    4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456,
    536870912, 1073741824, 2147483648LL, 4294967296, 8589934592, 17179869184,
    34359738368, 68719476736, 137438953472, 274877906944, 549755813888,
    1099511627776, 2199023255552, 4398046511104, 8796093022208, 17592186044416,
    35184372088832, 70368744177664, 140737488355328, 281474976710656,
     562949953421312, 1125899906842624, 2251799813685248, 4503599627370496,
    9007199254740992, 18014398509481984, 36028797018963968, 72057594037927936,
    144115188075855872, 288230376151711744, 576460752303423488,
    1152921504606846976, 2305843009213693952, 4611686018427387904,
    9223372036854775808ULL 
};

const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};
const Bitboard debruijn64 = 0x03f79d71b4cb0a89ULL;

Bitboard pawnAttacks[2][64];
Bitboard knightAttacks[64];
Bitboard kingAttacks[64];

int popcount(Bitboard b) {
    int count = 0;
    while (b) {
        count++;
        b &= b - 1;
    }
    return count; 
}

// Initialize key bitboard constants
void initBitboards() {
    for (int i = 0; i < 64; i++) {
        Bitboard b = sqToBB[i];
        pawnAttacks[nWhite][i] = (b << 7) & ~HFile;
        pawnAttacks[nWhite][i] |= (b << 9) & ~AFile;
        pawnAttacks[nBlack][i] = (b >> 7) & ~AFile;
        pawnAttacks[nBlack][i] |= (b >> 9) & ~HFile;

        knightAttacks[i] = (b << 17) & ~AFile;
        knightAttacks[i] |= (b << 10) & ~AFile &~BFile;
        knightAttacks[i] |= (b >>  6) & ~AFile & ~BFile;
        knightAttacks[i] |= (b >> 15) & ~AFile;
        knightAttacks[i] |= (b << 15) & ~HFile;
        knightAttacks[i] |= (b <<  6) & ~GFile & ~HFile;
        knightAttacks[i] |= (b >> 10) & ~GFile & ~HFile;
        knightAttacks[i] |= (b >> 17) & ~HFile;

        kingAttacks[i] = (b << 1) & ~HFile;
        kingAttacks[i] |= (b >> 1) & ~AFile;
        kingAttacks[i] |= (kingAttacks[i] << 8) | (kingAttacks[i] >> 8);
        kingAttacks[i] |= (b << 8) | (b >> 8);
    }
    initmagicmoves();
}

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param b bitboard to scan
 * @requires b != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(Bitboard b) {
   return index64[((b ^ (b-1)) * debruijn64) >> 58];
}

/**
 * bitScanReverse
 * @authors Kim Walisch, Mark Dickinson
 * @param b bitboard to scan
 * @requires b != 0
 * @return index (0..63) of most significant one bit
 */
int bitScanReverse(Bitboard b) {
   b |= b >> 1; 
   b |= b >> 2;
   b |= b >> 4;
   b |= b >> 8;
   b |= b >> 16;
   b |= b >> 32;
   return index64[(b * debruijn64) >> 58];
}
