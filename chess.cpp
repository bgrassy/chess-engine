#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>
#include <set>
#include "board.hpp"

using namespace types;
typedef uint64_t U64;

int main() {
    board BOARD;
    move m(1, 16, color::White, piece::Knight);
    std::cout << std::bitset<64>(pawn_attacks(BOARD.getPieces(color::White, piece::Pawn))) << std::endl;
    std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
    std::cout << BOARD.legalMove(m) << '\n';
    return 0;
}

