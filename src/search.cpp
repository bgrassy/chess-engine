#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<double, Move> alphabeta(Board &b, int depth, double alpha, double beta) {
    Move bestMove;
    U64 hash = b.getHashVal();    
    int hashIndex = hash % 10000; 
    HashEntry h = b.transTable[hashIndex];
    if (depth == 0) {
        return std::make_pair(b.boardScore(), bestMove);
//        return quiesce(b, alpha, beta, bestMove);
    }
    double best = -MAX_VALUE;
    auto moveList = b.getLegalMoves();
    std::sort(moveList.begin(), moveList.end(), Board::compareTo);
    for (Move m : b.getLegalMoves()) {
        if (b.makeMove(m)) {
            auto score = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (-score.first >= beta) {
                b.insertTransTable(hashIndex, HashEntry(hash, bestMove, beta, depth, 1)); // fail-low
                return std::make_pair(beta, bestMove);
            }
            if (-score.first > best) {
                best = -score.first;
                bestMove = m;
                if (best > alpha) {
                    b.insertTransTable(hashIndex, HashEntry(hash, bestMove, beta, depth, 0)); // exact
                    alpha = best;
                }
            }
        }
    }
    if (b.inCheck() && best == -MAX_VALUE) {
        return std::make_pair(-MATE_VALUE - depth, bestMove);
    }
    b.transTable[hashIndex] = HashEntry(hash, bestMove, best, depth, 2); // fail-high 
    return std::make_pair(best, bestMove);
}

std::pair<double, Move> quiesce(Board &b, double alpha, double beta, Move bestMove) {
    double stand_pat = b.boardScore();
    if (stand_pat >= beta) {
        return std::make_pair(stand_pat, bestMove);
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    for (Move m : b.getLegalMoves()) {
        if (m.getCapture()) {
            if (b.makeMove(m)) {
                double score = -quiesce(b, -beta, -alpha, bestMove).first;
                b.unmakeMove();

                if (score >= beta) {
                    return std::make_pair(beta, bestMove);
                }
                if (score > alpha) {
                    alpha = score;
                    bestMove = m;
                }
            }
        }
    }
    return std::make_pair(alpha, bestMove);
}
