#include "search.hpp"

const int MATE_VALUE = 25000;
const int MAX_VALUE = 50000;
const int TABLE_SIZE = 100000;
// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
MoveData Search::alphabeta(Board &b, int depth, int alpha, int beta) {
    int ply = SEARCH_DEPTH - depth;
    int oldAlpha = alpha;
    int hashKey = b.getZobrist() % TABLE_SIZE;
    HashEntry entry = b.getTransTable(hashKey);
    if (entry.nodeType != HASH_NULL && entry.depth >= depth) { // valid node
        if (entry.zobrist == b.getZobrist()) {
            if (entry.nodeType == HASH_EXACT) {
                return MoveData(entry.score, entry.move);
            } else if (entry.zobrist == HASH_ALPHA) {
                alpha = max(alpha, entry.score);
            } else {
                beta = max(beta, entry.score);
            }
        }
        if (alpha > beta) {
            return MoveData(entry.score, entry.move);
        }
    }
    
    Move bestMove;
    if (depth == 0) {
        int score = quiesce(b, alpha, beta);
        return MoveData(score, bestMove);
    }

    std::vector<Move> moves;
    std::vector<MoveData> moveList;
    b.getToMove() == nWhite ? getAllMoves<nWhite>(moves, b) : getAllMoves<nBlack>(moves, b);
    Search::orderMoves(b, moves, moveList, ply);
    int bestVal = -MAX_VALUE;
    for (MoveData mv : moveList) {
        Move m = mv.move;
        if (b.isLegal(m)) {
            b.makeMove(m);
            auto searchVal = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (-searchVal.score > bestVal) {
                bestVal = -searchVal.score;
                bestMove = m;
            }
            alpha = max(-searchVal.score, alpha);

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
    if (bestVal <= oldAlpha) {
        entry.nodeType = HASH_BETA;
    } else if (bestVal >= beta) {
        entry.nodeType = HASH_ALPHA;
    } else {
        entry.nodeType = HASH_EXACT;
    }

    entry.depth = depth;
    b.setTransTable(hashKey, entry);
    return MoveData(bestVal, bestMove);
}

int Search::quiesce(Board &b, int alpha, int beta) {
    int stand_pat = b.boardScore();
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    vector<Move> moveList;
    b.getToMove() == nWhite ? getCaptures<nWhite>(moveList, b) : getCaptures<nBlack>(moveList, b);
    for (Move m : moveList)  {
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

void Search::orderMoves(Board& b, std::vector<Move>& moveList, std::vector<MoveData>& moveScores, int ply) {
    int hashKey = b.getZobrist() % TABLE_SIZE;
    for (Move m : moveList) {
        MoveData mv = MoveData(0, m);
        if (b.getTransTable(hashKey).move == m) {
            mv.score = 100000;
        } else if (m.isCapture()) {
            mv.score = PieceVals[b.getPiece(m.getTo())] -
                b.getPiece(m.getFrom());
        } else if (m == b.killerMoves[ply][0]) {
            mv.score = 50;
        } else if (m == b.killerMoves[ply][1]) {
            mv.score = 49;
        } else {
            mv.score = 0;
        }
        moveScores.push_back(mv);
    }

    std::sort(moveScores.begin(), moveScores.end(), sortMoves());
}
