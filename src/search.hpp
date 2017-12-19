#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"

const int MAX_VALUE = 50000;
const int MATE_VALUE = 25000;
std::pair<double, Move> alphabeta(Board &b, int depth, double alpha, double beta);
std::pair<double, Move> quiesce(Board &b, double alpha, double beta, Move bestMove);
#endif /*SEARCH_HPP*/
