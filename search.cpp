#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<double, move> alphabeta(board b, int depth, double alpha, double beta, bool maximize) {
    double v;
    move bestMove(0, 0, 0);
    if (depth == 0) {
        return std::make_pair(b.boardScore(), bestMove);
    }
    if (maximize) {
        v = -999;
        for (move m : b.getLegalMoves()) {
            if (b.makeMove(m)) {
                auto score = alphabeta(b, depth - 1, alpha, beta, false);
                v = std::max(v, score.first);
                if (v > alpha) {
                    alpha = v;
                    bestMove = m;
                }
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
                auto score = alphabeta(b, depth - 1, alpha, beta, true);
                v = std::max(v, score.first);
                if (v < beta) {
                    bestMove = m;
                    beta = v;
                }
                b.unmakeMove();
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    return std::make_pair(b.boardScore(), bestMove);
}
