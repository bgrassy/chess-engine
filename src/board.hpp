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

const int SEARCH_DEPTH = 7;

const short pieceTable[6][64] = {
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

enum HashType {
    HASH_EXACT,
    HASH_ALPHA,
    HASH_BETA,
    HASH_NULL
};

struct HashEntry {
    unsigned long long zobrist;
    int depth;
    int score;
    bool ancient;
    HashType nodeType;
    Move move;

    HashEntry(unsigned long long zobrist, int depth, int score, bool ancient,
            HashType nodeType, Move move) {
        this->zobrist = zobrist;
        this->depth = depth;
        this->score = score;
        this->ancient = ancient;
        this->nodeType = nodeType;
        this->move = move;
    }

    HashEntry() {
        this->zobrist = 0;
        this->depth = 0;
        this->score = 0;
        this->ancient=true;
        this->nodeType=HASH_NULL;
        this->move = Move();
    }
};

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
    // holds the zobrist keys
    std::stack<unsigned long long> zobrist;
    // holds the transposition table
    HashEntry transTable[100000];
public:
    // Holds the killer moves list
    Move killerMoves[SEARCH_DEPTH + 1][2];
    // holds the list of moves
    std::stack<Move> moveList;
    /**
     * Constructs a new Board object to the starting chess position.
     */
    Board();

    void setZobrist();

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

    // Returns zobrist hash key
    unsigned long long getZobrist() const;

    // Returns target of en passant
    Square enPassantTarget() const;
    
    // Returns the color of the side to move
    Color getToMove() const;

    // Returns whether the player to move is in check or not
    bool inCheck() const;

    // Returns whether the player is in double check our not
    bool doubleCheck() const;

    Bitboard getCheckers() const;

    Bitboard pinnedPieces(Bitboard pinners, Square sq) const;

    bool attacked(int square, Color side) const;

    Square lva(Square sq, Color side) const;

    int see(Square sq, Color side);

    short getCastlingRights() const;

    void makeMove(Move m);

    void unmakeMove();

    bool isLegal(Move m) const;

    Piece getPiece(int sq) const;

    Color getColor(int sq) const;

    void printBoard() const;

    int boardScore() const;

    HashEntry getTransTable(int key) const;

    void setTransTable(int key, HashEntry entry);
};

#endif // #ifndef BOARD

