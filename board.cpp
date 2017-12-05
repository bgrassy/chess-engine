#include "board.hpp"

board::board() {
    pieceBB[0] = 0x000000000000FFFF; // white pieces
    pieceBB[1] = 0xFFFF000000000000; // black pieces
    pieceBB[2] = 0x00FF00000000FF00; // pawns
    pieceBB[3] = 0x4200000000000042; // knights
    pieceBB[4] = 0x2400000000000024; // bishops
    pieceBB[5] = 0x8100000000000081; // rooks
    pieceBB[6] = 0x0800000000000008; // queens
    pieceBB[7] = 0x1000000000000010; // kings
    for (auto i = 0; i < 64; i++) {
        lookup[i] = (U64)1 << i;
    }
    moveList.push_back(move(0, 0, 0));
    castleList.push_back(castle);
    enPassantList.push_back(enPassant);
    initHash();
    hashVal = 0;
    for (auto i = 0; i < 64; i++) {
        int c = (int) getColor(i); 
        int p = (int) getPiece(i); 
        if (c != 2) { // if color is not none
            hashVal ^= hashTable[i][6 * c + p];
        }
    }
    for (auto i = 0; i < 5; i++) {
        hashVal ^= specialHashTable[i]; // whiteMove, castle;
    }
    hashList.push_back(hashVal);
}

U64 board::initHash() {
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<long long int> dist(std::llround(std::pow(2,61)), std::llround(std::pow(2,62)));


    for (auto i = 0; i < 64; i++) {
        for (auto j = 0; j < 12; j++) {
            hashTable[i][j] = dist(e2);
        }
    }

    for (auto i = 0; i < 13; i++) {
        // 0: whiteMove or no
        // 1-4: castle k/q, w/b
        // 5-12: File of en passant
        specialHashTable[i] = dist(e2);
    }
}

bool board::getMove() {
    return whiteMove;
}
// Returns a bitboard representing all the pieces on the board.
U64 board::getPieces() {
    return pieceBB[0] | pieceBB[1];
}

// Takes a color. Returns a bitboard of all the pieces of that color.
U64 board::getPieces(color c) {
    return pieceBB[(int)c];
}

// Takes a piece type. Returns a bitboard of all the pieces of that type.
U64 board::getPieces(piece p) {
    return pieceBB[(int)p + 2];
}

// Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
U64 board::getPieces(color c, piece p) {
    return pieceBB[(int)c] & pieceBB[(int)p + 2];
}

// Flips the board 180 degress.
void board::invert() {
    for (int i = 0; i < 8; i++) {
        pieceBB[i] = flip180(pieceBB[i]);
    }
}

// Takes an unsigned 64-bit integer and "flips it"
U64 board::flip180(U64 x) {
    U64 h1 = 0x5555555555555555;
    U64 h2 = 0x3333333333333333;
    U64 h4 = 0x0F0F0F0F0F0F0F0F;
    U64 v1 = 0x00FF00FF00FF00FF;
    U64 v2 = 0x0000FFFF0000FFFF;
    x = ((x >>  1) & h1) | ((x & h1) <<  1);
    x = ((x >>  2) & h2) | ((x & h2) <<  2);
    x = ((x >>  4) & h4) | ((x & h4) <<  4);
    x = ((x >>  8) & v1) | ((x & v1) <<  8);
    x = ((x >> 16) & v2) | ((x & v2) << 16);
    x = (x >> 32) | (x << 32);
    return x;
}

// Takes two integers corresponding to squares, and returns whether they are in the same rank.
bool board::sameRank(unsigned int square1, unsigned int square2) {
    return (square1 >> 3) == (square2 >> 3);
}

// Takes two integers corresponding to squares, and returns whether they are in the same file.
bool board::sameFile(unsigned int square1, unsigned int square2) {
    return (square1 & 7) == (square2 & 7);
}

// Takes two integers corresponding to squares, and returns whether they are in the same diagonal.
bool board::sameDiagonal(unsigned int square1, unsigned int square2) {
    return bishop_attacks(lookup[square1]) & bishop_attacks(lookup[square2]);
}
// Checks to see if there are any pieces between two given squares. 
bool board::inBetween(unsigned int square1, unsigned int square2) {
    if (square1 > square2) {
        int tmp = square1;
        square1 = square2;
        square2 = tmp;
    }
    if (sameRank(square1, square2)) { // in the same row
        for (unsigned int i = square1 + 1; i < square2; ++i) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameFile(square1, square2)) { // in the same column
        for (unsigned int i = square1 + 8; i < square2; i+=8) {
            if (lookup[i] & getPieces()) {
                return true;
            }
        }
    } else if (sameDiagonal(square1, square2)) {
        if ((square2 - square1) % 9 == 0) {
            for (unsigned int i = square1 + 9; i < square2; i+=9) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }
        } else if ((square2 - square1) % 7 == 0) {
            for (unsigned int i = square1 + 7; i < square2; i+=7) {
                if (lookup[i] & getPieces()) {
                    return true;
                }
            }

        }
    }
    return false;
} 

// Takes a move, and tests if it is a "pseudo-legal" move.
bool board::legalMove(class move m) {
    int start = m.getStart();
    int end = m.getEnd();
    bool capture = m.getCapture();
    bool prom = m.getProm();
    int type = m.getType();
    piece p = getPiece(start);
    color c = getColor(start);
    color opp = (color) (((int)c+1)%2);
    U64 startBB = lookup[start];
    U64 endBB = lookup[end];
    move lastMove = moveList.back();
    if (getPiece(start) == piece::None) {
        return false;
    } 
    if (whiteMove != (c == color::White)) {
        return false;
    }
    if (capture && (getColor(end) != opp) && (p != piece::Pawn)) {
        return false;
    }

    if (!capture && (getPiece(end) != piece::None)) {
        return false;
    }

    if (prom && (!(startBB & Rank7) || (p != piece::Pawn))) {
        return false;
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

    if (!prom && !capture && ((type >> 1) == 1)) { // castling
        if (c == color::White) {
            if (p != piece::King) {
                return false;
            }
            if (end == 2) {
                return (castle & 2) && ((type & 1) == 1);            
            } else if (end == 6) {
                return (castle & 1) && ((type & 1) == 0);
            }
        } else {
            if (p != piece::King) {
                return false;
            }
            if (end == 58) {
                return (castle & 8) && ((type & 1) == 1);            
            } else if (end == 60) {
                return (castle & 4) && ((type & 1) == 0);
            }
        }
    }
    
    if (p == piece::Pawn) {
        if (capture) {
            if (type == 1) { // en passant
                if (lastMove.getFlags() == 1) {
                    if (c == color::White) {
                        return (lastMove.getEnd() == end - 8) && (pawn_attacks(startBB, c) & endBB);
                    } else {
                        return (lastMove.getEnd() == end + 8) && (pawn_attacks(startBB, c) & endBB);
                    }
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
    } else if (p == piece::Knight) {
        return knight_attacks(startBB) & endBB;
    } else if (p == piece::Bishop) {
        return bishop_attacks(startBB) & endBB;
    } else if (p == piece::Rook) {
        return rook_attacks(startBB) & endBB;
    } else if (p == piece::Queen) {
        return queen_attacks(startBB) & endBB;
    } else if (p == piece::King) {
        return king_attacks(startBB) & endBB;
    }
    return false;
}

// Takes a square and color and determines whether that player is attacking that square. 
bool board::attacked(unsigned int square, color c) {
    U64 sq = lookup[square]; 
    if (sq & (pawn_attacks(getPieces(c, piece::Pawn), c) | 
                knight_attacks(getPieces(c, piece::Knight)))) {
        return true;
    } 
    if (sq & bishop_attacks(getPieces(c, piece::Bishop))) {
        U64 b = getPieces(c, piece::Bishop); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                if (bishop_attacks(lookup[count]) & sq) {
                    if (!inBetween(square, count)) {
                        return true;
                    }
                }
            }
            b >>= 1;
            ++count;
        }
    } 
    if (sq & rook_attacks(getPieces(c, piece::Rook))) {
        U64 b = getPieces(c, piece::Rook); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                if (rook_attacks(lookup[count]) & sq) {
                    if (!inBetween(square, count)) {
                        return true;
                    }
                }
            }
            b >>= 1;
            ++count;
        }
    } 
    if (sq & queen_attacks(getPieces(c, piece::Queen))) {
        U64 b = getPieces(c, piece::Queen); 
        int count = 0;
        while (b > 0) {
            if (b & 1) {
                if (queen_attacks(lookup[count]) & sq) {
                    if (!inBetween(square, count)) {
                        return true;
                    }
                }
            }
            b >>= 1;
            ++count;
        }
    }
    return false;
}

// Takes a square, and returns the color of the piece on the square
color board::getColor(unsigned int square) {
    U64 b = lookup[square];
    if (b & getPieces(color::White)) {
        return color::White;
    } else if (b & getPieces(color::Black)) {
        return color::Black;
    }
    return color::None;
}

// Takes a square, and returns the type of the piece on the square.
piece board::getPiece(unsigned int square) {
    U64 b = lookup[square];
    for (int i = 0; i < 6; i++) {
        if (b & getPieces((piece) i)) {
            return (piece) i;
        }
    }
    return piece::None;
}

// Returns whether the current player is in check or not.
bool board::inCheck() {
    color c, opp;    
    if (whiteMove) {
        c = color::White;
        opp = color::Black;
    } else {
        c = color::Black;
        opp = color::White;
    }
    U64 kingLoc = getPieces(c, piece::King);
    int kingSquare = 0;
    while (kingLoc > 1) {
        kingSquare++;
        kingLoc >>= 1;
    } 
    return attacked(kingSquare, opp);
}

// Takes a move, and executes the move on the board if it is legal.
bool board::makeMove(move m) {
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
    piece startP = getPiece(start);
    color startC = getColor(start);
    piece endP = getPiece(end);
    color endC = getColor(end);
    color opp = (color) (((int) startC + 1) % 2);
    // Initialize hash index values for start and end pieces.
    int sHashIndex = 6 * (int) startC + (int) startP;
    int eHashIndex = 6 * (int) endC + (int) endP;
    // Determines the square of the king
    U64 kingLoc = getPieces(startC, piece::King);
    int kingSquare = 0;
    while (kingLoc > 1) {
        kingSquare++;
        kingLoc >>= 1;
    } 

    if (!legalMove(m)) { return false;}

    U64 tmpHashVal = hashVal; 
    tmpHashVal ^= specialHashTable[0];
    pieceBB[(int)startP + 2] ^= startEndBB; 
    pieceBB[(int)startC] ^= startEndBB;

    tmpHashVal ^= hashTable[start][sHashIndex];
    tmpHashVal ^= hashTable[end][sHashIndex];
    
    enPassant = 0;
    if (type == 1) {
        int file = start % 8;
        if (file == 0) {
            enPassant = 0b00000010;
            tmpHashVal ^= specialHashTable[6];
        } else if (file == 7) {
            enPassant = 0b01000000;
            tmpHashVal ^= specialHashTable[11];
        } else {
            enPassant ^= lookup[file - 1] ^ lookup[file + 1];
            tmpHashVal ^= specialHashTable[file + 4];
            tmpHashVal ^= specialHashTable[file + 6];
        }
    }

    if (capture) {
        pieceBB[(int) endP + 2] ^= endBB;
        pieceBB[(int) endC] ^= endBB;
        tmpHashVal ^= hashTable[end][eHashIndex];
    }

    if (prom) {
        int promPiece = 1 + (type & 3);
        pieceBB[promPiece + 2] ^= endBB;
        pieceBB[2] ^= endBB;
        tmpHashVal ^= hashTable[end][sHashIndex];
        tmpHashVal ^= hashTable[end][6 * (int) startC + promPiece];
    } 
    
    if (((type >> 1) & 1) == 1) { // castling
        if (!inCheck()) {
            U64 rookBB;
            if ((type & 1) == 0) { // kingside
                if (!attacked(kingSquare + 1, opp) && !attacked(kingSquare + 2, opp)) { // not castle through check
                    if (startC == color::White) {
                        rookBB = lookup[5] ^ lookup[7];
                        castle ^= 0b0001;
                        tmpHashVal ^= hashTable[5][3];
                        tmpHashVal ^= hashTable[7][3];
                    } else { 
                        rookBB = lookup[61] ^ lookup[63];
                        castle ^= 0b0100;
                        tmpHashVal ^= hashTable[61][9];
                        tmpHashVal ^= hashTable[63][9];
                    }
                    pieceBB[5] ^= rookBB;
                    pieceBB[(int) startC] ^= rookBB;
                } else {
                    pieceBB[(int)startP + 2] ^= startEndBB; 
                    pieceBB[(int)startC] ^= startEndBB;
                    tmpHashVal ^= hashTable[start][sHashIndex];
                    tmpHashVal ^= hashTable[end][sHashIndex];
                    return false;
                }
            } else { // queenside
                if (!attacked(kingSquare - 1, opp) && !attacked(kingSquare - 2, opp)) { // not castle through check
                    if (startC == color::White) {
                        rookBB = lookup[0] ^ lookup[3];
                        castle ^= 0b0010;
                        tmpHashVal ^= hashTable[0][3];
                        tmpHashVal ^= hashTable[3][3];
                    } else {
                        rookBB = lookup[54] ^ lookup[57];
                        castle ^= 0b1000;
                        tmpHashVal ^= hashTable[54][9];
                        tmpHashVal ^= hashTable[57][9];
                    }
                    pieceBB[5] ^= rookBB;
                    pieceBB[(int) startC] ^= rookBB;
                } else {
                    pieceBB[(int)startP + 2] ^= startEndBB; 
                    pieceBB[(int)startC] ^= startEndBB;
                    tmpHashVal ^= hashTable[start][sHashIndex];
                    tmpHashVal ^= hashTable[end][sHashIndex];
                    return false;
                }
            }
        } else { // return to original state, return false
            pieceBB[(int)startP + 2] ^= startEndBB; 
            pieceBB[(int)startC] ^= startEndBB;
            tmpHashVal ^= hashTable[start][sHashIndex];
            tmpHashVal ^= hashTable[end][sHashIndex];
            return false;
        } 
    } 
    
    if (type == 5) { // en passant
        endP = piece::Pawn;
        if (startC == color::White) {
            endC = color::Black;
            pieceBB[(int) endP + 2] ^= lookup[end - 8];    
            pieceBB[(int) endC] ^= lookup[end - 8];
            tmpHashVal ^= hashTable[end - 8][6];
        } else {
            endC = color::White;
            pieceBB[(int) endP + 2] ^= lookup[end + 8];    
            pieceBB[(int) endC] ^= lookup[end + 8];
            tmpHashVal ^= hashTable[end + 8][0];
        }
    }

    if (startP == piece::Rook) {
        if (start == 0) {
            castle ^= 0b0010;
            tmpHashVal ^= specialHashTable[2];
        } else if (start == 7) {
            castle ^= 0b0001;
            tmpHashVal ^= specialHashTable[1];
        } else if (start == 54) {
            castle ^= 0b1000;
            tmpHashVal ^= specialHashTable[4];
        } else if (start == 63) {
            castle ^= 0b0100;
            tmpHashVal ^= specialHashTable[3];
        }
    }

    if (startP == piece::King) {
        if (startC == color::White) {
            castle ^= 0b0011;
            tmpHashVal ^= specialHashTable[1];
            tmpHashVal ^= specialHashTable[2];
        } else {
            castle ^= 0b1100;
            tmpHashVal ^= specialHashTable[3];
            tmpHashVal ^= specialHashTable[4];
        }
    }
    int newKingSquare = 0;
    kingLoc = getPieces(startC, piece::King);
    while (kingLoc > 1) {
        newKingSquare++;
        kingLoc >>= 1;
    } 

    whiteMove = !whiteMove;
    moveList.push_back(move(start, end, flags));
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

void board::unmakeMove() {
    whiteMove = !whiteMove;
    move lastMove = moveList.back();
    piece lastCaptured = captureList.back();
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
    U64 startBB = lookup[start];
    U64 endBB = lookup[end];
    color sCol = getColor(end);
    piece sPiece = getPiece(end);
    if (lastCaptured != piece::None) {
        pieceBB[2 + (int) lastCaptured] ^= endBB;
        pieceBB[((int) sCol + 1) % 2] ^= endBB; 
    }
    pieceBB[(int) sCol] ^= (startBB ^ endBB);
    pieceBB[2 + (int) sPiece] ^= (startBB ^ endBB);
    if (!lastMove.getProm() && !lastMove.getCapture()) {
        if ((flags >> 1) != 0) { // castling
            if (sCol == color::White) {
                if (flags % 2 == 0) { // kingside
                    pieceBB[5] ^= (lookup[5] | lookup[7]);
                } else {
                    pieceBB[5] ^= (lookup[3] | lookup[0]);
                }
            } else {
                if (flags % 2 == 0) { // kingside
                    pieceBB[5] ^= (lookup[61] | lookup[63]);
                } else {
                    pieceBB[5] ^= (lookup[59] | lookup[56]);
                }
            }
        }
    } else if (lastMove.getProm()) {
        pieceBB[(int) sPiece + 2] ^= startBB; 
        pieceBB[2] ^= startBB;
    } else if (flags == 5) { // en passant
        pieceBB[((int) sCol + 1) % 2] ^= endBB; 
        if (sCol == color::White) {
            pieceBB[((int) sCol + 1) % 2] ^= lookup[end - 8]; 
        } else {
            pieceBB[((int) sCol + 1) % 2] ^= lookup[end + 8]; 
        }
    }
}
// Returns a vector of the possible moves.
std::vector<move> board::getLegalMoves() {
    U64 pieces; 
    color c;
    move m(0, 0, 0);
    std::vector<move> moves;
    if (whiteMove) {
        pieces = pieceBB[0];
        c = color::White;
    } else {
        pieces = pieceBB[1];
        c = color::Black;
    }
    int count = 0;
    while (pieces > 0) {
        if ((pieces & 1) == 1) {
            U64 pBB = lookup[count]; 
            piece p = getPiece(count);
            if (p == piece::Pawn) {
                U64 pawnAttacks = pawn_attacks(pBB, c);
                U64 pawnMoves = single_pawn_moves(pBB, c) | double_pawn_moves(pBB, c);
                int attCount = 0;
                int moveCount = 0;
                while (pawnAttacks > 0) {
                    if ((pawnAttacks & 1) == 1) {
                        for (int i = 8; i <= 11; i++) {
                            move m(count, attCount, i);
                            if (legalMove(m)) {
                                moves.push_back(m);
                            }
                        }
                        for (int i = 4; i <= 5; i++) {
                            move m(count, attCount, i);
                            if (legalMove(m)) {
                                moves.push_back(m);
                            }
                        }
                    }
                    attCount++;
                    pawnAttacks >>= 1;
                }
                while (pawnMoves > 0) {
                    if ((pawnMoves & 1) == 1) {
                        for (int i = 12; i <= 15; i++) {
                            move m(count, moveCount, i);
                            if (legalMove(m)) {
                                moves.push_back(m);
                            }
                        }
                        for (int i = 0; i <= 1; i++) {
                            move m(count, moveCount, i);
                            if (legalMove(m)) {
                                moves.push_back(m);
                            }
                        } 
                    }
                    moveCount++;
                    pawnMoves >>= 1;
                }
            } else if (p == piece::Knight) {
                U64 knightMoves = knight_attacks(pBB);
                int moveCount = 0;
                while (knightMoves > 0) {
                    if ((knightMoves & 1) == 1) {
                        move m(count, moveCount, 0);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                        m = move(count, moveCount, 4);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                    }
                    moveCount++;
                    knightMoves >>= 1;
                }
            } else if (p == piece::Bishop) {
                U64 bishopMoves = bishop_attacks(pBB);
                int moveCount = 0;
                while (bishopMoves > 0) {
                    if ((bishopMoves & 1) == 1) {
                        move m(count, moveCount, 0);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                        m = move(count, moveCount, 4);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                    }
                    moveCount++;
                    bishopMoves >>= 1;
                }
            } else if (p == piece::Rook) {
                U64 rookMoves = rook_attacks(pBB);
                int moveCount = 0;
                while (rookMoves > 0) {
                    if ((rookMoves & 1) == 1) {
                        move m(count, moveCount, 0);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                        m = move(count, moveCount, 4);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                    }
                    moveCount++;
                    rookMoves >>= 1;
                }
            } else if (p == piece::Queen) {
                U64 queenMoves = queen_attacks(pBB);
                int moveCount = 0;
                while (queenMoves > 0) {
                    if ((queenMoves & 1) == 1) {
                        move m(count, moveCount, 0);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                        m = move(count, moveCount, 4);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                    }
                    moveCount++;
                    queenMoves >>= 1;
                }
            } else if (p == piece::King) {
                U64 kingMoves = king_attacks(pBB);
                int moveCount = 0;
                while (kingMoves > 0) {
                    if ((kingMoves & 1) == 1) {
                        move m(count, moveCount, 0);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                        m = move(count, moveCount, 4);
                        if (legalMove(m)) {
                            moves.push_back(m);
                        }
                    }
                    moveCount++;
                    kingMoves >>= 1;
                }
            }
        }

        count++;
        pieces >>= 1;
    }
    m = move(4, 6, 2);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = move(4, 2, 3);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = move(60, 62, 2);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    m = move(60, 58, 3);
    if (legalMove(m)) {
        moves.push_back(m);
    }
    return moves;
}

double board::boardScore() {
    double eval = 0;
    double pieceValues[] = {1, 3, 3.5, 5, 9, 10000};
    for (int p = 0; p < 6; p++) {
        for (int c = 0; c < 2; c++) {
            U64 pieces = getPieces((color) c, (piece) p);
            while (pieces > 0) {
                if ((pieces & 1) == 1) {
                    eval += pieceValues[p] * (1 - 2 * c);
                }
                pieces >>= 1;
            }
        }
    }
    if (!whiteMove) {
        eval *= -1;
    }
    return eval;
}
