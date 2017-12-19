#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"

const int MAX_VALUE = 50000;
const int MATE_VALUE = 25000;
std::pair<int, Move> alphabeta(Board &b, int depth, int alpha, int beta);
std::pair<int, Move> quiesce(Board &b, int alpha, int beta, Move bestMove);
#endif /*SEARCH_HPP*/
