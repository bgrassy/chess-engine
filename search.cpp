#include "search.hpp"

double alphabeta(board b, int depth, double alpha, double beta, bool maximize) {
    double v;
    if (depth == 0) {
        return b.boardScore();
    }
    if (maximize) {
        v = -999;
        for (move m : b.getLegalMoves()) {
            if (b.makeMove(m)) {
                v = std::max(v, alphabeta(b, depth - 1, alpha, beta, false));
                alpha = std::max(alpha, v);
                b.unmakeMove();
                if (beta <= alpha) {
                    break;
                }
            }
        }
    } else {
        v = 999;
        for (move m : b.getLegalMoves()) {
            if (b.makeMove(m)) {
                v = std::min(v, alphabeta(b, depth - 1, alpha, beta, true));
                alpha = std::min(alpha, v);
                b.unmakeMove();
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    return v;
}

move getBestMove(board b, int depth) {
    double bestScore = -900.0;
    move bestMove(0, 0, 0);
    for (move m : b.getLegalMoves()) {
        if (b.makeMove(m)) {
            int score = alphabeta(b, depth, -999, 999, false);
            b.unmakeMove();
            if (score >= bestScore) {
                bestScore = score;
                bestMove = m;
            }
        }
    }
    return bestMove;
}
