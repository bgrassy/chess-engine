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

using namespace types;

int stringToSquare(std::string s) {
    return 8 * (std::stoi(s.substr(1,1)) - 1) + ((int)s.at(0) - 97);
}

void game(board BOARD) {
    srand(time(NULL));
    std::string mov = "";
    while (mov != "q") {
        std::string start, end;
        int flags;
        bool goodMove = false;
        if (mov == "q") {
            exit(0);
        }
        while (!goodMove) {
            std::cout << "move: " << std::endl;
            std::getline(std::cin, mov);
            std::stringstream os(mov);
            os >> start;
            os >> end;
            os >> flags;
            move M(stringToSquare(start), stringToSquare(end), flags);
            std::cout << BOARD.legalMove(M) << std::endl;
            goodMove = BOARD.makeMove(M);
            if (!goodMove) {
                std::cout << "Illegal move!" << std::endl;
            }
            std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
            std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
        }
        std::cout << BOARD.boardScore() << std::endl;
        std::cout << "computer's move..." << std::endl;
        move best = alphabeta(BOARD, 4, -50000, 50000).second;
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
        std::cout << BOARD.boardScore() << std::endl;
        std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
        std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
    }
}

void gameTest(board BOARD) {
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
                move M(stringToSquare(start), stringToSquare(end), flags);
                std::cout << BOARD.legalMove(M) << std::endl;
                goodMove = BOARD.makeMove(M);
                if (!goodMove) {
                    std::cout << "Illegal move!" << std::endl;
                }
            }
            std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
            std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
            std::cout << std::bitset<64>(BOARD.getPieces(color::White, piece::King)) << '\n';
        }
    }
}

int main() {
    board BOARD;
    std::cout << "legal moves" << std::endl;
    std::vector<move> moves = BOARD.getLegalMoves();
    std::cout << moves.size() << std::endl;
    for (auto mov : moves) {
        std::cout << mov.toString() << std::endl;
    }
    game(BOARD);
    return 0;
}
