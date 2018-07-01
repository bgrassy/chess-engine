/**
 * Constructs a new Board object to the starting chess position.
 */

#include "board.hpp"

Board::Board() {
    // initialize pieces
    pieceBB[0] = Rank1 | Rank2;
    pieceBB[1] = Rank7 | Rank8;
    pieceBB[2] = Rank2 | Rank7;
    pieceBB[3] = sqToBB[B1] | sqToBB[G1] | sqToBB[B8] | sqToBB[G8];
    pieceBB[4] = sqToBB[C1] | sqToBB[F1] | sqToBB[C8] | sqToBB[F8];
    pieceBB[5] = sqToBB[A1] | sqToBB[H1] | sqToBB[A8] | sqToBB[H8];
    pieceBB[6] = sqToBB[D1] | sqToBB[D8];
    pieceBB[7] = sqToBB[E1] | sqToBB[E8];

    occupiedBB = (pieceBB[0] | pieceBB[1]);
    emptyBB = ~occupiedBB;

    enPassant = NONE;
    castling[0] = true;
    castling[1] = true;
    castling[2] = true;
    castling[3] = true;
}

/**
 * Constructs a new Board object to the given position
 *
 * @param FEN the desired starting position in FEN form
 */
Board::Board(std::string FEN) {
    
}

// Returns pieces of the given piece type
Bitboard Board::getPieces(Piece pt) {
    return pieceBB[2 + pt];
}

// Returns pieces of the given color
Bitboard Board::getPieces(Color ct) {
    return pieceBB[ct];
}

// Returns pieces of the given piece and color
Bitboard Board::getPieces(Piece pt, Color ct) {
    return pieceBB[2 + pt] & pieceBB[ct];
}

// Returns bitboard of all the white pawns
Bitboard Board::getWhitePawns() {
    return pieceBB[0] & pieceBB[2];
}

// Returns bitboard of all the white knights
Bitboard Board::getWhiteKnights() {
    return pieceBB[0] & pieceBB[3];
}

// Returns bitboard of all the white bishops
Bitboard Board::getWhiteBishops() {
    return pieceBB[0] & pieceBB[4];
}

// Returns bitboard of all the white rooks
Bitboard Board::getWhiteRooks() {
    return pieceBB[0] & pieceBB[5];
}

// Returns bitboard of all the white queens
Bitboard Board::getWhiteQueens() {
    return pieceBB[0] & pieceBB[6];
}

// Returns bitboard of the white king
Bitboard Board::getWhiteKing() {
    return pieceBB[0] & pieceBB[7];
}

// Returns bitboard of all the black pawns
Bitboard Board::getBlackPawns() {
    return pieceBB[1] & pieceBB[2];
}

// Returns bitboard of all the black knights
Bitboard Board::getBlackKnights() {
    return pieceBB[1] & pieceBB[3];
}

// Returns bitboard of all the black bishops
Bitboard Board::getBlackBishops() {
    return pieceBB[1] & pieceBB[4];
}

// Returns bitboard of all the black rooks
Bitboard Board::getBlackRooks() {
    return pieceBB[1] & pieceBB[5];
}

// Returns bitboard of all the black queens
Bitboard Board::getBlackQueens() {
    return pieceBB[1] & pieceBB[6];
}

// Returns bitboard of the black king
Bitboard Board::getBlackKing() {
    return pieceBB[1] & pieceBB[7];
}

Bitboard Board::getOccupied() {
    return occupiedBB;
}

Bitboard Board::getEmpty() {
    return emptyBB;
}

enumSquare Board::enPassantTarget() {
    return enPassant;
}

bool Board::attacked(Bitboard occupied, enumSquare square, Color side) {
   Bitboard pawns = getPieces(nPawn, side);
   if (pawnAttacks[side^1][square] & pawns) {
       return true;
   }
   Bitboard knights = getPieces(nKnight, side);
   if (knightAttacks[square] & knights) {
       return true;
   }
   Bitboard king = getPieces(nKing, side);
   if (kingAttacks[square] & king) {
       return true;
   }
   Bitboard bishopsQueens = getPieces(nQueen, side) | getPieces(nBishop, side);
   if (slidingAttacksBB<nBishop>(square, occupied) & bishopsQueens) {
       return true;
   }
   Bitboard rooksQueens = getPieces(nQueen, side) | getPieces(nRook, side);
   if (slidingAttacksBB<nRook>(square, occupied) & rooksQueens) {
       return true;
   }
}
