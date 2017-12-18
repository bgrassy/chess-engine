#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<double, move> alphabeta(board &b, int depth, double alpha, double beta) {
    move bestMove(0, 0, 0);
    if (depth == 0) {
        return std::make_pair(b.boardScore(), bestMove);
        //return quiesce(b, alpha, beta, bestMove);
    }
    double best = -50000;
    if (depth == 4) {
        std::cout << b.getPieces(piece::Rook) << std::endl;
    }
    for (move m : b.getLegalMoves()) {
        U64 temp[8] = {b.pieceBB[0], b.pieceBB[1], b.pieceBB[2], b.pieceBB[3],
                        b.pieceBB[4], b.pieceBB[5], b.pieceBB[6], b.pieceBB[7]};        
        if (depth == 1 && m.getStart() == 0 && m.getEnd() == 16) {
            int hoho = 5;
        }
        if (b.makeMove(m)) {
            auto score = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            for (int i = 0; i < 8; i++) {
                if (temp[i] != b.pieceBB[i] && depth == 1) {
                    std::cout << "ohno" << std::endl;
                    std::cout << i << std::endl;
                    std::cout << b.pieceBB[i] << std::endl;
                    std::cout << temp[i] <<std::endl;
                    for (int i = 0; i < 7; i++) {
                        std::cout << b.moveList.at(i).toString() << std::endl;
                    }
                    std::cout << "move" << std::endl;
                    std::cout << m.toString() << std::endl;
                    break;
                }
            }
            if (-score.first >= beta) {
                return std::make_pair(beta, bestMove);
            }
            if (-score.first > best) {
                best = -score.first;
                bestMove = m;
                if (-score.first > alpha) {
                    alpha = -score.first;
                }
            }
        }
    }
    return std::make_pair(best, bestMove);
}

std::pair<double, move> quiesce(board &b, double alpha, double beta, move bestMove) {
    double stand_pat = b.boardScore();
    if (stand_pat >= beta) {
        return std::make_pair(stand_pat, bestMove);
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    for (move m : b.getLegalMoves()) {
        if (m.getCapture()) {
            if (b.makeMove(m)) {
                double score = -quiesce(b, -beta, -alpha, bestMove).first;
                b.unmakeMove();

                if (score >= beta) {
                    return std::make_pair(beta, bestMove);
                }
                if (score > alpha) {
                    alpha = score;
                    bestMove = m;
                }
            }
        }
    }
    return std::make_pair(alpha, bestMove);
}

/*std::pair<double, move> alphabeta(board b, int depth, double alpha, double beta) {
  move bestMove(0, 0, 0);
  if (depth == 0) {
  return std::make_pair(b.boardScore(), bestMove);
  }
  double max = -32767;
  for (move m : b.getLegalMoves()) {
  if (b.makeMove(m)) {
  auto score = alphabeta(b, depth - 1, -beta, -alpha);
  if (-score.first > max) {
  max = -score.first;
  bestMove = m;
  }
  b.unmakeMove();
  }
  }
  return std::make_pair(alpha, bestMove);
  }*/
