#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "board.hpp"
#include "move.hpp"
#include "bitboard.hpp"
#include <vector>

using namespace std;

template<Color c>
void getPawnMoves(vector<Move> &moveList, Board &b) {
    Bitboard pawns = b.getPieces(nPawn, c);
    Bitboard empty = b.getEmpty();
    Bitboard other = (c == nWhite ? b.getPieces(nBlack) : b.getPieces(nWhite));
    Bitboard fourthRank = (c == nWhite ? Rank4 : Rank5);
    Square enPassant = b.enPassantTarget();

    constexpr Direction up = (c == nWhite ? NORTH : SOUTH);
    constexpr Direction upRight = (c == nWhite ? NORTH_EAST : SOUTH_WEST);
    constexpr Direction upLeft = (c == nWhite ? NORTH_WEST : SOUTH_EAST);

    Bitboard singleMoves = shift<up>(pawns) & empty;
    Bitboard doubleMoves = (shift<up>(singleMoves) & fourthRank) & empty;
    Bitboard attacksLeft = shift<upLeft>(singleMoves) & other;
    Bitboard attacksRight = shift<upLeft>(singleMoves) & other;

    // single pawn moves
    while (singleMoves != 0) {
        int index = bitScanForward(singleMoves);
        singleMoves &= singleMoves - 1;
        if (index >= A8 || index <= H1) { // promotion
            for (int flag = 12; flag <= 15; flag++) {
                moveList.push_back(Move(index - up, index, flag));
            }
        } else {
            moveList.push_back(Move(index - up, index, 0));
        }
    }
    while (doubleMoves != 0) {
        int index = bitScanForward(doubleMoves);
        doubleMoves &= doubleMoves - 1;
        moveList.push_back(Move(index - 2 * up, index, 1));
    }

    while (attacksLeft != 0) {
        int index = bitScanForward(attacksLeft);
        attacksLeft &= attacksLeft - 1;
        moveList.push_back(Move(index - upLeft, index, 4));
    }
    while (attacksRight != 0) {
        int index = bitScanForward(attacksRight);
        attacksRight &= attacksRight - 1;
        moveList.push_back(Move(index - upRight, index, 4));
    }

    if (enPassant != SQ_NONE) {
        if (sqToBB[enPassant - upLeft] & pawns) {
            moveList.push_back(Move(enPassant - upLeft, enPassant, 5)); 
        }
        if (sqToBB[enPassant - upRight] & pawns) {
            moveList.push_back(Move(enPassant - upRight, enPassant, 5)); 
        }
    }
}

template<Color c, Piece p>
void getSlidingMoves(vector<Move> &moveList, Board &b) {
    Bitboard pieces = b.getPieces(p, c);
    Bitboard occupied = b.getOccupied();
    Bitboard other = (c == nWhite ? b.getPieces(nBlack) : b.getPieces(nWhite));

    while (pieces > 0) {
        int square = bitScanForward(pieces);
        pieces &= pieces - 1;
        // automatically finds proper valid moves
        Bitboard attacks = slidingAttacksBB<p>(square, occupied);
        while (attacks > 0) {
            int attackSquare = bitScanForward(attacks);
            attacks &= attacks - 1;
            if (sqToBB[attackSquare] & other) { // other piece there
                moveList.push_back(Move(square, attackSquare, 4)); 
            } else if (sqToBB[attackSquare] & ~occupied) { // not a capture
                moveList.push_back(Move(square, attackSquare, 0)); 
            }
        }
    }
}

template<Color c, Piece p>
void getMoves(vector<Move> &moveList, Board &b) {
    Bitboard pieces = b.getPieces(p, c); 
    Bitboard occupied = b.getOccupied();
    Bitboard other = (c == nWhite ? b.getPieces(nBlack) : b.getPieces(nWhite));
    
    while (pieces != 0) {
        int square = bitScanForward(pieces);
        pieces &= pieces - 1;

        Bitboard attacks;
        if (p == nKnight) {
            attacks = knightAttacks[square];
        } else { // king
            attacks = kingAttacks[square];
        }

        while (attacks > 0) {
            int attackSquare = bitScanForward(attacks);
            attacks &= attacks - 1;
            if (sqToBB[attackSquare] & other) { // other piece there
                moveList.push_back(Move(square, attackSquare, 4)); 
            } else if (sqToBB[attackSquare] & ~occupied) { // not a capture
                moveList.push_back(Move(square, attackSquare, 0)); 
            }
        }
    }
}

template<Color c>
void getCastleMoves(vector<Move> &moveList, Board &b, bool
        kingSide) {
    short castling = b.getCastlingRights();      
    Square start = (c == nWhite) ? E1 : E8;
    Square end = kingSide ? ((c == nWhite) ? G1 : G8) : ((c == nWhite) ? C1
            : C8); 
    Color other = (c == nWhite) ? nBlack : nWhite;
    if ((castling >> (2 * other + kingSide)) & 1) {
        for (int i = start; i <= end; i++) {
            if (b.attacked((Square)i, other)) {
                return;
            }
        }
        if (!(betweenBB[start][end] & b.getOccupied())) {
            moveList.push_back(Move(start, end, 2 + !kingSide));
        }
    }
}

template<Color c>
inline void getPseudoLegalMoves(vector<Move> &moveList, Board& b) {
    getPawnMoves<c>(moveList, b);
    getSlidingMoves<c, nBishop>(moveList, b);
    getSlidingMoves<c, nRook>(moveList, b);
    getSlidingMoves<c, nQueen>(moveList, b);
    getMoves<c, nKnight>(moveList, b);
    getMoves<c, nKing>(moveList, b);
    getCastleMoves<c>(moveList, b, true);
    getCastleMoves<c>(moveList, b, false);
}

#endif
