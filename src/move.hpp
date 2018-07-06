#ifndef MOVE_HPP
#define MOVE_HPP

#include <iostream>

extern const std::string squareNames[65];

class Move {
    unsigned int move;  
public:
    // Constructs a new Move object holding given squares and flags.
    Move(unsigned int from, unsigned int to, unsigned int flags);

    // Overloads equal operator for Move objects
    bool operator==(const Move& other);

    friend std::ostream& operator<<(std::ostream& os, const Move& mv) {
        os << squareNames[mv.getFrom()] << " " << squareNames[mv.getTo()] << " " <<
            mv.getFlags();
        return os;
    }


    // Returns the starting square of the move
    unsigned int getFrom() const;

    // Returns the ending square of the move
    unsigned int getTo() const;

    // Returns the move's flags
    unsigned int getFlags() const;

    // Returns true iff a move is a capture
    bool isCapture() const;

    // Returns true iff a move is a promotion
    bool isPromotion() const;
};

#endif
