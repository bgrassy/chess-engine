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
    U64 x = BOARD.getPieces();
    std::cout << x << std::endl;
//    std::cout << std::bitset<64>(BOARD.getPieces()) << std::endl;
//    std::cout << std::bitset<64>(BOARD.getPieces(color::White)) << '\n';
    return 0;
}
