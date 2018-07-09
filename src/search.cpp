#include "search.hpp"

const int MATE_VALUE = 25000;
const int MAX_VALUE = 50000;
// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
MoveData alphabeta(Board &b, int depth, int alpha, int beta) {
    Move bestMove;
    if (depth == 0) {
        int score = b.boardScore();
        return MoveData(score, bestMove);
    }

    std::vector<Move> moveList;
    b.getToMove() == nWhite ? getAllMoves<nWhite>(moveList, b) : getAllMoves<nBlack>(moveList, b);
    //std::sort(moveList.begin(), moveList.end(), [&b](Move a, Move c) {return b.compareTo(a, c);});
    for (Move m : moveList) {
        if (b.isLegal(m)) {
            b.makeMove(m);
            auto searchVal = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (-searchVal.score >= beta) {
                return MoveData(beta, bestMove);
            }
            if (-searchVal.score > alpha) {
                bestMove = m;
                alpha = -searchVal.score;
            }
        }
    }
    if (b.inCheck() && alpha == -MAX_VALUE) {
        alpha = -MATE_VALUE - depth;
    }
    return MoveData(alpha, bestMove);
}
