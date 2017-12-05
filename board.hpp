#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <cmath>
#include "BB.hpp"
#include "move.hpp"

using namespace types;

class board {
        int castle = 0b1111;
        int enPassant = 0b00000000;
        // en passant possible on each file
        bool whiteMove = true;
        std::vector<move> moveList;
        std::vector<piece> captureList;
        std::vector<int> castleList;
        std::vector<int> enPassantList;
        U64 pieceBB[8];
        U64 lookup[64];
        U64 hashVal;
        U64 hashTable[64][12];
        U64 specialHashTable[13];
        std::vector<class move> legalMoves;
        std::vector<U64> hashList;
    public:
        board();

        U64 initHash();
        // Returns a bitboard representing all the pieces on the board
        
        bool getMove();

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

        bool attacked(unsigned int square, color c);

        color getColor(unsigned int square);

        piece getPiece(unsigned int square);

        bool inCheck();

        bool makeMove(move m);

        void unmakeMove();

        std::vector<move> getLegalMoves();

        double boardScore();

};
#endif /* BOARD_HPP */
