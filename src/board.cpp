typedef unsigned long long U64;
#include "board.hpp"

CBoard::CBoard() {
    for (int i = 0; i < 64; i++) {
        lookup[i] = (U64) 1 << i;
    }
    pieceBB[0] = 255 | (255 << 8);
    pieceBB[1] = (255ULL << 48) | (255ULL << 56);
    pieceBB[2] = (255 << 8) | (255ULL << 48);
    pieceBB[3] = lookup[1] | lookup[6];
    pieceBB[4] = lookup[2] | lookup[5];
    pieceBB[5] = lookup[0] | lookup[7];
    pieceBB[6] = lookup[3];
    pieceBB[7] = lookup[4];
    pieceBB[8] = (255ULL << 48);
    pieceBB[9] = lookup[57] | lookup[62];
    pieceBB[10] = lookup[58] | lookup[61];
    pieceBB[11] = lookup[56] | lookup[63];
    pieceBB[12] = lookup[59];
    pieceBB[13] = lookup[60];
    occupiedBB = pieceBB[0] & pieceBB[1];
    emptyBB = ~occupiedBB; 
    whiteMove = true;
    castling = 0b1111;
    enPassant = -1;
}

U64 CBoard::getPieceType(enumPiece p) const { return pieceBB[p]; }

U64 CBoard::getOccupied() const { return occupiedBB; }

U64 CBoard::getEmpty() const { return emptyBB; }

U64 CBoard::getWhite() const { return pieceBB[nWhite]; }

U64 CBoard::getBlack() const { return pieceBB[nBlack]; }

U64 CBoard::getWhitePawns() const { return pieceBB[nWhitePawn]; }

U64 CBoard::getWhiteKnights() const { return pieceBB[nWhiteKnight]; }

U64 CBoard::getWhiteBishops() const { return pieceBB[nWhiteBishop]; }

U64 CBoard::getWhiteRooks() const { return pieceBB[nWhiteRook]; }

U64 CBoard::getWhiteQueens() const { return pieceBB[nWhiteQueen]; }

U64 CBoard::getWhiteKings() const { return pieceBB[nWhiteKing]; }

U64 CBoard::getBlackPawns() const { return pieceBB[nBlackPawn]; }

U64 CBoard::getBlackKnights() const { return pieceBB[nBlackKnight]; }

U64 CBoard::getBlackBishops() const { return pieceBB[nBlackBishop]; }

U64 CBoard::getBlackRooks() const { return pieceBB[nBlackRook]; }

U64 CBoard::getBlackQueens() const { return pieceBB[nBlackQueen]; }

U64 CBoard::getBlackKings() const { return pieceBB[nBlackKing]; }

bool CBoard::attacked(U64 occupied, unsigned int square, enumColor c) {
    U64 pawns = pieceBB[nWhitePawn + c];
    U64 knights = pieceBB[nWhiteKnight + c];
    U64 bishopsQueens = pieceBB[nWhiteBishop + c] | pieceBB[nWhiteQueen + c];
    U64 rooksQueens = pieceBB[nWhiteRook + c] | pieceBB[nWhiteQueen + c];
    U64 king = pieceBB[nWhiteQueen + c];
    return pawnAttacks[c][square];
}
