#include <algorithm>
#include "board.hpp"

double alphabeta(board b, int depth, double alpha, double beta, bool maximizing);

move getBestMove(board b, int depth);
