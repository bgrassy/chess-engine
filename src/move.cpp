#include "move.hpp"

Move::Move() {
    start = 0;
    end = 0;
    flags = 0;
    capture = false;
    promotion = false;
    type = 0;
}

Move::Move(unsigned int s, unsigned int e, unsigned int f) {
    start = s; 
    end = e;
    flags = f;
    capture = (f >> 2) & 1;
    promotion = (f >> 3) & 1;
    type = f & 3;
}

int Move::getStart() const {
    return start;
}

int Move::getEnd() const {
    return end;
}

int Move::getFlags() const {
    return flags;
}

bool Move::getCapture() const {
    return capture;
}

bool Move::getProm() const {
    return promotion;
}

int Move::getType() const {
    return type;
}

std::string Move::toString() const {
    char sRank = start % 8 + 97;
    char eRank = end % 8 + 97;
    int sFile = start / 8 + 1;
    int eFile = end / 8 + 1;
    return sRank + std::to_string(sFile) + " " + eRank + std::to_string(eFile) + " " + std::to_string(start) + " " 
        + std::to_string(end) + " " + std::to_string(flags);
}

bool Move::equals(const Move& a) const {
    return (a.getStart() == getStart()) && (a.getEnd() == getEnd()) && (a.getFlags() == getFlags());
}
