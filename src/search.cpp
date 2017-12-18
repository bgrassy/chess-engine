#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<double, move> alphabeta(board &b, int depth, double alpha, double beta) {
    move bestMove(0, 0, 0);
    if (depth == 0) {
        return std::make_pair(b.boardScore(), bestMove);
//        return quiesce(b, alpha, beta, bestMove);
    }
    double best = -50000;
    for (move m : b.getLegalMoves()) {
        if (b.makeMove(m)) {
            auto score = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (-score.first >= beta) {
                return std::make_pair(beta, bestMove);
            }
            if (-score.first > best) {
                best = -score.first;
                bestMove = m;
                if (-score.first > alpha) {
                    alpha = -score.first;
                }
            }
        }
    }
    return std::make_pair(best, bestMove);
}

std::pair<double, move> quiesce(board &b, double alpha, double beta, move bestMove) {
    double stand_pat = b.boardScore();
    if (stand_pat >= beta) {
        return std::make_pair(stand_pat, bestMove);
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    for (move m : b.getLegalMoves()) {
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
