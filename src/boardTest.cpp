#include "board.hpp"
#include <iostream>
int main() {
    CBoard b = CBoard();
    std::cout << b.getWhiteBishops() << std::endl;
    Bitboard::init();
    return 0;
}
