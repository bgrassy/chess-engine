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

std::string move::toString() {
    char sRank = start % 8 + 97;
    char eRank = end % 8 + 97;
    int sFile = start / 8 + 1;
    int eFile = end / 8 + 1;
    //std::string s = std::to_string(start);
    //s += " " + std::to_string(end); s += " " + std::to_string(flags);
    return sRank + std::to_string(sFile) + " " + eRank + std::to_string(eFile) + " " + std::to_string(flags)
        + " " + std::to_string(start);
}
