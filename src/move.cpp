class CMove {
    unsigned short move_data;
    public:
        // First four bits correspond to flags, next six correspond to start, last six correspond to end
        CMove(unsigned int from, unsigned int to, unsigned int flags) {
            move_data = ((flags & 0xf)<<12) | ((from & 0x3f)<<6) | (to & 0x3f);
        }

        unsigned int getFrom() const {return (move_data >> 6) & 0x3f;}
        unsigned int getTo() const {return move_data & 0x3f;}
        unsigned int getFlags() const {return (move_data >> 12) & 0xf;}

        bool isCapture() const {return (move_data >> 14) & 1;}
        bool isProm() const {return (move_data >> 15) & 1;}

        bool operator==(CMove other) const {return move_data == other.move_data;};
        bool operator!=(CMove other) const {return move_data != other.move_data;};
};
