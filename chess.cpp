#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>
#include <set>
#include <sstream>
#include "board.hpp"

using namespace types;
typedef uint64_t U64;

void game(board BOARD) {
    std::string mov = "";
    while (mov != "q") {
        int start, end, flags;
        std::cout << "move: " << std::endl;
        std::getline(std::cin, mov);
        std::stringstream os(mov);
        os >> start;
        os >> end;
        os >> flags;
        if (mov != "q") {
            move M(start, end, flags);
            std::cout << BOARD.legalMove(M) << std::endl;
            BOARD.move(M);
            std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
            std::cout << std::bitset<64>(BOARD.getPieces(color::Black)) << '\n';
        }
    }
}
int main() {
    board BOARD;
    move m(50, 42, 0b0000);
    std::cout << std::bitset<64>(pawn_moves(BOARD.getPieces(color::Black, piece::Pawn), color::Black)) << std::endl;
    std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
    std::cout << BOARD.legalMove(m) << '\n';
    game(BOARD);
    return 0;
}

