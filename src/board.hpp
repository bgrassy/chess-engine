#ifndef BOARD
#define BOARD

#include <iostream>
#include "bitboard.hpp"

class Board {
    // Holds bitboards for the different colors and types of pieces
    Bitboard pieceBB[8];    
    // Bitboard that is 1 for all the empty squares
    Bitboard emptyBB;
    // Bitboard that is 1 for all the occupied squares
    Bitboard occupiedBB;
    // En passant target square
    enumSquare enPassant;
    // Holds the castling rights
    bool castling[4];
    // square to bitboard
public:
    /**
     * Constructs a new Board object to the starting chess position.
     */
    Board();

    /**
     * Constructs a new Board object to the given position
     *
     * @param FEN the desired starting position in FEN form
     */
    Board(std::string FEN);

    // Returns pieces of the given piece type
    Bitboard getPieces(Piece pt);

    // Returns pieces of the given color
    Bitboard getPieces(Color ct);

    // Returns pieces of the given piece and color
    Bitboard getPieces(Piece pt, Color ct);

    // Returns bitboard of all the white pawns
    Bitboard getWhitePawns();

    // Returns bitboard of all the white knights
    Bitboard getWhiteKnights();

    // Returns bitboard of all the white bishops
    Bitboard getWhiteBishops();

    // Returns bitboard of all the white rooks
    Bitboard getWhiteRooks();

    // Returns bitboard of all the white queens
    Bitboard getWhiteQueens();

    // Returns bitboard of the white king
    Bitboard getWhiteKing();

    // Returns bitboard of all the black pawns
    Bitboard getBlackPawns();

    // Returns bitboard of all the black knights
    Bitboard getBlackKnights();

    // Returns bitboard of all the black bishops
    Bitboard getBlackBishops();

    // Returns bitboard of all the black rooks
    Bitboard getBlackRooks();

    // Returns bitboard of all the black queens
    Bitboard getBlackQueens();

    // Returns bitboard of the black king
    Bitboard getBlackKing();

    // Returns occupied bitboard
    Bitboard getOccupied();

    // Returns bitboard of empty pieces
    Bitboard getEmpty();

    // Returns target of en passant
    enumSquare enPassantTarget();

    bool attacked(Bitboard occupied, enumSquare square, Color side);
};

#endif // #ifndef BOARD
