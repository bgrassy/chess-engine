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
    std::vector<piece> captureList;
    std::vector<int> castleList;
    std::vector<int> enPassantList;
    U64 lookup[64];
    U64 hashVal;
    U64 hashTable[64][12];
    U64 specialHashTable[13];
    std::vector<class move> legalMoves;
    std::vector<U64> hashList;
    public:
    U64 pieceBB[8];
    std::vector<move> moveList;
    short pieceTable[6][64] = {
		// pawn
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        },
		// knight
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-20,-30,-30,-20,-40,-50
        },
		// bishop
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-40,-10,-10,-40,-10,-20
        },
		// rook
        {
            0,  0,  0,  0,  0,  0,  0,  0,
			5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0,-5,
            -5,  0,  0,  0,  0,  0,  0,-5,
            -5,  0,  0,  0,  0,  0,  0,-5,
            -5,  0,  0,  0,  0,  0,  0,-5,
            -5,  0,  0,  0,  0,  0,  0,-5,
            0,  0,  0,  5,  5,  0,  0,  0
        },
		// queen
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -5,  0,  5,  5,  5,  5,  0, -5,
            0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        },
        {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            20, 30, 10,  0,  0, 10, 30, 20
        }
    };
    board();

    U64 initHash();
    // Returns a bitboard representing all the pieces on the board

    U64 flip180(U64 x) const;
    bool getMove() const;

    U64 getPieces() const;

    // Takes a color. Returns a bitboard of all the pieces of that color.
    U64 getPieces(color c) const;

    // Takes a piece type. Returns a bitboard of all the pieces of that type.
    U64 getPieces(piece p) const;

    // Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
    U64 getPieces(color c, piece p) const;

    bool sameRank(unsigned int square1, unsigned int square2) const;

    bool sameFile(unsigned int square1, unsigned int square2) const;

    bool sameDiagonal(unsigned int square1, unsigned int square2) const;

    // Checks to see if there are any pieces in between the two squares. 
    bool inBetween(unsigned int square1, unsigned int square2) const;

    bool legalMove(move m) const;

    bool attacked(unsigned int square, color c) const;

    color getColor(unsigned int square) const;

    piece getPiece(unsigned int square) const;

    bool inCheck() const;

    bool makeMove(move m);

    void unmakeMove();

    std::vector<move> getLegalMoves();

    double boardScore() const;

};
#endif /* BOARD_HPP */
