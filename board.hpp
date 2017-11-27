#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <stdint.h>
#include "BB.hpp"
#include "move.hpp"

using namespace types;
typedef uint64_t U64;

class board {
        bool kingMoved[2];
        bool rookMoved[4];
        bool whiteMove;
        U64 pieceBB[8];
        U64 lookup[64];
    public:
        board();
        
        // Returns a bitboard representing all the pieces on the board.
        U64 getPieces();

        // Takes a color. Returns a bitboard of all the pieces of that color.
        U64 getPieces(color c);

        // Takes a piece type. Returns a bitboard of all the pieces of that type.
        U64 getPieces(piece p);

        // Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
        U64 getPieces(color c, piece p);
        
        void invert();

        U64 flip180(U64 x);

        bool sameRank(unsigned int square1, unsigned int square2);

        bool sameFile(unsigned int square1, unsigned int square2);

        bool sameDiagonal(unsigned int square1, unsigned int square2);

        // Checks to see if there are any pieces in between the two squares. 
        bool inBetween(unsigned int square1, unsigned int square2);

        bool legalMove(move m);

        bool inCheck();

        color getColor(unsigned int square);

        piece getPiece(unsigned int square);

        void move(class move m);
};
#endif /* BOARD_H */
