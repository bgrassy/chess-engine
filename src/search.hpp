#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"
#include "movegen.hpp"

struct MoveData {
    Move move;
    int score;

    MoveData(int score, Move m) {
        this->move = m;
        this->score = score;
    }
};

extern const int MAX_VALUE;
extern const int MATE_VALUE;
MoveData alphabeta(Board &b, int depth, int alpha, int beta);
int quiesce(Board &b, int alpha, int beta);
#endif /*SEARCH_HPP*/
