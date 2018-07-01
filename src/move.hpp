#ifndef MOVE_HPP
#define MOVE_HPP

class Move {
    unsigned int move;  
public:
    // Constructs a new Move object holding given squares and flags.
    Move(unsigned int from, unsigned int to, unsigned int flags);

    // Overloads equal operator for Move objects
    bool operator==(const Move& other);

    // Returns the starting square of the move
    unsigned int getFrom();

    // Returns the ending square of the move
    unsigned int getTo();

    // Returns the move's flags
    unsigned int getFlags();

    // Returns true iff a move is a capture
    bool isCapture();

    // Returns true iff a move is a promotion
    bool isPromotion();
};

#endif
