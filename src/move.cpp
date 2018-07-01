#include "move.hpp"

Move::Move(unsigned int from, unsigned int to, unsigned int flags) {
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
}

bool Move::operator==(const Move& other) {
    return other.move == move;
}

unsigned int Move::getFrom() {
    return (move >> 6) & 0x3f;
}

unsigned int Move::getTo() {
    return move & 0x3f;
}

unsigned int Move::getFlags() {
    return (move >> 12) & 0xf;
}

bool Move::isCapture() {
    return (move >> 14) & 1;
}

bool Move::isPromotion() {
    return (move >> 15) & 1;
}
