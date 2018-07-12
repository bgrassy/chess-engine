#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"
#include "movegen.hpp"

extern const int MAX_VALUE;
extern const int MATE_VALUE;
extern const int TABLE_SIZE;


struct MoveData {
    Move move;
    int score;

    MoveData(int score, Move m) {
        this->move = m;
        this->score = score;
    }
    
    MoveData() {
        this->move = Move();
        this->score = 0;
    }
};

struct sortMoves {
    bool operator()(MoveData const &a, MoveData const &b) { 
            return a.score > b.score;
    }
};

namespace Search {
    MoveData alphabeta(Board &b, int depth, int alpha, int beta);
    int quiesce(Board &b, int alpha, int beta);
    void orderMoves(Board& b, std::vector<Move>& moveList, std::vector<MoveData>& moveScores, int ply);
}
#endif /*SEARCH_HPP*/
