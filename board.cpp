#include <iostream>
#include "types.hpp"
#include "BB.hpp"
#include <stdint.h>
#include <vector>
#include <set>

typedef uint64_t U64;
const U64 Rank1 = 0xFF;
const U64 Rank2 = 0xFF00;
const U64 Rank3 = 0xFF0000;
const U64 Rank4 = 0xFF000000;
const U64 Rank5 = 0xFF00000000;
const U64 Rank6 = 0xFF0000000000;
const U64 Rank7 = 0xFF000000000000;
const U64 Rank8 = 0xFF00000000000000;
const U64 AFile = 0x0101010101010101;
const U64 BFile = 0x0202020202020202;
const U64 CFile = 0x0404040404040404;
const U64 DFile = 0x0808080808080808;
const U64 EFile = 0x1010101010101010;
const U64 FFile = 0x2020202020202020;
const U64 GFile = 0x4040404040404040;
const U64 HFile = 0x8080808080808080;

using namespace types;
using namespace std;
typedef uint64_t U64;
class board {
        bool white_move;
        U64 pieceBB[8];
        U64 lookup[64];
    public:
        board() {
            white_move = true;
            pieceBB[0] = 0x000000000000FFFF; // white pieces
            pieceBB[1] = 0xFFFF000000000000; // black pieces
            pieceBB[2] = 0x00FF00000000FF00; // pawns
            pieceBB[3] = 0x4200000000000042; // knights
            pieceBB[4] = 0x2400000000000024; // bishops
            pieceBB[5] = 0x8100000000000081; // rooks
            pieceBB[6] = 0x0800000000000008; // queens
            pieceBB[7] = 0x1000000000000010; // kings
            for (int i = 0; i < 64; i++) {
                lookup[i] = (U64)1 << i;
            }
        }

        
        // Returns a bitboard representing all the pieces on the board.
        U64 getPieces() {
            std::cout << "what the fuck" << std::endl;
            return 0;
        }

        // Takes a color. Returns a bitboard of all the pieces of that color.
        U64 getPieces(color c) {
            return pieceBB[(int)c];
        }

        // Takes a piece type. Returns a bitboard of all the pieces of that type.
        U64 getPieces(piece p) {
            return pieceBB[(int)p + 2];
        }

        // Takes a piece type and color. Returns a bitboard of all the pieces of that type and color.
        U64 getPieces(color c, piece p) {
            return pieceBB[(int)c] & pieceBB[(int)p + 2];
        }
        
        void invert() {
            for (int i = 0; i < 8; i++) {
                pieceBB[i] = flip180(pieceBB[i]);
            }
        }

        U64 flip180(U64 x) {
            U64 h1 = 0x5555555555555555;
            U64 h2 = 0x3333333333333333;
            U64 h4 = 0x0F0F0F0F0F0F0F0F;
            U64 v1 = 0x00FF00FF00FF00FF;
            U64 v2 = 0x0000FFFF0000FFFF;
            x = ((x >>  1) & h1) | ((x & h1) <<  1);
            x = ((x >>  2) & h2) | ((x & h2) <<  2);
            x = ((x >>  4) & h4) | ((x & h4) <<  4);
            x = ((x >>  8) & v1) | ((x & v1) <<  8);
            x = ((x >> 16) & v2) | ((x & v2) << 16);
            x = (x >> 32) | (x << 32);
            return x;
        }

        bool sameRank(unsigned int square1, unsigned int square2) {
            return (square1 >> 3) == (square2 >> 3);
        }

        bool sameFile(unsigned int square1, unsigned int square2) {
            return (square1 & 7) == (square2 & 7);
        }

        bool sameDiagonal(unsigned int square1, unsigned int square2) {
           return bishop_attacks(lookup[square1]) & bishop_attacks(lookup[square2]);
        }
        // Checks to see if there are any pieces in between the two squares. 
        bool inBetween(unsigned int square1, unsigned int square2) {
            if (square1 > square2) {
                int tmp = square1;
                square1 = square2;
                square2 = tmp;
            }
            if (sameRank(square1, square2)) { // in the same row
                for (unsigned int i = square1 + 1; i < square2; ++i) {
                    if (lookup[i] & getPieces()) {
                        return true;
                    }
                }
            } else if (sameFile(square1, square2)) { // in the same column
                for (unsigned int i = square1 + 8; i < square2; i+=8) {
                    if (lookup[i] & getPieces()) {
                        return true;
                    }
                }
            } else if (sameDiagonal(square1, square2)) {
                if ((square2 - square1) % 9 == 0) {
                    for (unsigned int i = square1 + 9; i < square2; i+=9) {
                        if (lookup[i] & getPieces()) {
                            return true;
                        }
                    }
                } else if ((square2 - square1) % 7 == 0) {
                    for (unsigned int i = square1 + 7; i < square2; i+=7) {
                        if (lookup[i] & getPieces()) {
                            return true;
                        }
                    }

                }
            }
            return false;
        } 

        bool legalMove(unsigned int square1, unsigned int square2) {
            U64 sq1 = lookup[square1];
            U64 sq2 = lookup[square2];
            if (inBetween(square1, square2)) {
                return false;
            }
            if (sq1 & getPieces(color::White)) {
                 if (sq2 & getPieces(color::White)) {
                     return false;
                 }
                 if (sq1 & getPieces(color::Black)) {
                     if ((sq2 & pawn_attacks(sq1) & getPieces(color::Black)) || (sq2 & pawn_moves(sq1))) {
                         return true;
                     }
                 } else if (sq1 & getPieces(piece::Knight)) {
                     if (sq2 & knight_attacks(sq1)) {
                         return true;
                     }
                 } else if (sq1 & getPieces(piece::Bishop)) {
                     if (sq2 & bishop_attacks(sq1)) {
                         return true;
                     }
                 } else if (sq1 & getPieces(piece::Rook)) {
                     if (sq2 & rook_attacks(sq1)) {
                         return true;
                     }
                 } else if (sq1 & getPieces(piece::Queen)) {
                     if (sq2 & queen_attacks(sq1)) {
                         return true;
                     }
                 } else if (sq1 & getPieces(piece::King)) {
                     if (sq2 & king_attacks(sq1)) {
                         return true;
                     }
                 }
            }
            return false;
        }

        bool inCheck() {
            if (getPieces(color::White, piece::King) & (pawn_attacks(getPieces(color::Black, piece::Pawn)) | 
                    knight_attacks(getPieces(color::Black, piece::Knight)))) {
                return true;
            } else if (getPieces(color::White, piece::King) & bishop_attacks(getPieces(color::Black, piece::Bishop))) {
                U64 b = getPieces(color::Black, piece::Bishop); 
                int count = 0;
                while (b > 0) {
                    if (b & 1) {
                        U64 bishop = lookup[count];
                        if (!inBetween(getPieces(color::White, piece::King), bishop)) {
                            return true;
                        }
                        b >>= 1;
                        ++count;
                    }
                }
            } else if (getPieces(color::White, piece::King) & rook_attacks(getPieces(color::Black, piece::Rook))) {
                U64 b = getPieces(color::Black, piece::Rook); 
                int count = 0;
                while (b > 0) {
                    if (b & 1) {
                        U64 rook = lookup[count];
                        if (!inBetween(getPieces(color::White, piece::King), rook)) {
                            return true;
                        }
                        b >>= 1;
                        ++count;
                    }
                }
            } else if (getPieces(color::White, piece::King) & queen_attacks(getPieces(color::Black, piece::Queen))) {
                U64 b = getPieces(color::Black, piece::Queen); 
                int count = 0;
                while (b > 0) {
                    if (b & 1) {
                        U64 queen = lookup[count];
                        if (!inBetween(getPieces(color::White, piece::King), queen)) {
                            return true;
                        }
                        b >>= 1;
                        ++count;
                    }
                }
            }
            return false;
        }

        // Assumes the square has a piece on it.
        color getColor(unsigned int square) {
            U64 b = lookup[square];
            if (b & getPieces(color::White)) {
                return color::White;
            }
            return color::Black;
        }

        piece getPiece(unsigned int square) {
            U64 b = lookup[square];
            for (int i = 0; i < 6; i++) {
                if (b & getPieces((piece) i)) {
                    return (piece) i;
                }
            }
            return piece::Pawn;
        }

        void move(unsigned int square1, unsigned int square2) {
            if (legalMove(square1, square2)) {
               piece p = getPiece(square1);  
               color c = getColor(square1);
            }
        }
};

