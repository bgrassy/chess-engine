#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <algorithm>
#include <utility>
#include "board.hpp"

std::pair<double, move> alphabeta(board b, int depth, double alpha, double beta, int color);

#endif /*SEARCH_HPP*/
