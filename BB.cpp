#include "BB.hpp"

typedef uint64_t U64;
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
// Takes a bitboard corresponding to pawn locations. Returns the set of squares that are attacked by them.
U64 pawn_attacks(U64 pawns) {
    U64 b = (pawns << 7) & ~HFile;
    return b | ((pawns << 9) & ~AFile);
}

// Takes a bitboard corresponding to pawn locations. Returns the set of squares the pawns can move to.
U64 pawn_moves(U64 pawns) {
    U64 b = pawns << 8;
    return b | ((pawns & Rank2) << 16); 
}

// Takes a bitboard corresponding to knight locations. Returns the set of squares the knights can move to.
U64 knight_attacks(U64 knights) {
    U64 b = (knights << 15) & ~(HFile);
    b |= (knights << 17) & ~(AFile);
    b |= (knights >> 15) & ~(AFile);
    b |= (knights >> 17) & ~(HFile);
    b |= (knights << 6) & ~(GFile) & ~(HFile);
    b |= (knights << 10) & ~(AFile) & ~(BFile);
    b |= (knights >> 6) & ~(AFile) & ~(BFile);
    b |= (knights >> 10) & ~(GFile) & ~(HFile);
    return b;
}


// Takes a bitboard holding the locations of bishops. Returns a bitboard holding all the squares that
// are attacked by those bishops.
U64 bishop_attacks(U64 b) {
    std::vector<int> indices;
    std::set<int> attacked;
    int count = 0;
    while (b > 0) {
        if (b & 1) {
            indices.push_back(count); 
        }
        b >>= 1;
        ++count;
    }
    for (int i : indices) {
        int t1 = i + 7;
        int t2 = i + 9;
        int t3 = i - 7;
        int t4 = i - 9;
        while (t1 % 8 != 7 && t1 <= 63 && t1 >= 0) {
            attacked.insert(t1);
            t1 += 7;
        }
        while (t2 % 8 != 0 && t2 <= 63 && t2 >= 0) {
            attacked.insert(t2);
            t2 += 9;
        }
        while (t3 % 8 != 0 && t3 <= 63 && t3 >= 0) {
            attacked.insert(t3);
            t3 -= 7;
        }
        while (t4 % 8 != 7 && t4 <= 63 && t4 >= 0) {
            attacked.insert(t4);
            t4 -= 9;
        }
    }
    U64 c = 0;
    for (int i : attacked) {
        c |= ((U64)1 << i);
    }
    return c;
}

// Takes a bitboard holding rook locations. Returns the set of squares the rooks can move to.
U64 rook_attacks(U64 rooks) {
    U64 files[] = {AFile, BFile, CFile, DFile, EFile, FFile, GFile, HFile};
    U64 ranks[] = {Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8};
    U64 b = 0;
    for (unsigned int i = 0; i < 8; i++) {
        if (files[i] & rooks) {
           b |= files[i]; 
        }
        if (ranks[i] & rooks) {
           b |= ranks[i]; 
        }
    }
    return b;
}

// Takes a bitboard holding queen locations. Returns the set of squares the queens can move to.
U64 queen_attacks(U64 queen) {
    return bishop_attacks(queen) | rook_attacks(queen);
}

// Takes a bitboard holding king locations. Returns the set of squares the kings can move to.
U64 king_attacks(U64 king) {
    U64 b = (king << 1) & ~HFile;
    b |= (king >> 1) & ~AFile;
    b |= king >> 8;
    b |= king << 8;
    b |= (king >> 9) & ~AFile;
    b |= (king >> 7) & ~HFile;
    b |= (king << 9) & ~HFile;
    b |= (king << 7) & ~AFile;
    return b;
}
