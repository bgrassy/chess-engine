#include <iostream>
#include "bitboard.hpp"

U64 squareBB[64];
U64 pawnAttacks[2][64];
U64 knightAttacks[64];
U64 kingAttacks[64];

void Bitboard::printBB(U64 b) {
    int BArray[64];
    for (int i = 0; i < 64; ++i) {
        BArray[i] = (b >> i) & 1; 
    }
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            std::cout << BArray[8 * i + j];
        }
        std::cout << std::endl;
    }
}
void Bitboard::init() {
    // initialize bitboard of squares 
    for (int i = 0; i < 64; i++) {
        squareBB[i] = 1ULL << i;
    }

    // initialize white pawn attacks
    for (int i = 0; i < 64; i++) {
        pawnAttacks[0][i] = ((squareBB[i] << 7) & ~HFile) | ((squareBB[i] << 9) & ~AFile);
    }
    
    // initialize black pawn attacks
    for (int i = 0; i < 64; i++) {
        pawnAttacks[1][i] = ((squareBB[i] >> 7) & ~AFile) | ((squareBB[i] >> 9) & ~HFile);
    }

    // initialize knight attacks
    for (int i = 0; i < 64; i++) {
        knightAttacks[i] = (squareBB[i] << 15) & ~HFile;
        knightAttacks[i] |= (squareBB[i] << 17) & ~AFile;
        knightAttacks[i] |= (squareBB[i] >> 15) & ~AFile;
        knightAttacks[i] |= (squareBB[i] >> 17) & ~HFile;
        knightAttacks[i] |= (squareBB[i] << 10) & ~AFile & ~BFile;
        knightAttacks[i] |= (squareBB[i] << 6) & ~GFile & ~HFile;
        knightAttacks[i] |= (squareBB[i] >> 6) & ~AFile & ~BFile;
        knightAttacks[i] |= (squareBB[i] >> 10) & ~GFile & ~HFile;
    }

    for(int i = 0; i < 64; i++) {
        kingAttacks[i] = pawnAttacks[0][i] | pawnAttacks[1][i];
        kingAttacks[i] |= (squareBB[i] << 1) & ~AFile;
        kingAttacks[i] |= (squareBB[i] >> 1) & ~HFile;
        kingAttacks[i] |= (squareBB[i] << 8);
        kingAttacks[i] |= (squareBB[i] >> 8);
        printBB(kingAttacks[i]);
        std::cout << std::endl;
    }
}
