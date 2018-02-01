#ifndef BOARD
#define BOARD

#include "types.hpp"
#include "bitboard.hpp"
class CBoard {
    U64 pieceBB[14]; 
    U64 lookup[64];
    enumPiece pieces[64];
    U64 occupiedBB;
    U64 emptyBB;
    bool whiteMove;
    short castling;
    short enPassant; // en passant square
    public:
        CBoard();
        
        U64 getPieceType(enumPiece p) const;

        U64 getOccupied() const;
        
        U64 getEmpty() const;

        U64 getWhite() const;

        U64 getBlack() const;

        U64 getWhitePawns() const;

        U64 getWhiteKnights() const;

        U64 getWhiteBishops() const;

        U64 getWhiteRooks() const;

        U64 getWhiteQueens() const;

        U64 getWhiteKings() const;

        U64 getBlackPawns() const;

        U64 getBlackKnights() const;

        U64 getBlackBishops() const;

        U64 getBlackRooks() const;

        U64 getBlackQueens() const;

        U64 getBlackKings() const;

        bool attacked(U64 occupied, unsigned int square, enumColor c);
};

#endif /* ifndef BOARD */
