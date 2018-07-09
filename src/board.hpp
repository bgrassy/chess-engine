#ifndef BOARD
#define BOARD

#include <iostream>
#include <vector>
#include <stack>
#include <sstream>
#include <cassert>
#include <algorithm>
#include "bitboard.hpp"
#include "move.hpp"

class Board {
    // Holds bitboards for the different colors and types of pieces
    Bitboard pieceBB[8];    
    // Bitboard that is 1 for all the empty squares
    Bitboard emptyBB;
    // Bitboard that is 1 for all the occupied squares
    Bitboard occupiedBB;
    // En passant target square
    std::stack<Square> enPassant;
    // Holds the castling rights:
    // 0b1011 means: 
    // white CAN castle kingside
    // white CAN'T castle queenside
    // black CAN castle kingside
    // black CAN castle queenside
    std::stack<short> castling;
    // Holds the color of the side to move
    Color toMove;
    // Holds the fifty move counter
    std::stack<int> fiftyList;
    // holds the list of captured pieces
    std::stack<Piece> capturedList;
    // holds the full move counter
    int fullMove;
public:
    // holds the list of moves
    std::stack<Move> moveList;
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

    std::string getFEN() const;

    // Returns pieces of the given piece type
    Bitboard getPieces(Piece pt) const;

    // Returns pieces of the given color
    Bitboard getPieces(Color ct) const;

    // Returns pieces of the given piece and color
    Bitboard getPieces(Piece pt, Color ct) const;

    // Returns bitboard of all the white pawns
    Bitboard getWhitePawns() const;

    // Returns bitboard of all the white knights
    Bitboard getWhiteKnights() const;

    // Returns bitboard of all the white bishops
    Bitboard getWhiteBishops() const;

    // Returns bitboard of all the white rooks
    Bitboard getWhiteRooks() const;

    // Returns bitboard of all the white queens
    Bitboard getWhiteQueens() const;

    // Returns bitboard of the white king
    Bitboard getWhiteKing() const;

    // Returns bitboard of all the black pawns
    Bitboard getBlackPawns() const;

    // Returns bitboard of all the black knights
    Bitboard getBlackKnights() const;

    // Returns bitboard of all the black bishops
    Bitboard getBlackBishops() const;

    // Returns bitboard of all the black rooks
    Bitboard getBlackRooks() const;

    // Returns bitboard of all the black queens
    Bitboard getBlackQueens() const;

    // Returns bitboard of the black king
    Bitboard getBlackKing() const;

    // Returns occupied bitboard
    Bitboard getOccupied() const;

    // Returns bitboard of empty pieces
    Bitboard getEmpty() const;

    // Returns target of en passant
    Square enPassantTarget() const;
    
    Color getToMove() const;

    bool inCheck() const;

    bool doubleCheck() const;

    Bitboard getCheckers() const;

    Bitboard pinnedPieces(Bitboard pinners, Square sq) const;

    bool attacked(int square, Color side) const;

    short getCastlingRights() const;

    void makeMove(Move m);

    void unmakeMove();

    bool isLegal(Move m) const;

    Piece getPiece(int sq) const;

    Color getColor(int sq) const;

    void printBoard() const;
};

#endif // #ifndef BOARD
