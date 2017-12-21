#include "search.hpp"

// Alpha beta search algorithm. Takes a board and a search depth, and finds the board score
// using an implementation of alpha beta and minimax.
std::pair<int, Move> alphabeta(Board &b, int depth, int alpha, int beta) {
    Move bestMove;
    U64 hash = b.getHashVal();    
    int hashIndex = hash % 10000; 
    // store our desired hash key
    int hashKey = 2;
    HashEntry h1 = b.transTableDepth[hashIndex];
    HashEntry h2 = b.transTableAlways[hashIndex];
    HashEntry entries[2] = {h1, h2};
    for (HashEntry h : entries) {
        if (h.depth >= depth && h.zobrist == hash) {
            if (h.type == 0) {
                return std::make_pair(h.score, h.bestMove);
            } else if ((h.type == 1) && h.score <= alpha) {
                return std::make_pair(alpha, h.bestMove);
            } else if ((h.type == 2) && h.score >= beta) {
                return std::make_pair(beta, h.bestMove);
            }
        }
    }
    if (depth == 0) {
        int score = b.boardScore();
        //int score = quiesce(b, alpha, beta);
        b.insertTransTable(hashIndex, HashEntry(hash, bestMove, score, depth, 0, false));
        return std::make_pair(b.boardScore(), bestMove);
    }
    auto moveList = b.getLegalMoves();
    std::sort(moveList.begin(), moveList.end(), [&b](Move a, Move c) {return b.compareTo(a, c);});
    for (Move m : moveList) {
        if (b.makeMove(m)) {
            auto score = alphabeta(b, depth - 1, -beta, -alpha);
            b.unmakeMove();
            if (-score.first >= beta) {
                b.insertTransTable(hashIndex, HashEntry(hash, bestMove, beta, depth, 1, false)); // fail-high
                return std::make_pair(beta, bestMove);
            }
            if (-score.first > alpha) {
                bestMove = m;
                alpha = -score.first;
                hashKey = 0;              
            }
        }
    }
    if (b.inCheck() && alpha == -MAX_VALUE) {
        alpha = -MATE_VALUE - depth;
    }
    b.insertTransTable(hashIndex, HashEntry(hash, bestMove, alpha, depth, hashKey, false)); 
    return std::make_pair(alpha, bestMove);
}

int quiesce(Board &b, int alpha, int beta) {
    int stand_pat = b.boardScore();
    if (stand_pat >= beta) {
        return stand_pat;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    auto moveList = b.getLegalMoves();
    std::sort(moveList.begin(), moveList.end(), [&b](Move a, Move c) {return b.compareTo(a, c);});
    for (Move m : moveList) {
        if (m.getCapture()) {
            if (b.makeMove(m)) {
                int score = -quiesce(b, -beta, -alpha);
                b.unmakeMove();
                if (score >= beta) {
                    return beta;
                }
                if (score > alpha) {
                    alpha = score;
                }
            }
        }
    }
    return alpha;
}
