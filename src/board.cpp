/**
 * Constructs a new Board object to the starting chess position.
 */

#include "board.hpp"
#include <random>
using namespace std;

namespace Zobrist {
    unsigned long long pieces[12][64];
    unsigned long long blackMove;
    unsigned long long castling[4];
    unsigned long long enPassant[8];
};

void initZobrist() {
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<unsigned long long> distr;
    for (int color = nWhite; color <= nBlack; color++) {
        for (int piece = nPawn; piece <= nKing; piece++) {
            for (int sq = A1; sq <= H8; sq++) {
                Zobrist::pieces[2 * color + piece][sq] = distr(eng);
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        Zobrist::castling[i] = distr(eng);
    }
    for (int i = 0; i < 8; i++) {
        Zobrist::enPassant[i] = distr(eng);
    }
    Zobrist::blackMove = distr(eng);
}

void Board::setZobrist() {
    unsigned long long hashKey = 0;
    for (int color = nWhite; color <= nBlack; color++) {
        for (int piece = nPawn; piece <= nKing; piece++) {
            Bitboard pieces = getPieces((Piece)piece, (Color)color);
            while (pieces) {
                int sq = pop_lsb(&pieces);
                hashKey ^= Zobrist::pieces[2 * color + piece][sq];
            }
        }
    }

    short castle = castling.top();
    for (int i = 0; i < 4; i++) {
        if (castle & (1 << i)) {
            hashKey ^= Zobrist::castling[i];
        }
    }

    if (toMove == nBlack) {
        hashKey ^= Zobrist::blackMove;
    }

    if (enPassant.top() != SQ_NONE) {
        hashKey ^= Zobrist::enPassant[enPassant.top() % 8];
    }
    zobrist.push(hashKey);
}


Board::Board() {
    initBitboards();
    initZobrist();

    // initialize pieces
    pieceBB[0] = Rank1 | Rank2;
    pieceBB[1] = Rank7 | Rank8;
    pieceBB[2] = Rank2 | Rank7;
    pieceBB[3] = sqToBB[B1] | sqToBB[G1] | sqToBB[B8] | sqToBB[G8];
    pieceBB[4] = sqToBB[C1] | sqToBB[F1] | sqToBB[C8] | sqToBB[F8];
    pieceBB[5] = sqToBB[A1] | sqToBB[H1] | sqToBB[A8] | sqToBB[H8];
    pieceBB[6] = sqToBB[D1] | sqToBB[D8];
    pieceBB[7] = sqToBB[E1] | sqToBB[E8];

    occupiedBB = (pieceBB[0] | pieceBB[1]);
    emptyBB = ~occupiedBB;

    enPassant.push(SQ_NONE);
    castling.push(0b1111);
    capturedList.push(PIECE_NONE);
    moveList.push(Move(0, 0, 0)); // null move
    toMove = nWhite;
    fiftyList.push(0);
    fullMove = 1;

    setZobrist();
}

/**
 * Constructs a new Board object to the given position
 *
 * @param FEN the desired starting position in FEN form
 */
Board::Board(std::string FEN) {
    initZobrist();
    initBitboards();

    pieceBB[0] = 0; 
    pieceBB[1] = 0;
    pieceBB[2] = 0;
    pieceBB[3] = 0;
    pieceBB[4] = 0;
    pieceBB[5] = 0;
    pieceBB[6] = 0;
    pieceBB[7] = 0;

    std::vector<std::string> result;
    std::vector<std::string> pieceList;
    std::istringstream iss(FEN);
    for(std::string s; iss >> s; )
        result.push_back(s);    

    assert(result.size() == 6 || result.size() == 4);
    std::string pieces = result[0];
    std::replace(pieces.begin(), pieces.end(), '/', ' ');

    std::istringstream pieceISS(pieces);
    for(std::string s; pieceISS >> s; )
        pieceList.push_back(s);    

    assert(pieceList.size() == 8);
    for (int i = 0; i < 8; i++) {
        std::string row = pieceList[i];
        int currSq = 0;
        for (char& ch : row) {
            if (isdigit(ch)) {
                currSq += ch - '0';
                continue;
            }
            Color c = isupper(ch) ? nWhite : nBlack;
            ch = tolower(ch);
            Piece p;
            switch (ch) {
                case 'p':
                    p = nPawn;
                    break;
                case 'n':
                    p = nKnight;
                    break;
                case 'b':
                    p = nBishop;
                    break;
                case 'r':
                    p = nRook;
                    break;
                case 'q':
                    p = nQueen;
                    break;
                case 'k':
                    p = nKing;
                    break;
                default:
                    assert(false);
            }
            pieceBB[c] |= sqToBB[(7 - i) * 8 + currSq];
            pieceBB[2 + p] |= sqToBB[(7 - i) * 8 + currSq];
            currSq++;
        }
    }

    toMove = (result[1] == "w" ? nWhite : nBlack);

    // castling rights
    short castle = 0;
    for (char& ch : result[2]) {
        switch(ch) {
            case 'K':
                castle |= 0b1000;
                break;
            case 'Q':
                castle |= 0b0100;
                break;
            case 'k':
                castle |= 0b0010;
                break;
            case 'q':
                castle |= 0b0001;
                break;
        }
    }
    castling.push(castle);

    // en passant
    if (result[3] == "-") {
        enPassant.push(SQ_NONE);
    } else {
        enPassant.push((Square)(std::find(squareNames, squareNames+65, result[3]) -
                    squareNames));
    }

    if (result.size() == 6) {
        fiftyList.push(stoi(result[4]));
        fullMove = stoi(result[5]);
    } else {
        fiftyList.push(0);
        fullMove = 1;
    }

    occupiedBB = (pieceBB[0] | pieceBB[1]);
    emptyBB = ~occupiedBB;

    setZobrist(); 
}

std::string Board::getFEN() const {
    std::string FEN = "";
    for (int row = 7; row >= 0; row--) {
        int emptyCount = 0;
        for (int col = 0; col < 8; col++) {
            int sq = 8 * row + col;
            if (getPiece(sq) == PIECE_NONE) {
                emptyCount++;
                continue;
            }
            if (emptyCount != 0) {
                FEN += std::to_string(emptyCount);
                emptyCount = 0;
            }
            char p;
            switch(getPiece(sq)) {
                case nPawn:
                    p = 'p';
                    break;
                case nKnight:
                    p = 'n';
                    break;
                case nBishop:
                    p = 'b';
                    break;
                case nRook:
                    p = 'r';
                    break;
                case nQueen:
                    p = 'q';
                    break;
                case nKing:
                    p = 'k';
                    break;
                default:
                    break;
            }
            if (getColor(sq) == nWhite) {
                p = toupper(p); 
            }
            FEN += p;
        }
        if (emptyCount != 0) {
            FEN += std::to_string(emptyCount);
        }
        if (row != 0) {
            FEN += "/";
        }
    }
    FEN += " ";
    FEN += (toMove == nWhite ? "w" : "b");
    FEN += " ";

    short castle = castling.top();
    if (castle & 0b1000) {
        FEN += "K";
    }  
    if (castle & 0b0100) {
        FEN += "Q";
    } 
    if (castle & 0b0010) {
        FEN += "k";
    } 
    if (castle & 0b0001) {
        FEN += "q";
    }
    if (castle) {
        FEN += " ";
    } else {
        FEN += "- ";
    }

    FEN += (enPassant.top() == SQ_NONE ? "-" : squareNames[enPassant.top()]);
    FEN += " ";

    FEN += std::to_string(fiftyList.top()) + " " + std::to_string(fullMove);

    return FEN;
}

// Returns pieces of the given piece type
Bitboard Board::getPieces(Piece pt) const {
    return pieceBB[2 + pt];
}

// Returns pieces of the given color
Bitboard Board::getPieces(Color ct) const {
    return pieceBB[ct];
}

// Returns pieces of the given piece and color
Bitboard Board::getPieces(Piece pt, Color ct) const {
    return pieceBB[2 + pt] & pieceBB[ct];
}

// Returns bitboard of all the white pawns
Bitboard Board::getWhitePawns() const {
    return pieceBB[0] & pieceBB[2];
}

// Returns bitboard of all the white knights
Bitboard Board::getWhiteKnights() const {
    return pieceBB[0] & pieceBB[3];
}

// Returns bitboard of all the white bishops
Bitboard Board::getWhiteBishops() const {
    return pieceBB[0] & pieceBB[4];
}

// Returns bitboard of all the white rooks
Bitboard Board::getWhiteRooks() const {
    return pieceBB[0] & pieceBB[5];
}

// Returns bitboard of all the white queens
Bitboard Board::getWhiteQueens() const {
    return pieceBB[0] & pieceBB[6];
}

// Returns bitboard of the white king
Bitboard Board::getWhiteKing() const {
    return pieceBB[0] & pieceBB[7];
}

// Returns bitboard of all the black pawns
Bitboard Board::getBlackPawns() const {
    return pieceBB[1] & pieceBB[2];
}

// Returns bitboard of all the black knights
Bitboard Board::getBlackKnights() const {
    return pieceBB[1] & pieceBB[3];
}

// Returns bitboard of all the black bishops
Bitboard Board::getBlackBishops() const {
    return pieceBB[1] & pieceBB[4];
}

// Returns bitboard of all the black rooks
Bitboard Board::getBlackRooks() const {
    return pieceBB[1] & pieceBB[5];
}

// Returns bitboard of all the black queens
Bitboard Board::getBlackQueens() const {
    return pieceBB[1] & pieceBB[6];
}

// Returns bitboard of the black king
Bitboard Board::getBlackKing() const {
    return pieceBB[1] & pieceBB[7];
}

Bitboard Board::getOccupied() const {
    return occupiedBB;
}

Bitboard Board::getEmpty() const {
    return emptyBB;
}

Square Board::enPassantTarget() const {
    return enPassant.top();
}

short Board::getCastlingRights() const {
    return castling.top();
}

Color Board::getToMove() const {
    return toMove;
}

unsigned long long Board::getZobrist() const {
    return zobrist.top();
}

bool Board::attacked(int square, Color side) const {
    Bitboard pawns = getPieces(nPawn, side);
    if (pawnAttacks[side^1][square] & pawns) {
        return true;
    }
    Bitboard knights = getPieces(nKnight, side);
    if (knightAttacks[square] & knights) {
        return true;
    }
    Bitboard king = getPieces(nKing, side);
    if (kingAttacks[square] & king) {
        return true;
    }
    Bitboard bishopsQueens = getPieces(nQueen, side) | getPieces(nBishop, side);
    if (slidingAttacksBB<nBishop>(square, occupiedBB) & bishopsQueens) {
        return true;
    }
    Bitboard rooksQueens = getPieces(nQueen, side) | getPieces(nRook, side);
    if (slidingAttacksBB<nRook>(square, occupiedBB) & rooksQueens) {
        return true;
    }
}

Square Board::lva(Square sq, Color side) const {
    Bitboard pawns = getPieces(nPawn, side);
    if (pawnAttacks[side^1][sq] & pawns) {
        return lsb(pawnAttacks[side^1][sq] & pawns);
    }

    Bitboard knights = getPieces(nKnight, side);
    if (knightAttacks[sq] & knights) {
        return lsb(knightAttacks[sq] & knights);
    }

    Bitboard bishops = getPieces(nBishop, side);
    Bitboard bishopAttackers = slidingAttacksBB<nBishop>(sq, occupiedBB) & bishops;
    if (bishopAttackers) {
        return lsb(bishopAttackers);
    }

    Bitboard rooks = getPieces(nRook, side);
    Bitboard rookAttackers = slidingAttacksBB<nRook>(sq, occupiedBB) & rooks;
    if (rookAttackers) {
        return lsb(rookAttackers);
    }

    Bitboard queens = getPieces(nRook, side);
    Bitboard queenAttackers = (slidingAttacksBB<nBishop>(sq, occupiedBB) |
            slidingAttacksBB<nRook>(sq, occupiedBB)) & queens;
    if (queenAttackers) {
        return lsb(queenAttackers);
    }

    Bitboard king = getPieces(nKing, side);
    if (kingAttacks[sq] & king) {
        return lsb(king);
    }
}

bool Board::inCheck() const {
    Square kingSquare = lsb(getPieces(nKing, toMove)); 
    Color other = (toMove == nWhite ? nBlack : nWhite);
    return(attacked(kingSquare, other));
}

bool Board::doubleCheck() const {
    int kingSquare = lsb(getPieces(nKing, toMove)); 
    Color other = (toMove == nWhite ? nBlack : nWhite);
    int count = 0;

    Bitboard knights = getPieces(nKnight, other);
    if (knightAttacks[kingSquare] & knights) {
        count++;
    }
    Bitboard bishopsQueens = getPieces(nQueen, other) | getPieces(nBishop,
            other);
    if (slidingAttacksBB<nBishop>(kingSquare, occupiedBB) & bishopsQueens) {
        if (count == 1) {
            return true;
        }
        count++;
    }

    if (count == 0) {
        return false;
    }

    Bitboard rooksQueens = getPieces(nQueen, other) | getPieces(nRook, other);
    if (slidingAttacksBB<nRook>(kingSquare, occupiedBB) & rooksQueens) {
        return true;
    }
    return false;
}

Bitboard Board::getCheckers() const {
    int kingSquare = lsb(getPieces(nKing, toMove)); 
    Color other = (toMove == nWhite ? nBlack : nWhite);
    Bitboard checkers = 0;

    checkers |= (pawnAttacks[toMove][kingSquare] & getPieces(nPawn, other));
    checkers |= (knightAttacks[kingSquare] & getPieces(nKnight, other));

    Bitboard bishopsQueens = getPieces(nQueen, other) | getPieces(nBishop,
            other);
    checkers |= (slidingAttacksBB<nBishop>(kingSquare, occupiedBB) & bishopsQueens);

    Bitboard rooksQueens = getPieces(nQueen, other) | getPieces(nRook, other);
    checkers |= (slidingAttacksBB<nRook>(kingSquare, occupiedBB) & rooksQueens);

    return checkers;
}

Piece Board::getPiece(int sq) const {
    Bitboard sqBB = sqToBB[sq];
    if (sqBB & pieceBB[2 + nPawn]) {
        return nPawn;
    } else if (sqBB & pieceBB[2 + nKnight]) {
        return nKnight;
    } else if (sqBB & pieceBB[2 + nBishop]) {
        return nBishop;
    } else if (sqBB & pieceBB[2 + nRook]) {
        return nRook;
    } else if (sqBB & pieceBB[2 + nQueen]) {
        return nQueen;
    } else if (sqBB & pieceBB[2 + nKing]) {
        return nKing;
    } else {
        return PIECE_NONE;
    }
}

Color Board::getColor(int sq) const {
    Bitboard sqBB = sqToBB[sq];
    if (sqBB & pieceBB[nWhite]) {
        return nWhite;
    } else if (sqBB & pieceBB[nBlack]) {
        return nBlack;
    } else {
        return COLOR_NONE;
    }
}

bool Board::isLegal(Move m) const {
    int start = m.getFrom();
    int end = m.getTo();
    Color startColor = getColor((Square)start);
    Color endColor = getColor((Square)end);
    Piece startPiece = getPiece((Square)start);
    Color otherColor = (startColor == nWhite ? nBlack : nWhite);

    int pawnSq = end + 8 * (2 * startColor - 1);

    if (m.getFlags() == 1 && (abs(start - end) != 16 || startPiece != nPawn)) {
        return false;
    }

    if (toMove != startColor || toMove == endColor) {
        return false;
    }

    if (m.isCapture() && endColor == startColor) {
        return false;
    }
    
    if (m.isCapture() && (endColor == COLOR_NONE) && (m.getFlags() != 5)) {
        return false;
    }

    if (!m.isCapture() && endColor != COLOR_NONE) {
        return false;
    }
    if (m.getFlags() == 5) { // en passant
        if (end != enPassant.top()) {
            return false;
        }
        if (endColor != COLOR_NONE || startPiece != nPawn) {
            return false;
        }
        // check if there actually is an opposing pawn in the right place
        if (getPiece((Square)(pawnSq)) != nPawn) {
            return false;
        }

        Bitboard bishopsQueens = (pieceBB[nBishop + 2] | pieceBB[nQueen + 2]) &
            pieceBB[otherColor];
        Bitboard rooksQueens = (pieceBB[nRook + 2] | pieceBB[nQueen + 2]) &
            pieceBB[otherColor];
        Bitboard kingLoc = pieceBB[nKing + 2] & pieceBB[startColor];
        Bitboard newOccupied = occupiedBB ^ sqToBB[start] ^ sqToBB[end] ^ sqToBB[pawnSq];
        return !(allSlidingAttacks<nBishop>(bishopsQueens, newOccupied) & kingLoc) &&
                !(allSlidingAttacks<nRook>(rooksQueens, newOccupied) & kingLoc);
    }


    if (startPiece == nKing) {
        return m.getFlags() == 2 || m.getFlags() == 3 || !attacked((Square)m.getTo(),
                otherColor);
    }

    int kingSquare = lsb(getPieces(nKing, toMove));
    if (!(pinnedPieces((getPieces(nBishop) | getPieces(nRook) | getPieces(nQueen)) &
            getPieces(otherColor), (Square)kingSquare) & sqToBB[start])) {
        return true;
    }

    return lineBB[kingSquare][start] & sqToBB[end];
}

// returns all pieces that are pinned to a square by a piece of either color
Bitboard Board::pinnedPieces(Bitboard pinners, Square sq) const {
    Bitboard pinned = 0;
    // get the list of possible attackers/pinners
    Bitboard attackers = ((slidingAttacksBB<nBishop>(sq, 0) & (getPieces(nBishop) |
        getPieces(nQueen))) | (slidingAttacksBB<nRook>(sq, 0) &
        (getPieces(nRook) | getPieces(nQueen)))) & pinners;

    while (attackers) {
        int attackSq = pop_lsb(&attackers);
        Bitboard blockers = betweenBB[attackSq][sq] & occupiedBB;
        if (popcount(blockers) == 1) {
            pinned |= blockers; 
        }
    }

    return pinned;
}

void Board::makeMove(Move m) {
    //if (!isLegal(m)) { return;}
    
    unsigned long long hashKey = zobrist.top();
    int fiftyCounter = fiftyList.top() + 1;
    if (toMove == nBlack) { 
        fullMove++;
    }

    hashKey ^= Zobrist::blackMove;

    toMove = (toMove == nWhite ? nBlack : nWhite);

    // Get all the information from the move.
    int start = m.getFrom();
    int end = m.getTo();
    int flags = m.getFlags();
    bool capture = m.isCapture();
    bool prom = m.isPromotion();
    // Bitboards corresponding to start and end square.
    Bitboard startBB = sqToBB[start];
    Bitboard endBB = sqToBB[end];
    Bitboard startEndBB = startBB | endBB;
    // Get the start and end pieces.
    Piece startP = getPiece(start);
    Color startC = getColor(start);
    Piece endP = getPiece(end);
    Color endC = getColor(end);

    pieceBB[(int)startP + 2] ^= startEndBB; 
    pieceBB[(int)startC] ^= startEndBB;

    hashKey ^= Zobrist::pieces[2 * startC + startP][start];
    hashKey ^= Zobrist::pieces[2 * startC + startP][end];

    short newCastling = castling.top();

    if (startP == nPawn || capture) {
        fiftyCounter = 0;
    }

    if (enPassant.top() != SQ_NONE) {
        hashKey ^= Zobrist::enPassant[enPassant.top() % 8];
    }

    if (flags == 1) {
        enPassant.push((Square)(startC == nWhite ? end - 8 : end + 8));
        hashKey ^= Zobrist::enPassant[enPassant.top() % 8];
    } else {
        enPassant.push(SQ_NONE);
    }

    if (flags == 5) { // en passant
        endP = nPawn;
        if (startC == nWhite) {
            pieceBB[2] ^= sqToBB[end - 8];    
            pieceBB[1] ^= sqToBB[end - 8];
            hashKey ^= Zobrist::pieces[2 * nBlack + nPawn][end - 8];
        } else {
            pieceBB[2] ^= sqToBB[end + 8];    
            pieceBB[0] ^= sqToBB[end + 8];
            hashKey ^= Zobrist::pieces[2 * nWhite + nPawn][end + 8];
        }
    } else if (capture) {
        pieceBB[(int) endP + 2] ^= endBB;
        pieceBB[(int) endC] ^= endBB;
        hashKey ^= Zobrist::pieces[2 * endC + endP][end];
    }

    if (prom) {
        int promPiece = 1 + (flags & 3);
        pieceBB[promPiece + 2] ^= endBB;
        pieceBB[2] ^= endBB;
        hashKey ^= Zobrist::pieces[2 * startC + promPiece][end];
        hashKey ^= Zobrist::pieces[2 * startC + nPawn][end];
    } 

    if (flags == 2) { // castling
        if (startC == nWhite) {
            pieceBB[nRook + 2] ^= (sqToBB[F1] | sqToBB[H1]);
            pieceBB[startC] ^= (sqToBB[F1] | sqToBB[H1]);
            hashKey ^= Zobrist::pieces[2 * nWhite + nRook][F1];
            hashKey ^= Zobrist::pieces[2 * nWhite + nRook][H1];
            newCastling &= 0b0011;
        } else { 
            pieceBB[nRook + 2] ^= (sqToBB[F8] | sqToBB[H8]);
            pieceBB[startC] ^= (sqToBB[F8] | sqToBB[H8]);
            hashKey ^= Zobrist::pieces[2 * nBlack + nRook][F8];
            hashKey ^= Zobrist::pieces[2 * nBlack + nRook][H8];
            newCastling &= 0b1100;
        }
    } else if (flags == 3)  { // queenside
        if (startC == nWhite) {
            pieceBB[nRook + 2] ^= (sqToBB[A1] | sqToBB[D1]);
            pieceBB[startC] ^= (sqToBB[A1] | sqToBB[D1]);
            hashKey ^= Zobrist::pieces[2 * nWhite + nRook][A1];
            hashKey ^= Zobrist::pieces[2 * nWhite + nRook][D1];
            newCastling &= 0b0011;
        } else { 
            pieceBB[nRook + 2] ^= (sqToBB[A8] | sqToBB[D8]);
            pieceBB[startC] ^= (sqToBB[A8] | sqToBB[D8]);
            hashKey ^= Zobrist::pieces[2 * nBlack + nRook][A8];
            hashKey ^= Zobrist::pieces[2 * nBlack + nRook][D8];
            newCastling &= 0b1100;
        }
    } 

    if (startP == nRook) {
        if (start == A1) {
            newCastling &= 0b1011;
        } else if (start == H1) {
            newCastling &= 0b0111;
        } else if (start == A8) {
            newCastling &= 0b1110;
        } else if (start == H8) {
            newCastling &= 0b1101;
        }
    }

    if (endP == nRook) {
        if (end == A1) {
            newCastling &= 0b1011;
        } else if (end == H1) {
            newCastling &= 0b0111;
        } else if (end == A8) {
            newCastling &= 0b1110;
        } else if (end == H8) {
            newCastling &= 0b1101;
        }
    }

    if (startP == nKing) {
        if (startC == nWhite) {
            newCastling &= 0b0011;
        } else {
            newCastling &= 0b1100;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (newCastling & (1 << i)) {
            hashKey ^= Zobrist::castling[i];
        }
    }

    castling.push(newCastling);

    occupiedBB = (pieceBB[0] | pieceBB[1]);
    emptyBB = ~occupiedBB;
    capturedList.push(endP);
    moveList.push(m);
    fiftyList.push(fiftyCounter);
    zobrist.push(hashKey);
}

void Board::unmakeMove() {
    if (toMove == nWhite) {
        fullMove--;
    }

    toMove = (toMove == nWhite ? nBlack : nWhite);
    Move m = moveList.top();
        
    Piece endP = capturedList.top();

    bool prom = m.isPromotion();
    bool capture = m.isCapture();

    castling.pop();
    enPassant.pop();
    capturedList.pop();
    moveList.pop();
    fiftyList.pop();
    zobrist.pop();
    int start = m.getFrom();
    int end = m.getTo();
    int flags = m.getFlags();

    Color startC = getColor(end);
    Piece startP = (prom ? nPawn : getPiece(end));
    Color other = (startC == nWhite ? nBlack : nWhite);

    Bitboard startBB = sqToBB[start];
    Bitboard endBB = sqToBB[end];
    Bitboard startEndBB = startBB ^ endBB;

    pieceBB[(int)startP + 2] ^= startEndBB; 
    pieceBB[(int)startC] ^= startEndBB;

    if (flags == 5) { // en passant
        endP = nPawn;
        if (startC == nWhite) {
            pieceBB[2] ^= sqToBB[end - 8];    
            pieceBB[1] ^= sqToBB[end - 8];
        } else {
            pieceBB[2] ^= sqToBB[end + 8];    
            pieceBB[0] ^= sqToBB[end + 8];
        }
    } else if (capture) {
        pieceBB[(int) endP + 2] ^= endBB;
        pieceBB[(int) other] ^= endBB;
    }

    if (prom) {
        int promPiece = 1 + (flags & 3);
        pieceBB[promPiece + 2] ^= endBB;
        pieceBB[2] ^= endBB;
    } 

    if (flags == 2) { // kingside
        if (startC == nWhite) {
            pieceBB[nRook + 2] ^= (sqToBB[F1] | sqToBB[H1]);
            pieceBB[startC] ^= (sqToBB[F1] | sqToBB[H1]);
        } else { 
            pieceBB[nRook + 2] ^= (sqToBB[F8] | sqToBB[H8]);
            pieceBB[startC] ^= (sqToBB[F8] | sqToBB[H8]);
        }
    } else if (flags == 3) { // queenside
        if (startC == nWhite) {
            pieceBB[nRook + 2] ^= (sqToBB[A1] | sqToBB[D1]);
            pieceBB[startC] ^= (sqToBB[A1] | sqToBB[D1]);
        } else { 
            pieceBB[nRook + 2] ^= (sqToBB[A8] | sqToBB[D8]);
            pieceBB[startC] ^= (sqToBB[A8] | sqToBB[D8]);
        }
    } 

    occupiedBB = (pieceBB[0] | pieceBB[1]);
    emptyBB = ~occupiedBB;
}

HashEntry Board::getTransTable(int key) const {
    return transTable[key];
}

void Board::setTransTable(int key, HashEntry entry) {
   transTable[key] = entry;
}

void Board::printBoard() const {
    for (int row = 7; row >= 0; row--) {
        char p;
        switch(getPiece(8 * row)) {
            case nPawn:
                p = 'p';
                break;
            case nKnight:
                p = 'n';
                break;
            case nBishop:
                p = 'b';
                break;
            case nRook:
                p = 'r';
                break;
            case nQueen:
                p = 'q';
                break;
            case nKing:
                p = 'k';
                break;
            default:
                p = '-';
                break;
        }
        if (getColor(8 * row) == nWhite) {
            p = toupper(p);
        }
        cout << p;
        for (int col = 1; col < 8; col++) {
            int sq = 8 * row + col;
            char p;
            switch(getPiece(sq)) {
                case nPawn:
                    p = 'p';
                    break;
                case nKnight:
                    p = 'n';
                    break;
                case nBishop:
                    p = 'b';
                    break;
                case nRook:
                    p = 'r';
                    break;
                case nQueen:
                    p = 'q';
                    break;
                case nKing:
                    p = 'k';
                    break;
                default:
                    p = '-';
                    break;
            }
            if (getColor(sq) == nWhite) {
                p = toupper(p);
            }
            cout << " " << p;
        }
        cout << endl;
    }
}

int Board::boardScore() const {
    int score = 0;
    for (Color c : {nWhite, nBlack}) {
        for (int p = nPawn; p <= nKing; p++) {
            int scale = (c == nWhite ? 1 : -1);
            Bitboard pieces = getPieces((Piece)p, c);
            while (pieces) {
                Square sq = pop_lsb(&pieces);
                score += scale * PieceVals[p];
                score += scale * pieceTable[p][sq];
            }
        }
    }

    return (toMove == nWhite ? score : -score);
}
