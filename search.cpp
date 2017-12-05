#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<double, move> alphabeta(board b, int depth, double alpha, double beta, int color) {
    double v;
    move bestMove(0, 0, 0);
    if (depth == 0) {
        return std::make_pair(color * b.boardScore(), bestMove);
    }
    v = -999;
    for (move m : b.getLegalMoves()) {
        if (b.makeMove(m)) {
            auto score = alphabeta(b, depth - 1, -beta, -alpha, -color);
            if (-score.first > v) {
                bestMove = m;
                v = -score.first;
            }
            alpha = std::max(v, alpha);
            b.unmakeMove();
            if (alpha >= beta) {
                break;
            }
        }
    }
    return std::make_pair(v, bestMove);
}
