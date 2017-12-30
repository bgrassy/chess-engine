#include <iostream>
#include <bitset>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "board.hpp"
#include "search.hpp"
#include <chrono>

using namespace Types;

int MAX_DEPTH = 4;

int stringToSquare(std::string s) {
    return 8 * (std::stoi(s.substr(1,1)) - 1) + ((int)s.at(0) - 97);
}

bool legalSquare(std::string s) {
    if ((int)s.at(0) - 97 >= 0 && (int)s.at(0) - 97 < 8) {
        if (std::stoi(s.substr(1,1)) - 1 >= 0 && std::stoi(s.substr(1,1)) - 1 < 8) {
            return true;
        }
    }
    return false;
}

void game(Board BOARD) {
    srand(time(NULL));
    std::string mov = "";
    while (mov != "q") {
        std::string start, end;
        int flags;
        bool goodMove = false;
        if (BOARD.inCheckmate()) {
            std::cout << "Checkmate!" << std::endl;
            exit(0);
        }
        if (mov == "q") {
            exit(0);
        }
        while (!goodMove) {
            std::cout << BOARD.checkCount() << std::endl;
            std::cout << "move: " << std::endl;
            std::getline(std::cin, mov);
            std::stringstream os(mov);
            if (os >> start) {
                if (os >> end) {
                    if (os >> flags) {
                        if (legalSquare(start) && legalSquare(end)) {
                            Move m(stringToSquare(start), stringToSquare(end), flags);
                            goodMove = BOARD.makeMove(m);
                        }
                    }
                }
            }
            if (!goodMove) {
                std::cout << "Illegal move!" << std::endl;
            }
        }
        std::cout << "computer's move..." << std::endl;
        BOARD.flushTransTable();
        Move best = alphabeta(BOARD, MAX_DEPTH, -MAX_VALUE, MAX_VALUE).move;
        std::cout << "move: " << std::endl;
        if (best.getStart() != 0 || best.getEnd() != 0) {
            BOARD.makeMove(best);
            std::cout << best.toString() << std::endl;
        } else {
            if (BOARD.inCheck()) {
                std::cout << "checkmate!" << std::endl;
            } else {
                std::cout << "stalemate!" << std::endl;
            }
            exit(0);
        }
    }
}

void gameTest(Board BOARD) {
    srand(time(NULL));
    std::string mov = "";
    while (mov != "q") {
        std::string start, end;
        int flags;
        bool goodMove = false;
        while (!goodMove) {
            std::cout << "move: " << std::endl;
            std::getline(std::cin, mov);
            std::stringstream os(mov);
            os >> start;
            os >> end;
            os >> flags;
            if (mov == "u") {
                BOARD.unmakeMove();
            } else {
                auto begin = std::chrono::high_resolution_clock::now();
                Move M(stringToSquare(start), stringToSquare(end), flags);
                std::cout << BOARD.legalMove(M) << std::endl;
                goodMove = BOARD.makeMove(M);
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns" << std::endl;
                if (!goodMove) {
                    std::cout << "Illegal move!" << std::endl;
                }
            }
        }
    }
}

U64 perft(Board& b, int depth) {
    U64 nodes = 0;
    if (depth == 0) {
        return 1;
    }
    auto moveList = b.getLegalMoves();
    for (Move m : moveList) {
        if (b.makeMove(m)) {
            nodes += perft(b, depth - 1);
            b.unmakeMove();
        }
    }
    return nodes;
}

int main() {
    initmagicmoves();
    Board BOARD;
    game(BOARD);
    return 0;
}
