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

using namespace Types;

int MAX_DEPTH = 6;

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
        Move best = alphabeta(BOARD, MAX_DEPTH, -MAX_VALUE, MAX_VALUE).second;
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
                Move M(stringToSquare(start), stringToSquare(end), flags);
                std::cout << BOARD.legalMove(M) << std::endl;
                goodMove = BOARD.makeMove(M);
                if (!goodMove) {
                    std::cout << "Illegal move!" << std::endl;
                }
            }
        }
    }
}

int main() {
    initmagicmoves();
    Board BOARD;
    game(BOARD);
    return 0;
}
