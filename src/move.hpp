#include <iostream>
#include <string>
class move {
        unsigned int start;
        unsigned int end;
        unsigned int flags;
        bool capture;
        bool promotion;
        int type;
    public:
        move(unsigned int start, unsigned int end, unsigned int flags);
        int getStart();
        int getEnd();
        int getFlags();
        bool getCapture();
        bool getProm();
        int getType();
        std::string toString();
};
