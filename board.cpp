#include "board.hpp"

board::board() {
    white_move = true;
    pieceBB[0] = 0x000000000000FFFF; // white pieces
    pieceBB[1] = 0xFFFF000000000000; // black pieces
    pieceBB[2] = 0x00FF00000000FF00; // pawns
    pieceBB[3] = 0x4200000000000042; // knights
    pieceBB[4] = 0x2400000000000024; // bishops
    pieceBB[5] = 0x8100000000000081; // rooks
    pieceBB[6] = 0x0800000000000008; // queens
    pieceBB[7] = 0x1000000000000010; // kings
    for (int i = 0; i < 64; i++) {
        lookup[i] = (U64)1 << i;
    }
}


// Returns a bitboard representing all the pieces on the board.
U64 board::getPieces() {
    return pieceBB[0] | pieceBB[1];
}

// Takes a color. Returns a bitboard of all the pieces of that color.
U64 board::getPieces(color c) {
    return pieceBB[(int)c];
}

// Takes a piece type. Returns a bitboard of all the pieces of that type.
U64 board::getPieces(piece p) {
    return pieceBB[(int)p + 2];
}

// Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
U64 board::getPieces(color c, piece p) {
    return pieceBB[(int)c] & pieceBB[(int)p + 2];
}

void board::invert() {
    for (int i = 0; i < 8; i++) {
        pieceBB[i] = flip180(pieceBB[i]);
    }
}

U64 board::flip180(U64 x) {
    U64 h1 = 0x5555555555555555;
    U64 h2 = 0x3333333333333333;
    U64 h4 = 0x0F0F0F0F0F0F0F0F;
    U64 v1 = 0x00FF00FF00FF00FF;
    U64 v2 = 0x0000FFFF0000FFFF;
    x = ((x >>  1) & h1) | ((x & h1) <<  1);
    x = ((x >>  2) & h2) | ((x & h2) <<  2);
    x = ((x >>  4) & h4) | ((x & h4) <<  4);
    x = ((x >>  8) & v1) | ((x & v1) <<  8);
    x = ((x >> 16) & v2) | ((x & v2) << 16);
    x = (x >> 32) | (x << 32);
    return x;
}

bool board::sameRank(unsigned int square1, unsigned int square2) {
    return (square1 >> 3) == (square2 >> 3);
}

bool board::sameFile(unsigned int square1, unsigned int square2) {
    return (square1 & 7) == (square2 & 7);
}

bool board::sameDiagonal(unsigned int square1, unsigned int square2) {
   return bishop_attacks(lookup[square1]) & bishop_attacks(lookup[square2]);
}
// Checks to see if there are any pieces in between the two squares. 
bool board::inBetween(unsigned int square1, unsigned int square2) {
    if (square1 > square2) {
        int tmp = square1;
        square1 = square2;
        square2 = tmp;
    }
    if (sameRank(square1, square2)) { // in the same row
        for (unsigned int i = square1 + 1; i < square2; ++i) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameFile(square1, square2)) { // in the same column
        for (unsigned int i = square1 + 8; i < square2; i+=8) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameDiagonal(square1, square2)) {
        if ((square2 - square1) % 9 == 0) {
            for (unsigned int i = square1 + 9; i < square2; i+=9) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }
        } else if ((square2 - square1) % 7 == 0) {
            for (unsigned int i = square1 + 7; i < square2; i+=7) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }

        }
    }
    return false;
} 

bool board::legalMove(class move m) {
    int start = m.start;
    int end = m.end;
    color c = m.c;
    color opp = (color) (((int)c+1)%2);
    piece p = m.p;
    U64 startBB = lookup[start];
    U64 endBB = lookup[end];

    // checks if the piece is in the valid square or if the end piece is the same color
    if (c != getColor(start) || p != getPiece(start) || c == getColor(end)) {
        return false;
    }
    if (inBetween(start, end)) {
        return false;
    }
    // tests if the start and end square are the same
    if (start == end) {
        return false;
    }
    if (p == piece::Pawn) {
        if (pawn_moves(startBB) & endBB){
            return getPiece(end) == piece::None;
        } else if (pawn_attacks(startBB) & endBB) {
            return getColor(end) == opp;
        }
    } else if (p == piece::Knight) {
        return knight_attacks(startBB) & endBB;
    } else if (p == piece::Bishop) {
        return bishop_attacks(startBB) & endBB;
    } else if (p == piece::Rook) {
        return rook_attacks(startBB) & endBB;
    } else if (p == piece::Queen) {
        return queen_attacks(startBB) & endBB;
    } else if (p == piece::King) {
        return king_attacks(startBB) & endBB;
    }
    return false;
}

bool board::inCheck() {
    if (getPieces(color::White, piece::King) & (pawn_attacks(getPieces(color::Black, piece::Pawn)) | 
            knight_attacks(getPieces(color::Black, piece::Knight)))) {
        return true;
    } else if (getPieces(color::White, piece::King) & bishop_attacks(getPieces(color::Black, piece::Bishop))) {
        U64 b = getPieces(color::Black, piece::Bishop); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                U64 bishop = lookup[count];
                if (!inBetween(getPieces(color::White, piece::King), bishop)) {
                    return true;
                }
                b >>= 1;
                ++count;
            }
        }
    } else if (getPieces(color::White, piece::King) & rook_attacks(getPieces(color::Black, piece::Rook))) {
        U64 b = getPieces(color::Black, piece::Rook); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                U64 rook = lookup[count];
                if (!inBetween(getPieces(color::White, piece::King), rook)) {
                    return true;
                }
                b >>= 1;
                ++count;
            }
        }
    } else if (getPieces(color::White, piece::King) & queen_attacks(getPieces(color::Black, piece::Queen))) {
        U64 b = getPieces(color::Black, piece::Queen); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                U64 queen = lookup[count];
                if (!inBetween(getPieces(color::White, piece::King), queen)) {
                    return true;
                }
                b >>= 1;
                ++count;
            }
        }
    }
    return false;
}

color board::getColor(unsigned int square) {
    U64 b = lookup[square];
    if (b & getPieces(color::White)) {
        return color::White;
    } else if (b & getPieces(color::Black)) {
        return color::Black;
    }
    return color::None;
}

piece board::getPiece(unsigned int square) {
    U64 b = lookup[square];
    for (int i = 0; i < 6; i++) {
        if (b & getPieces((piece) i)) {
            return (piece) i;
        }
    }
    return piece::None;
}

void board::move(class move m) {
    int square1 = m.start;
    int square2 = m.end;
    if (legalMove(m)) {
       piece p = getPiece(square1);  
       color c = getColor(square1);
    }
}

