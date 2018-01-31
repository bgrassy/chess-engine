#include "board.hpp"

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

HashEntry transTableDepth[10000];
HashEntry transTableAlways[10000];

Board::Board() {
    pieceBB[0] = 0x000000000000FFFF; // white pieces
    pieceBB[1] = 0xFFFF000000000000; // black pieces
    pieceBB[2] = 0x00FF00000000FF00; // pawns
    pieceBB[3] = 0x4200000000000042; // knights
    pieceBB[4] = 0x2400000000000024; // bishops
    pieceBB[5] = 0x8100000000000081; // rooks
    pieceBB[6] = 0x0800000000000008; // queens
    pieceBB[7] = 0x1000000000000010; // kings
    std::fill_n(zeroLookup, 67, -1);
    for (int i = 0; i < 64; i++) {
        lookup[i] = (U64)1 << i;
        zeroLookup[lookup[i] % 67] = i;        
    }
    moveList.push_back(Move(0, 0, 0));
    castleList.push_back(castle);
    enPassantList.push_back(enPassant);
    initHash();
    hashVal = 0;
    for (int i = 0; i < 64; i++) {
        int c = (int) getColor(i); 
        int p = (int) getPiece(i); 
        if (c != 2) { // if color is not none
            hashVal ^= hashTable[i][6 * c + p];
        }
    }
    for (int i = 0; i < 5; i++) {
        hashVal ^= specialHashTable[i]; // whiteMove, castle;
    }
    hashList.push_back(hashVal);
}

void Board::initHash() {
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<long long int> dist(std::llround(std::pow(2,61)), std::llround(std::pow(2,62)));


    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            hashTable[i][j] = dist(e2);
        }
    }

    for (int i = 0; i < 13; i++) {
        // 0: whiteMove or no
        // 1-4: castle k/q, w/b
        // 5-12: File of en passant
        specialHashTable[i] = dist(e2);
    }
}

// assumes the number is a power of 2
int Board::reverseLookup(U64 x) const {
    return zeroLookup[x % 67];
}

int Board::trailZeros(U64 x) const {
    return zeroLookup[(x & (~x + 1)) % 67];
}

int Board::popcnt(U64 x) const {
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; x; c++)
    {
          x &= x - 1; // clear the least significant bit set
    }
    return c;
}

std::vector<int> Board::getIndices(U64 x) const {
    std::vector<int> indices;
    while (x != 0) {
        int index = trailZeros(x);
        indices.push_back(index);
        x ^= lookup[index];
    }
    return indices;
}

void Board::colorFlip() {
    whiteMove = !whiteMove;
}

U64 Board::getHashVal() const {
    return hashVal;
} 

bool Board::getMove() const {
    return whiteMove;
}
// Returns a bitboard representing all the pieces on the board.
U64 Board::getPieces() const {
    return pieceBB[0] | pieceBB[1];
}

// Takes a color. Returns a bitboard of all the pieces of that color.
U64 Board::getPieces(Color c) const {
    return pieceBB[(int)c];
}

// Takes a piece type. Returns a bitboard of all the pieces of that type.
U64 Board::getPieces(Piece p) const {
    return pieceBB[(int)p + 2];
}

// Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
U64 Board::getPieces(Color c, Piece p) const {
    return pieceBB[(int)c] & pieceBB[(int)p + 2];
}

Piece Board::getLastCapture() const {
    return captureList.back();
}

// Takes two integers corresponding to squares, and returns whether they are in the same rank.
bool Board::sameRank(int square1, int square2) const{
    return (square1 >> 3) == (square2 >> 3);
}

// Takes two integers corresponding to squares, and returns whether they are in the same file.
bool Board::sameFile(int square1, int square2) const {
    return (square1 & 7) == (square2 & 7);
}

// Takes two integers corresponding to squares, and returns whether they are in the same diagonal.
bool Board::sameDiagonal(int square1, int square2) const {
    return bishop_attacks(lookup[square1], 0) & bishop_attacks(lookup[square2], 0);
}
// Checks to see if there are any pieces between two given squares. 
bool Board::inBetween(int square1, int square2) const {
    if (square1 > square2) {
        int tmp = square1;
        square1 = square2;
        square2 = tmp;
    }
    if (sameRank(square1, square2)) { // in the same row
        for (int i = square1 + 1; i < square2; ++i) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameFile(square1, square2)) { // in the same column
        for (int i = square1 + 8; i < square2; i+=8) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameDiagonal(square1, square2)) {
        if ((square2 - square1) % 9 == 0) {
            for (int i = square1 + 9; i < square2; i+=9) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }
        } else if ((square2 - square1) % 7 == 0) {
            for (int i = square1 + 7; i < square2; i+=7) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }

        }
    }
    return false;
} 

int Board::materialCount(Color c) const {
    int eval = 0;
    int pieceValues[] = {100, 320, 330, 500, 900, 20000};
    for (int p = 0; p < 6; p++) {
        U64 pieces = getPieces((Color) c, (Piece) p);
        for (int count : getIndices(pieces)) {
            eval += pieceValues[p];
            eval += pieceTable[p][count];
            eval += pieceTable[p][(7 - count / 8) * 8 + count % 8];
        }
    }
    return eval;
}

int Board::doublePawns(Color c) const {
    int count = 0;
    U64 pawns = getPieces(c, Piece::Pawn);
    for (int i = 0; i < 8; i++) {
        U64 filePawns = (1 << i) * 72340172838076673; 
        int pawnCount = popcnt(filePawns & pawns);
        if (pawnCount >= 2) {
            count += pawnCount;
        }
    }
    return -count * 25;
}

int Board::isoPawns(Color c) const {
    int count = 0;
    U64 pawns = getPieces(c, Piece::Pawn);
    int arr[8] = {0};
    for (int i = 0; i < 8; i++) {
        U64 filePawns = i * 72340172838076673; 
        for (int square : getIndices(filePawns & pawns)) {
            if (opposed(square, c)) {
                arr[i] += 7;
            } else {
                arr[i] += 17;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            if (arr[1] == 0) {
                count += arr[i];
            }
        } else if (i == 7) {
            if (arr[6] == 0) {
                count += arr[i];
            }
        } else {
            if (arr[i - 1] == 0 && arr[i + 1] == 0) {
                count += arr[i];
            }
        }
    }
    return count;
}

int Board::opposed(int pawn, Color c) const {
    if (c == Color::Black) {
        for (int square = pawn; square >= 7; square-=8) {
            if (getPiece(square) == Piece::Pawn) {
                if (getColor(square) == Color::White) {
                    return 1;
                }
            }
        }
    } else {
        for (int square = pawn; square <=55; square+=8) {
            if (getPiece(square) == Piece::Pawn) {
                if (getColor(square) == Color::Black) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int Board::blockedPawns(Color c) const {
    int count = 0;
    U64 pawns = getPieces(c, Piece::Pawn);
    for (int square : getIndices(pawns)) {
        if (single_pawn_moves(lookup[square], c) & ~getPieces()) {
            break; 
        } else if (double_pawn_moves(lookup[square], c) & ~getPieces()) {
            break; 
        } else if (pawn_attacks(lookup[square], c) & getPieces() & ~getPieces(c)) {
            break; 
        }
        count++;
    }
    return -15 * count;
}

int Board::mobility(Color c) const {
    int eval = 0;
    Color opp = (Color) (((int) c + 1) % 2);
    for (int square : getIndices(getPieces(c))) {
        if ((getPiece(square) == Piece::Pawn)) {
            eval += popcnt(pawn_attacks(lookup[square], c) & getPieces(opp)); 
            eval += popcnt(single_pawn_moves(lookup[square], c) & ~getPieces(c)); 
            eval += popcnt(double_pawn_moves(lookup[square], c) & ~getPieces(c)); 
        } else if ((getPiece(square) == Piece::Knight)) {
            eval += popcnt(knight_attacks(lookup[square]) & ~getPieces(c)); 
        } else if ((getPiece(square) == Piece::Bishop)) {
            eval += popcnt(bishop_attacks(lookup[square], getPieces(c)) & ~getPieces(c));
        } else if ((getPiece(square) == Piece::Rook)) {
            eval += popcnt(rook_attacks(lookup[square], getPieces(c)) & ~getPieces(c)); 
        } else if ((getPiece(square) == Piece::Queen)) {
            eval += popcnt(queen_attacks(lookup[square], getPieces()) & ~getPieces(c)); 
        } else if ((getPiece(square) == Piece::King)) {
            eval += popcnt(king_attacks(lookup[square]) & ~getPieces(c)); 
        }
    }
    return eval;
}

// Takes a move, and tests if it is a "pseudo-legal" move.
bool Board::legalMove(class Move m) const {
    int start = m.getStart();
    int end = m.getEnd();
    int flags = m.getFlags();
    bool capture = m.getCapture();
    bool prom = m.getProm();
    int type = m.getType();
    Piece p = getPiece(start);
    Color c = getColor(start);
    Color opp = (Color) (((int)c+1)%2);
    U64 startBB = lookup[start];
    U64 endBB = lookup[end];
    Move lastMove = moveList.back();
    if (getPiece(start) == Piece::None) {
        return false;
    } 
    if (whiteMove != (c == Color::White)) {
        return false;
    }
    if (capture && (getColor(end) != opp) && (p != Piece::Pawn)) {
        return false;
    }

    if (!capture && (getPiece(end) != Piece::None)) {
        return false;
    }

    if (prom) {
        if (p != Piece::Pawn) {
            return false;
        }
        if (c == Color::White && !(startBB & Rank7)) {
            return false; 
        }
        if (c == Color::Black && !(startBB & Rank2)) {
            return false;
        }
    }

    // checks if the end piece is the same color
    if (c == getColor(end)) {
        return false;
    }
    if (inBetween(start, end)) {
        return false;
    }
    // tests if the start and end square are the same
    if (start == end) {
        return false;
    }

    if (flags == 1 && p != Piece::Pawn) {
        return false;
    }

    if (!prom && !capture && ((type >> 1) == 1)) { // castling
        if (c == Color::White) {
            if (p != Piece::King) {
                return false;
            }
            if (end == 2) {
                if (getPiece(1) == Piece::None) {
                    return (castle & 2) && ((type & 1) == 1);            
                }
                return false;
            } else if (end == 6) {
                return (castle & 1) && ((type & 1) == 0);
            }
        } else {
            if (p != Piece::King) {
                return false;
            }
            if (end == 58) {
                if (getPiece(57) == Piece::None) { 
                    return (castle & 8) && ((type & 1) == 1); // queenside      
                }
                return false;
            } else if (end == 62) {
                return (castle & 4) && ((type & 1) == 0);
            }
        }
    }
    
    if (p == Piece::Pawn) {
        if (capture) {
            if (type == 1) { // en passant
                if (lastMove.getFlags() == 1) {
                    if (c == Color::White) {
                        return (lastMove.getEnd() == end - 8) && (pawn_attacks(startBB, c) & endBB);
                    } else {
                        return (lastMove.getEnd() == end + 8) && (pawn_attacks(startBB, c) & endBB);
                    }
                } else {
                    return false;
                }
            }    

            if (capture && (getColor(end) != opp)) {
                return false;
            }
            return pawn_attacks(startBB, c) & endBB; 
        } else {
            if (type == 0) {
                return single_pawn_moves(startBB, c) & endBB;
            } else if (type == 1) {
                return double_pawn_moves(startBB, c) & endBB;
            }
        }
    } else if (p == Piece::Knight) {
        return knight_attacks(startBB) & endBB;
    } else if (p == Piece::Bishop) {
        return bishop_attacks(startBB, getPieces()) & endBB;
    } else if (p == Piece::Rook) {
        return rook_attacks(startBB, getPieces()) & endBB;
    } else if (p == Piece::Queen) {
        return queen_attacks(startBB, getPieces()) & endBB;
    } else if (p == Piece::King) {
        return king_attacks(startBB) & endBB;
    }
    return false;
}

// Takes a square and color and determines whether that player is attacking that square. 
bool Board::attacked(int square, Color c) const {
    U64 sq = lookup[square]; 
    if (sq & (pawn_attacks(getPieces(c, Piece::Pawn), c) | 
                knight_attacks(getPieces(c, Piece::Knight)))) {
        return true;
    } 
    if (sq & bishop_attacks(getPieces(c, Piece::Bishop), getPieces())) {
        return true;
    } 
    if (sq & rook_attacks(getPieces(c, Piece::Rook), getPieces())) {
        return true;
    } 
    if (sq & queen_attacks(getPieces(c, Piece::Queen), getPieces())) {
        return true;
    }
    if (sq & king_attacks(getPieces(c, Piece::King))) {
        return true;
    }
    return false;
}

// Takes a square, and returns the color of the piece on the square
Color Board::getColor(int square) const {
    U64 b = lookup[square];
    if (b & getPieces(Color::White)) {
        return Color::White;
    } else if (b & getPieces(Color::Black)) {
        return Color::Black;
    }
    return Color::None;
}

// Takes a square, and returns the type of the piece on the square.
Piece Board::getPiece(int square) const {
    U64 b = lookup[square];
    for (int i = 0; i < 6; i++) {
        if (b & getPieces((Piece) i)) {
            return (Piece) i;
        }
    }
    return Piece::None;
}

// Returns whether the current player is in check or not.
bool Board::inCheck() const {
    Color c, opp;    
    if (whiteMove) {
        c = Color::White;
        opp = Color::Black;
    } else {
        c = Color::Black;
        opp = Color::White;
    }
    U64 kingLoc = getPieces(c, Piece::King);
    int kingSquare = reverseLookup(kingLoc);
    return attacked(kingSquare, opp);
}

// Takes a move, and executes the move on the board if it is legal.
bool Board::makeMove(Move& m) {
    if (!legalMove(m)) { return false;}
    // Get all the information from the move.
    int start = m.getStart();
    int end = m.getEnd();
    int flags = m.getFlags();
    bool capture = m.getCapture();
    bool prom = m.getProm();
    int type = m.getType();
    // Bitboards corresponding to start and end square.
    U64 startBB = lookup[start];
    U64 endBB = lookup[end];
    U64 startEndBB = startBB ^ endBB;
    // Get the start and end pieces.
    Piece startP = getPiece(start);
    Color startC = getColor(start);
    Piece endP = getPiece(end);
    Color endC = getColor(end);
    Color opp = (Color) (((int) startC + 1) % 2);
    bool checked = inCheck();
    // Determines the square of the king
    U64 kingLoc = getPieces(startC, Piece::King);
    int kingSquare = reverseLookup(kingLoc);

    U64 tmpHashVal = updatedHashVal(m); 
    pieceBB[(int)startP + 2] ^= startEndBB; 
    pieceBB[(int)startC] ^= startEndBB;

    enPassant = 0;
    if (type == 1) {
        int file = start % 8;
        if (file == 0) {
            enPassant = 0b00000010;
        } else if (file == 7) {
            enPassant = 0b01000000;
        } else {
            enPassant ^= lookup[file - 1] ^ lookup[file + 1];
        }
    }

    if (flags == 5) { // en passant
        endP = Piece::Pawn;
        if (startC == Color::White) {
            pieceBB[2] ^= lookup[end - 8];    
            pieceBB[1] ^= lookup[end - 8];
        } else {
            pieceBB[2] ^= lookup[end + 8];    
            pieceBB[0] ^= lookup[end + 8];
        }
    } else if (capture) {
        pieceBB[(int) endP + 2] ^= endBB;
        pieceBB[(int) endC] ^= endBB;
    }

    if (prom) {
        int promPiece = 1 + (type & 3);
        pieceBB[promPiece + 2] ^= endBB;
        pieceBB[2] ^= endBB;
    } 
    
    if (((type >> 1) & 1) == 1) { // castling
        if (!checked) {
            U64 rookBB;
            if ((type & 1) == 0) { // kingside
                if (!attacked(kingSquare + 1, opp) && !attacked(kingSquare + 2, opp)) { // not castle through check
                    if (startC == Color::White) {
                        rookBB = lookup[5] ^ lookup[7];
                        castle &= 0b1100;
                    } else { 
                        rookBB = lookup[61] ^ lookup[63];
                        castle &= 0b0011;
                    }
                    pieceBB[5] ^= rookBB;
                    pieceBB[(int) startC] ^= rookBB;
                } else {
                    pieceBB[(int)startP + 2] ^= startEndBB; 
                    pieceBB[(int)startC] ^= startEndBB;
                    return false;
                }
            } else { // queenside
                if (!attacked(kingSquare - 1, opp) && !attacked(kingSquare - 2, opp)) { // not castle through check
                    if (startC == Color::White) {
                        rookBB = lookup[0] ^ lookup[3];
                        castle &= 0b1100;
                    } else {
                        rookBB = lookup[56] ^ lookup[59];
                        castle &= 0b0011;
                    }
                    pieceBB[5] ^= rookBB;
                    pieceBB[(int) startC] ^= rookBB;
                } else {
                    pieceBB[(int)startP + 2] ^= startEndBB; 
                    pieceBB[(int)startC] ^= startEndBB;
                    return false;
                }
            }
        } else { // return to original state, return false
            pieceBB[(int)startP + 2] ^= startEndBB; 
            pieceBB[(int)startC] ^= startEndBB;
            return false;
        } 
    } 
    
    if (startP == Piece::Rook) {
        if (start == 0) {
            castle &= 0b1101;
        } else if (start == 7) {
            castle &= 0b1110;
        } else if (start == 56) {
            castle &= 0b0111;
        } else if (start == 63) {
            castle &= 0b1011;
        }
    }

    if (endP == Piece::Rook) {
        if (end == 0) {
            castle &= 0b1101;
        } else if (end == 7) {
            castle &= 0b1110;
        } else if (end == 56) {
            castle &= 0b0111;
        } else if (end == 63) {
            castle &= 0b1011;
        }
    }

    if (startP == Piece::King) {
        if (startC == Color::White) {
            castle &= 0b1100;
        } else {
            castle &= 0b0011;
        }
    }
    kingLoc = getPieces(startC, Piece::King);
    int newKingSquare = reverseLookup(kingLoc);

    whiteMove = !whiteMove;
    moveList.push_back(Move(start, end, flags));
    captureList.push_back(endP);
    enPassantList.push_back(enPassant);
    castleList.push_back(castle);
    hashList.push_back(tmpHashVal);
    if (attacked(newKingSquare, opp)) {
        unmakeMove();
        return false;
    }

    hashVal = tmpHashVal;
    return true;
}

void Board::unmakeMove() {
    whiteMove = !whiteMove;
    Move lastMove = moveList.back();
    Piece endP = captureList.back();
    Color endC = Color::White;
    if (whiteMove) {
        endC = Color::Black;
    }

    bool prom = lastMove.getProm();
    bool capture = lastMove.getCapture();

    castleList.pop_back();
    castle = castleList.back();
    enPassantList.pop_back();
    enPassant = enPassantList.back();
    hashList.pop_back();
    hashVal = hashList.back();
    moveList.pop_back();
    captureList.pop_back();
    int start = lastMove.getStart();
    int end = lastMove.getEnd();
    int flags = lastMove.getFlags();
    int type = lastMove.getType();

    Color startC = getColor(end);
    Piece startP;

    if (!prom) {
        startP = getPiece(end); 
    } else {
        startP = Piece::Pawn;
    }

    U64 startBB = lookup[start];
    U64 endBB = lookup[end];
    U64 startEndBB = startBB ^ endBB;

    pieceBB[(int)startP + 2] ^= startEndBB; 
    pieceBB[(int)startC] ^= startEndBB;

    if (flags == 5) { // en passant
        endP = Piece::Pawn;
        if (startC == Color::White) {
            pieceBB[2] ^= lookup[end - 8];    
            pieceBB[1] ^= lookup[end - 8];
        } else {
            pieceBB[2] ^= lookup[end + 8];    
            pieceBB[0] ^= lookup[end + 8];
        }
    } else if (capture) {
        pieceBB[(int) endP + 2] ^= endBB;
        pieceBB[(int) endC] ^= endBB;
    }

    if (prom) {
        int promPiece = 1 + (type & 3);
        pieceBB[promPiece + 2] ^= endBB;
        pieceBB[2] ^= endBB;
    } 
    
    if (((type >> 1) & 1) == 1) { // castling
        if (!inCheck()) {
            U64 rookBB;
            if ((type & 1) == 0) { // kingside
                if (startC == Color::White) {
                    rookBB = lookup[5] ^ lookup[7];
                } else { 
                    rookBB = lookup[61] ^ lookup[63];
                }
                pieceBB[5] ^= rookBB;
                pieceBB[(int) startC] ^= rookBB;
            } else { // queenside
                if (startC == Color::White) {
                    rookBB = lookup[0] ^ lookup[3];
                } else {
                    rookBB = lookup[56] ^ lookup[59];
                }
                pieceBB[5] ^= rookBB;
                pieceBB[(int) startC] ^= rookBB;
            }
        }
    } 
}

bool Board::inCheckmate() {
    if (inCheck()) {
        for (Move m : getLegalMoves()) {
            if (makeMove(m)) {
                unmakeMove();                
                return false;
            }
        }
        return true;
    }
    return false;
}

std::vector<Move> Board::getLegalMoves() const {
    if (checkCount() == 2) {
        return getDoubleCheckMoves();
    } else if (checkCount() == 1) {
        return getCheckMoves();
    } else {
        return getNormalMoves();
    }
}

// Returns a vector of the possible moves.
std::vector<Move> Board::getNormalMoves() const {
    U64 pieces = pieceBB[0];
    Color c;
    Move m(0, 0, 0);
    std::vector<Move> moves;
    if (whiteMove) {
        c = Color::White;
    } else {
        pieces = pieceBB[1];
        c = Color::Black;
    }
    for (int count : getIndices(getPieces())) {
        U64 pBB = lookup[count]; 
        Piece p = getPiece(count);
        if (p == Piece::Pawn) {
            U64 pawnAttacks = pawn_attacks(pBB, c);
            U64 pawnMoves = single_pawn_moves(pBB, c) | double_pawn_moves(pBB, c);
            for (int att : getIndices(pawnAttacks)) {
                if (getPiece(att) != Piece::None) {
                    if ((lookup[att] & Rank1) || (lookup[att] & Rank7)) {
                        for (int i = 8; i <= 11; i++) {
                            moves.push_back(Move(count, att, i));
                        }
                    }
                    moves.push_back(Move(count, att, 4));
                }
                moves.push_back(Move(count, att, 5));
            }
            for (int move : getIndices(pawnMoves)) {
                if (getPiece(move) == Piece::None) {
                    for (int i = 12; i <= 15; i++) {
                        Move m(count, move, i);
                        moves.push_back(m);
                    }
                    for (int i = 0; i <= 1; i++) {
                        Move m(count, move, i);
                        moves.push_back(m);
                    } 
                }
            }
        } else if (p == Piece::Knight) {
            U64 knightMoves = knight_attacks(pBB);
            for (int move : getIndices(knightMoves)) {
                if (getPiece(move) == Piece::None) {
                    moves.push_back(Move(count, move, 0));
                } else {
                    moves.push_back(Move(count, move, 4));
                }
            }
        } else if (p == Piece::Bishop) {
            U64 bishopMoves = bishop_attacks(pBB, getPieces());
            for (int move : getIndices(bishopMoves)) {
                if (getPiece(move) == Piece::None) {
                    moves.push_back(Move(count, move, 0));
                } else {
                    moves.push_back(Move(count, move, 4));
                }
            }
        } else if (p == Piece::Rook) {
            U64 rookMoves = rook_attacks(pBB, getPieces());
            for (int move : getIndices(rookMoves)) {
                if (getPiece(move) == Piece::None) {
                    moves.push_back(Move(count, move, 0));
                } else {
                    moves.push_back(Move(count, move, 4));
                }
            }
        } else if (p == Piece::Queen) {
            U64 queenMoves = queen_attacks(pBB, getPieces());
            for (int move : getIndices(queenMoves)) {
                if (getPiece(move) == Piece::None) {
                    moves.push_back(Move(count, move, 0));
                } else {
                    moves.push_back(Move(count, move, 4));
                }
            }
        } else if (p == Piece::King) {
            U64 kingMoves = king_attacks(pBB);
            for (int move : getIndices(kingMoves)) {
                if (getPiece(move) == Piece::None) {
                    moves.push_back(Move(count, move, 0));
                } else {
                    moves.push_back(Move(count, move, 4));
                }
            }
        }
    }
    m = Move(4, 6, 2);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = Move(4, 2, 3);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = Move(60, 62, 2);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = Move(60, 58, 3);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    return moves;
}

// Returns a vector of the possible moves if the player is in check
std::vector<Move> Board::getCheckMoves() const {
    U64 pieces; 
    std::vector<Move> moves;
    std::vector<int> endSquares;
    Move m(0, 0, 0);
    Color c, opp;    
    if (whiteMove) {
        pieces = pieceBB[0];
        c = Color::White;
        opp = Color::Black;
    } else {
        pieces = pieceBB[1];
        c = Color::Black;
        opp = Color::White;
    }
    U64 kingLoc = getPieces(c, Piece::King);
    int kingSquare = reverseLookup(kingLoc);
    if (kingLoc & pawn_attacks(getPieces(opp, Piece::Pawn), opp)) {
        int square = 0;
        for (int i = 0; 1; i++) {
            if (pawn_attacks(lookup[i], opp) & kingLoc) {
                if (getPieces(opp, Piece::Pawn) & lookup[i]) {
                    square = i;
                    break;
                }
            }
        }
        endSquares.push_back(square);
    } else if (kingLoc & knight_attacks(getPieces(opp, Piece::Knight))) {
        int square = 0;
        for (int i = 0; 1; i++) {
            if (knight_attacks(lookup[i]) & kingLoc) {
                if (getPieces(opp, Piece::Knight) & lookup[i]) {
                    square = i;
                    break;
                }
            }
        }
        endSquares.push_back(square);
    } else if (kingLoc & bishop_attacks(getPieces(opp, Piece::Bishop), getPieces())) {
        int square = 0;
        for (int i = 0; 1; i++) {
            if (bishop_attacks(lookup[i], getPieces()) & kingLoc) {
                if (getPieces(opp, Piece::Bishop) & lookup[i]) {
                    square = i;
                    break;
                }
            }
        }
        U64 squares = bishop_attacks(kingLoc, lookup[square]) & Bmagic(square, 0);
        endSquares = getIndices(squares);
        endSquares.push_back(square);
    } else if (kingLoc & rook_attacks(getPieces(opp, Piece::Rook), getPieces())) {
        int square = 0;
        for (int i = 0; 1; i++) {
            if (rook_attacks(lookup[i], getPieces()) & kingLoc) {
                if (getPieces(opp, Piece::Rook) & lookup[i]) {
                    square = i;
                    break;
                }
            }
        }
        U64 squares = rook_attacks(kingLoc, lookup[square]) & Rmagic(square, 0);
        endSquares = getIndices(squares);
        endSquares.push_back(square);
    } else if (kingLoc & queen_attacks(getPieces(opp, Piece::Queen), getPieces())) {
        int square = 0;
        for (int i = 0; 1; i++) {
            if (queen_attacks(lookup[i], getPieces()) & kingLoc) {
                if (getPieces(opp, Piece::Queen) & lookup[i]) {
                    square = i;
                    break;
                }
            }
        }
        U64 squares;
        if (sameDiagonal(square, kingSquare)) {
            squares = bishop_attacks(kingLoc, lookup[square]) & Bmagic(square, 0);
        } else {
            squares = rook_attacks(kingLoc, lookup[square]) & Rmagic(square, 0);
        }
        endSquares = getIndices(squares);
        endSquares.push_back(square);
    }
    for (int count : getIndices(getPieces())) {
        U64 pBB = lookup[count]; 
        Piece p = getPiece(count);
        if (p == Piece::Pawn) {
            U64 pawnAttacks = pawn_attacks(pBB, c);
            U64 pawnMoves = single_pawn_moves(pBB, c) | double_pawn_moves(pBB, c);
            for (int att : getIndices(pawnAttacks)) {
                if (std::find(endSquares.begin(), endSquares.end(), att) != endSquares.end()) {
                    if (getPiece(att) != Piece::None) {
                        if ((lookup[att] & Rank1) || (lookup[att] & Rank7)) {
                            for (int i = 8; i <= 11; i++) {
                                moves.push_back(Move(count, att, i));
                            }
                        }
                        moves.push_back(Move(count, att, 4));
                    }
                    moves.push_back(Move(count, att, 5));
                }
            }
            for (int move : getIndices(pawnMoves)) {
                if (std::find(endSquares.begin(), endSquares.end(), move) != endSquares.end()) {
                    if (getPiece(move) == Piece::None) {
                        for (int i = 12; i <= 15; i++) {
                            moves.push_back(Move(count, move, i));
                        }
                        moves.push_back(Move(count, move, 0));
                        moves.push_back(Move(count, move, 1));
                    }
                }
            }
        } else if (p == Piece::Knight) {
            U64 knightMoves = knight_attacks(pBB);
            for (int move : getIndices(knightMoves)) {
                if (std::find(endSquares.begin(), endSquares.end(), move) != endSquares.end()) {
                    if (getPiece(move) == Piece::None) {
                        moves.push_back(Move(count, move, 0));
                    } else {
                        moves.push_back(Move(count, move, 4));
                    }
                }
            }
        } else if (p == Piece::Bishop) {
            U64 bishopMoves = bishop_attacks(pBB, getPieces());
            for (int move : getIndices(bishopMoves)) {
                if (std::find(endSquares.begin(), endSquares.end(), move) != endSquares.end()) {
                    if (getPiece(move) == Piece::None) {
                        moves.push_back(Move(count, move, 0));
                    } else {
                        moves.push_back(Move(count, move, 4));
                    }
                }
            }
        } else if (p == Piece::Rook) {
            U64 rookMoves = rook_attacks(pBB, getPieces());
            for (int move : getIndices(rookMoves)) {
                if (std::find(endSquares.begin(), endSquares.end(), move) != endSquares.end()) {
                    if (getPiece(move) == Piece::None) {
                        moves.push_back(Move(count, move, 0));
                    } else {
                        moves.push_back(Move(count, move, 4));
                    }
                }
            }
        } else if (p == Piece::Queen) {
            U64 queenMoves = queen_attacks(pBB, getPieces());
            for (int move : getIndices(queenMoves)) {
                if (std::find(endSquares.begin(), endSquares.end(), move) != endSquares.end()) {
                    if (getPiece(move) == Piece::None) {
                        moves.push_back(Move(count, move, 0));
                    } else {
                        moves.push_back(Move(count, move, 4));
                    }
                }
            }
        } else if (p == Piece::King) {
            U64 kingMoves = king_attacks(pBB);
            for (int move : getIndices(kingMoves)) {
                if (getPiece(move) == Piece::None) {
                    Move m(count, move, 0);
                    if (legalMove(m)) {
                        moves.push_back(m);
                    }
                } else {
                    Move m(count, move, 4);
                    if (legalMove(m)) {
                        moves.push_back(m);
                    }
                }
            }
        }
    }
    return moves;
}


// Returns a vector of the possible moves if the player is in double check
std::vector<Move> Board::getDoubleCheckMoves() const {
    Color c;
    Move m(0, 0, 0);
    std::vector<Move> moves;
    if (whiteMove) {
        c = Color::White;
    } else {
        c = Color::Black;
    }
    U64 pieces = getPieces(c, Piece::King); 
    int kingSquare = reverseLookup(pieces);
    U64 kingMoves = king_attacks(kingSquare);
    for (int move : getIndices(kingMoves)) {
        if (getPiece(move) == Piece::None) {
            moves.push_back(Move(kingSquare, move, 0));
        } else {
            moves.push_back(Move(kingSquare, move, 4));
        }
    }
    return moves;
}

int Board::boardScore() {
    int eval = 0;
    eval += materialCount(Color::White) - materialCount(Color::Black);
    // eval += doublePawns(Color::White) - doublePawns(Color::Black);
    // eval += isoPawns(Color::White) - isoPawns(Color::Black);
    // eval += blockedPawns(Color::White) - blockedPawns(Color::Black);*/
    eval += 10 * (mobility(Color::White) - mobility(Color::Black));
    if (!whiteMove) {
        eval *= -1;
    }
    return eval;
}

bool Board::compareTo(const Move &a, const Move &b) { // return a < b
    //bool aCapture, bCapture;
    int aCapture = ((int) getPiece(a.getEnd()) + 1) % 7;
    int bCapture = ((int) getPiece(b.getEnd()) + 1) % 7;
    U64 hash = getHashVal();
    HashEntry boardEntry1 = transTableDepth[hash % 10000];
    HashEntry boardEntry2 = transTableAlways[hash % 10000];
    if (boardEntry1.type != -1 && boardEntry1.zobrist == hash) { // checks if there is a Hash Node
        if (boardEntry1.bestMove.equals(a)) {
            return true;
        }
        if (boardEntry1.bestMove.equals(b)) {
            return false;
        }
    }
    if (boardEntry2.type != -1 && boardEntry2.zobrist == hash) { // checks if there is a Hash Node
        if (boardEntry2.bestMove.equals(a)) {
            return true;
        }
        if (boardEntry2.bestMove.equals(b)) {
            return false;
        }
    }
    return aCapture > bCapture;
}

   
// Inserts the given HashEntry into the transportation table at the given index.
void Board::insertTransTable(int index, HashEntry h) {
    transTableAlways[index] = h;
    if (transTableDepth[index].ancient == true) {
        transTableDepth[index] = h;
    } else {
        if (transTableDepth[index].type != -1) {
            HashEntry other = transTableDepth[index];
            if (other.zobrist != h.zobrist) {
                transTableDepth[index] = h; 
            } else {
                if (other.depth < h.depth) {
                    transTableDepth[index] = h; 
                }
            }
        } else {
            transTableDepth[index] = h;
        }
    }
}

U64 Board::updatedHashVal(Move &m) const {
    // Get all the information from the move.
    int start = m.getStart();
    int end = m.getEnd();
    int flags = m.getFlags();
    bool capture = m.getCapture();
    bool prom = m.getProm();
    int type = m.getType();
    // Get the start and end pieces.
    Piece startP = getPiece(start);
    Color startC = getColor(start);
    Piece endP = getPiece(end);
    Color endC = getColor(end);
    // Initialize hash index values for start and end pieces.
    int sHashIndex = 6 * (int) startC + (int) startP;
    int eHashIndex = 6 * (int) endC + (int) endP;

    U64 tmpHashVal = hashVal; 
    tmpHashVal ^= specialHashTable[0];

    tmpHashVal ^= hashTable[start][sHashIndex];
    tmpHashVal ^= hashTable[end][sHashIndex];

    if (type == 1) {
        int file = start % 8;
        if (file == 0) {
            tmpHashVal ^= specialHashTable[6];
        } else if (file == 7) {
            tmpHashVal ^= specialHashTable[11];
        } else {
            tmpHashVal ^= specialHashTable[file + 4];
            tmpHashVal ^= specialHashTable[file + 6];
        }
    }

    if (flags == 5) { // en passant
        endP = Piece::Pawn;
        if (startC == Color::White) {
            tmpHashVal ^= hashTable[end - 8][6];
            tmpHashVal ^= hashTable[end][6];
        } else {
            tmpHashVal ^= hashTable[end + 8][0];
            tmpHashVal ^= hashTable[end][0];
        }
    } else if (capture) {
        tmpHashVal ^= hashTable[end][eHashIndex];
    }

    if (prom) {
        int promPiece = 1 + (type & 3);
        tmpHashVal ^= hashTable[end][sHashIndex];
        tmpHashVal ^= hashTable[end][6 * (int) startC + promPiece];
    } 

    if (((type >> 1) & 1) == 1) { // castling
        if ((type & 1) == 0) { // kingside
            if (startC == Color::White) {
                tmpHashVal ^= hashTable[5][3];
                tmpHashVal ^= hashTable[7][3];
            } else { 
                tmpHashVal ^= hashTable[61][9];
                tmpHashVal ^= hashTable[63][9];
            }
        } else { // queenside
            if (startC == Color::White) {
                tmpHashVal ^= hashTable[0][3];
                tmpHashVal ^= hashTable[3][3];
            } else {
                tmpHashVal ^= hashTable[56][9];
                tmpHashVal ^= hashTable[59][9];
            }
        }
    }
    if (startP == Piece::Rook) {
        if (start == 0) {
            tmpHashVal &= ~specialHashTable[2];
        } else if (start == 7) {
            tmpHashVal &= ~specialHashTable[1];
        } else if (start == 56) {
            tmpHashVal &= ~specialHashTable[4];
        } else if (start == 63) {
            tmpHashVal &= ~specialHashTable[3];
        }
    }

    if (startP == Piece::King) {
        if (startC == Color::White) {
            tmpHashVal &= specialHashTable[1];
            tmpHashVal &= ~specialHashTable[2];
        } else {
            tmpHashVal &= ~specialHashTable[3];
            tmpHashVal &= ~specialHashTable[4];
        }
    }
    return tmpHashVal;
}

// Resets the transposition table to its default state
void Board::flushTransTable() {
    for (int i = 0; i < 10000; i++) {
        transTableAlways[i].ancient = true;
        transTableDepth[i].ancient = true;
    }
}

// Returns the number of times the king is attacked
int Board::checkCount() const {
    int count = 0;
    Color c, opp;    
    if (whiteMove) {
        c = Color::White;
        opp = Color::Black;
    } else {
        c = Color::Black;
        opp = Color::White;
    }
    U64 kingLoc = getPieces(c, Piece::King);
    if (kingLoc & pawn_attacks(getPieces(opp, Piece::Pawn), opp)) {
        count++;
    } 
    if (kingLoc & knight_attacks(getPieces(opp, Piece::Knight))) {
        count++;
    } 
    if (kingLoc & bishop_attacks(getPieces(opp, Piece::Bishop), getPieces())) {
        count++;
    } 
    if (kingLoc & rook_attacks(getPieces(opp, Piece::Rook), getPieces())) {
        count++;
    } 
    if (kingLoc & queen_attacks(getPieces(opp, Piece::Queen), getPieces())) {
        count++;
    }
    return count;
}
