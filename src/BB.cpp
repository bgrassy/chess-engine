#include "BB.hpp"

// Takes a bitboard corresponding to pawn locations. Returns the set of squares that are attacked by them.
U64 pawn_attacks(U64 pawns, Color c) {
    if (c == Color::White) {
        U64 b = (pawns << 7) & ~HFile;
        return b | ((pawns << 9) & ~AFile);
    } else {
        U64 b = (pawns >> 7) & ~AFile;
        return b | ((pawns >> 9) & ~HFile);

    }
}

// Takes a bitboard corresponding to pawn locations. Returns the set of squares the pawns can move to.
U64 single_pawn_moves(U64 pawns, Color c) {
    if (c == Color::White) {
        return pawns << 8;
    } else {
        return pawns >> 8;
    }
}

U64 double_pawn_moves(U64 pawns, Color c) {
    if (c == Color::White) {
        return (pawns & Rank2) << 16; 
    } else {
        return (pawns & Rank7) >> 16; 
    }

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
U64 bishop_attacks(U64 bishops, U64 occupied) {
    U64 ret = 0;
    int count = 0;
    while (bishops > 0) {
        if (bishops & 1) {
            ret ^= Bmagic(count, occupied);
        }
        bishops >>= 1;
        ++count;
    }
    return ret;
}

// Takes a bitboard holding rook locations. Returns the set of squares the rooks can move to.
U64 rook_attacks(U64 rooks, U64 occupied) {
    U64 ret = 0;
    int count = 0;
    while (rooks > 0) {
        if (rooks & 1) {
            ret ^= Rmagic(count, occupied);
        }
        rooks >>= 1;
        ++count;
    }
    return ret;
}

// Takes a bitboard holding queen locations. Returns the set of squares the queens can move to.
U64 queen_attacks(U64 queen, U64 occupied) {
    return bishop_attacks(queen, occupied) | rook_attacks(queen, occupied);
}

// Takes a bitboard holding king locations. Returns the set of squares the kings can move to.
U64 king_attacks(U64 king) {
    U64 b = (king << 1) & ~AFile;
    b |= (king >> 1) & ~HFile;
    b |= king >> 8;
    b |= king << 8;
    b |= (king >> 9) & ~HFile;
    b |= (king >> 7) & ~AFile;
    b |= (king << 9) & ~AFile;
    b |= (king << 7) & ~HFile;
    return b;
}
