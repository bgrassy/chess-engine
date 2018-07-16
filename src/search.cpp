#include "search.hpp"

const int MATE_VALUE = 25000;
const int MAX_VALUE = 50000;

Search::Search(SearchInfo* info) {
    this->info = info;
}

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and negamax.
int Search::negamax(Board &b, int depth, int alpha, int beta) {
    info->nodes++;
    int ply = info->depth - depth;
    int oldAlpha = alpha;

    if (b.isRep() || b.getFiftyCount() > 99) { // one time repetition, fifty moves
        return 0;
    }

    int hashKey = b.getZobrist() % TABLE_SIZE;
    HashEntry oldEntry = b.getTransTable(hashKey);
    HashEntry entry = b.getTransTable(hashKey);

    if (entry.nodeType != HASH_NULL && entry.depth >= depth) { // valid node
        if (entry.zobrist == b.getZobrist()) {
            if (entry.nodeType == HASH_EXACT) {
                return entry.score;
            } else if (entry.zobrist == HASH_ALPHA) {
                alpha = max(alpha, entry.score);
            } else {
                beta = max(beta, entry.score);
            }
        }
        if (alpha > beta) {
            return entry.score;
        }
    }
    
    if (depth == 0) {
        int score = quiesce(b, alpha, beta);
        return score;
    }

    std::vector<Move> moves;
    std::vector<MoveData> moveList;
    b.getToMove() == nWhite ? getLegalMoves<nWhite>(moves, b) : getLegalMoves<nBlack>(moves, b);

    if (moves.empty()) { 
        if (b.inCheck()) {
            return -MATE_VALUE - depth;
       } else {
            return 0;
       }
    }

    Search::orderMoves(b, moves, moveList, ply);
    int bestVal = -MAX_VALUE;

    Move currBest;

    unsigned int loc = 0;

    for (MoveData mv : moveList) {
        loc++;
        if (2 * loc > moveList.size() && info->stopped) {
            return alpha;
        }
        Move m = mv.move;
        if (b.isLegal(m)) {
            b.makeMove(m);
            int searchVal = -negamax(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (searchVal > bestVal) {
                bestVal = searchVal;
                currBest = m;
            }
            alpha = max(searchVal, alpha);

            if (alpha >= beta) {
                b.killerMoves[ply][1] = b.killerMoves[ply][0];
                b.killerMoves[ply][0] = m;
                break;
            }
        }
    }

    entry.score = bestVal;
    entry.depth = depth;
    entry.move = currBest;

    if (bestVal <= oldAlpha) {
        entry.nodeType = HASH_BETA;
    } else if (bestVal >= beta) {
        entry.nodeType = HASH_ALPHA;
    } else {
        entry.nodeType = HASH_EXACT;
    }

    if (oldEntry.nodeType != HASH_EXACT && entry.nodeType == HASH_EXACT) {
        b.setTransTable(hashKey, entry);
    }
    if (!(oldEntry.nodeType == HASH_EXACT && entry.nodeType != HASH_EXACT)) {
        if (entry.depth >= oldEntry.depth) {
            b.setTransTable(hashKey, entry);
        }
    }

    return bestVal;
}


// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and negamax. Updates the best move in
// the search object.
int Search::negamaxRoot(Board &b, int depth, int alpha, int beta) {
    info->nodes++;
    int ply = info->depth - depth;
    int oldAlpha = alpha;

    int hashKey = b.getZobrist() % TABLE_SIZE;
    HashEntry oldEntry = b.getTransTable(hashKey);
    HashEntry entry = b.getTransTable(hashKey);

    if (entry.nodeType != HASH_NULL && entry.depth >= depth) { // valid node
        if (entry.zobrist == b.getZobrist()) {
            if (entry.nodeType == HASH_EXACT) {
                bestMove = entry.move;
                return entry.score;
            } else if (entry.zobrist == HASH_ALPHA) {
                alpha = max(alpha, entry.score);
            } else {
                beta = max(beta, entry.score);
            }
        }
        if (alpha > beta) {
            bestMove = entry.move;
            return entry.score;
        }
    }
    
    if (depth == 0) {
        int score = quiesce(b, alpha, beta);
        return score;
    }

    std::vector<Move> moves;
    std::vector<MoveData> moveList;
    b.getToMove() == nWhite ? getLegalMoves<nWhite>(moves, b) : getLegalMoves<nBlack>(moves, b);

    Search::orderMoves(b, moves, moveList, ply);
    int bestVal = -MAX_VALUE;

    unsigned int loc = 0;

    for (MoveData mv : moveList) {
        loc++;
        auto time = chrono::high_resolution_clock::now();
        auto dur = time - info->startTime;
        if (2 * loc > moveList.size() && ((info->duration != 0 && 
                info->duration <=
                chrono::duration_cast<std::chrono::milliseconds>(dur).count())
                || info->stopped)) {
            info->stopped = true;
            return alpha;
        }

        Move m = mv.move;
        if (b.isLegal(m)) {
            b.makeMove(m);
            int searchVal = -negamax(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (searchVal > bestVal) {
                bestVal = searchVal;
                bestMove = m;
            }
            alpha = max(searchVal, alpha);

            if (alpha >= beta) {
                b.killerMoves[ply][1] = b.killerMoves[ply][0];
                b.killerMoves[ply][0] = m;
                break;
            }
        }
    }
    if (b.inCheck() && alpha == -MAX_VALUE) {
        bestVal = -MATE_VALUE + depth;
    }

    entry.score = bestVal;
    entry.depth = depth;
    entry.move = bestMove;

    if (bestVal <= oldAlpha) {
        entry.nodeType = HASH_BETA;
    } else if (bestVal >= beta) {
        entry.nodeType = HASH_ALPHA;
    } else {
        entry.nodeType = HASH_EXACT;
    }

    if (oldEntry.nodeType != HASH_EXACT && entry.nodeType == HASH_EXACT) {
        b.setTransTable(hashKey, entry);
    }
    if (!(oldEntry.nodeType == HASH_EXACT && entry.nodeType != HASH_EXACT)) {
        if (entry.depth >= oldEntry.depth) {
            b.setTransTable(hashKey, entry);
        }
    }

    return bestVal;
}


// Performs quiescence search on the given board
int Search::quiesce(Board &b, int alpha, int beta) {
    int stand_pat = b.boardScore();
    info->nodes++;
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    vector<Move> moves;
    vector<MoveData> moveList;

    b.getToMove() == nWhite ? getCaptures<nWhite>(moves, b) : getCaptures<nBlack>(moves, b);
    Search::orderMoves(b, moves, moveList, -1);
    for (MoveData md : moveList)  {
        Move m = md.move;
        if (b.isLegal(m)) {
            b.makeMove(m);
            int score = -quiesce(b, -beta, -alpha);
            b.unmakeMove();

            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    return alpha;
}


// Orders the moves in the given move list
void Search::orderMoves(Board& b, std::vector<Move>& moveList, std::vector<MoveData>& moveScores, int ply) {
    int hashKey = b.getZobrist() % TABLE_SIZE;
    for (Move m : moveList) {
        MoveData mv = MoveData(0, m);
        if (b.getTransTable(hashKey).move == m) {
            mv.score = 100000;
        } else if (m.isCapture()) {
            mv.score = PieceVals[b.getPiece(m.getTo())] -
                b.getPiece(m.getFrom());
        } else if (ply != -1) {
            if (m == b.killerMoves[ply][0]) {
                mv.score = 50;
            } else if (m == b.killerMoves[ply][1]) {
                mv.score = 49;
            }
        } else {
            mv.score = 0;
        }
        moveScores.push_back(mv);
    }

    std::sort(moveScores.begin(), moveScores.end(), sortMoves());
}
