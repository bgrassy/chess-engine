#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"

std::pair<double, move> alphabeta(board &b, int depth, double alpha, double beta);
std::pair<double, move> quiesce(board &b, double alpha, double beta, move bestMove);
#endif /*SEARCH_HPP*/
