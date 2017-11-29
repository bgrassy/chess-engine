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

using namespace types;
typedef uint64_t U64;

void game(board BOARD) {
    srand(time(NULL));
    std::string mov = "";
    while (mov != "q") {
        int start, end, flags;
        bool goodMove = false;
        while (!goodMove) {
            std::cout << "move: " << std::endl;
            std::getline(std::cin, mov);
            std::stringstream os(mov);
            os >> start;
            os >> end;
            os >> flags;
            if (mov != "q") {
                move M(start, end, flags);
                std::cout << BOARD.legalMove(M) << std::endl;
                goodMove = BOARD.makeMove(M);
                std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
                std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
            }
        }
        std::cout << "computer's move..." << std::endl;
/*        std::cout << "legal moves" << std::endl;
        std::vector<move> moves = BOARD.getLegalMoves();
        std::cout << moves.size() << std::endl;
        for (auto mov : moves) {
            std::cout << mov.toString() << std::endl;
        }
        */
        std::vector<move> legal = BOARD.getLegalMoves();
        std::random_shuffle(legal.begin(), legal.end());
        while (legal.size() > 0) {
            move m = legal.back();
            bool valid = BOARD.makeMove(m); 
            legal.pop_back();
            std::cout << valid << std::endl;
            if (valid) {
                std::cout << m.toString() << std::endl;
                break;
            }
            if (!valid && legal.size() == 0) {
                std::cout << "checkmate!" << std::endl;
                exit(0);
            }
        }
        std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
        std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
    }
}
int main() {
    board BOARD;
    move m(50, 42, 0b0000);
    std::cout << std::bitset<64>(pawn_moves(BOARD.getPieces(color::Black, piece::Pawn), color::Black)) << std::endl;
    std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
    std::cout << BOARD.legalMove(m) << '\n';
    std::cout << "legal moves" << std::endl;
    std::vector<move> moves = BOARD.getLegalMoves();
    std::cout << moves.size() << std::endl;
    for (auto mov : moves) {
        std::cout << mov.toString() << std::endl;
    }
    game(BOARD);
    return 0;
}

