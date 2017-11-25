#include "types.hpp"

using namespace types;
class move {
    public:
        int start;
        int end;
        piece p;
        color c;
        move(int start, int end, color C, piece P);
};
