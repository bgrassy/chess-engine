#include "move.hpp"

move::move(unsigned int s, unsigned int e, unsigned int f) {
    start = s; 
    end = e;
    flags = f;
    capture = (f >> 3) & 1;
    promotion = (f >> 2) & 1;
    type = f & 3;
}

int move::getStart() {
    return start;
}

int move::getEnd() {
    return end;
}

int move::getFlags() {
    return flags;
}

bool move::getCapture() {
    return capture;
}

bool move::getProm() {
    return promotion;
}

int move::getType() {
    return type;
}
