#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <bitset>
#include "BB.hpp"
#include "move.hpp"

using namespace Types;

class HashEntry {
    public:
        U64 zobrist;
        Move bestMove;
        int score;
        int depth;
        short type; // 0 is exact, 1 is fail-high, 2 is fail-low
        bool ancient;
        HashEntry(U64 zobrist, Move bestMove, int score, int depth, short type, bool ancient) {
            this->zobrist = zobrist;
            this->bestMove = bestMove;
            this->score = score;
            this->depth = depth;
            this->type = type;
            this->ancient = ancient;
        }
        HashEntry() {
            this->zobrist = 0;
            this->bestMove = Move();
            this->score = 0;
            this->depth = 0;
            this->type = -1;
            this->ancient = true;
        }
};

extern const short pieceTable[6][64];
extern HashEntry transTableDepth[10000];
extern HashEntry transTableAlways[10000];

class Board {
    int castle = 0b1111;
    int enPassant = 0b00000000;
    // en passant possible on each file
    bool whiteMove = true;
    std::vector<Piece> captureList;
    std::vector<int> castleList;
    std::vector<int> enPassantList;
    U64 pieceBB[8];
    U64 hashVal;
    U64 hashTable[64][12];
    U64 specialHashTable[13];
    std::vector<class Move> legalMoves;
    std::vector<U64> hashList;
    public:
    U64 lookup[64];
    int zeroLookup[67];
    // our transposition table
    std::vector<Move> moveList;
    Board();

    void initHash();

    U64 getHashVal() const;

    int reverseLookup(U64 x) const;

    int trailZeros(U64 x) const;

    void colorFlip();

    int popcnt(U64 x) const;
    
    std::vector<int> getIndices(U64 x) const;

    bool getMove() const;

    // Returns a bitboard representing all the pieces on the board
    U64 getPieces() const;

    // Takes a color. Returns a bitboard of all the pieces of that color.
    U64 getPieces(Color c) const;

    // Takes a piece type. Returns a bitboard of all the pieces of that type.
    U64 getPieces(Piece p) const;

    // Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
    U64 getPieces(Color c, Piece p) const;

    Piece getLastCapture() const;

    bool sameRank(int square1, int square2) const;

    bool sameFile(int square1, int square2) const;

    bool sameDiagonal(int square1, int square2) const;

    // Checks to see if there are any pieces in between the two squares. 
    bool inBetween(int square1, int square2) const;

    int isoPawns(Color c) const;

    int materialCount(Color c) const;

    int doublePawns(Color c) const;

    int opposed(int pawn, Color c) const;

    int blockedPawns(Color c) const;

    int mobility(Color c) const;

    bool legalMove(Move m) const;

    bool attacked(int square, Color c) const;

    Color getColor(int square) const;

    Piece getPiece(int square) const;

    bool inCheck() const;

    bool makeMove(Move &m);

    void unmakeMove();

    bool inCheckmate();

    std::vector<Move> getLegalMoves() const;

    std::vector<Move> getNormalMoves() const;

    std::vector<Move> getCheckMoves() const;

    std::vector<Move> getDoubleCheckMoves() const;

    int boardScore();

    bool compareTo(const Move &a, const Move &b);

    void insertTransTable(int index, HashEntry h);

    U64 updatedHashVal(Move &m) const; 

    void flushTransTable();

    int checkCount() const;
};
#endif /* BOARD_HPP */
