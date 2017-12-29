#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"

struct MoveData {
    Move move;
    int score;
    MoveData(int score, Move move) {
        this->move = move;
        this->score = score;
    }
};

const int MAX_VALUE = 50000;
const int MATE_VALUE = 25000;
MoveData alphabeta(Board &b, int depth, int alpha, int beta);
int quiesce(Board &b, int alpha, int beta);
#endif /*SEARCH_HPP*/
