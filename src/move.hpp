#include <iostream>
#include <string>
class Move {
        unsigned int start;
        unsigned int end;
        unsigned int flags;
        bool capture;
        bool promotion;
        int type;
    public:
        Move();
        Move(unsigned int start, unsigned int end, unsigned int flags);
        int getStart() const;
        int getEnd() const;
        int getFlags() const;
        bool getCapture() const;
        bool getProm() const;
        int getType() const;
        std::string toString() const;
        bool equals(const Move& a) const;
};
