//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "board.hpp"
#include "movegen.hpp"
/**
TEST_CASE("Get Pieces", "[classic]") {
    SECTION("Test Construction") {
        Board();
    }

    SECTION("Test Piece Locations") {
        Board b = Board();
        REQUIRE(b.getPieces(nKnight, nWhite) == (sqToBB[B1] | sqToBB[G1]));
        REQUIRE(b.getPieces(nBishop, nWhite) == (sqToBB[C1] | sqToBB[F1]));
        REQUIRE(b.getPieces(nRook, nWhite) == (sqToBB[A1] | sqToBB[H1]));
        REQUIRE(b.getPieces(nQueen, nWhite) == sqToBB[D1]);
        REQUIRE(b.getPieces(nKing, nWhite) == sqToBB[E1]);
        REQUIRE(b.getPieces(nKnight, nBlack) == (sqToBB[B8] | sqToBB[G8]));
        REQUIRE(b.getPieces(nBishop, nBlack) == (sqToBB[C8] | sqToBB[F8]));
        REQUIRE(b.getPieces(nRook, nBlack) == (sqToBB[A8] | sqToBB[H8]));
        REQUIRE(b.getPieces(nQueen, nBlack) == sqToBB[D8]);
        REQUIRE(b.getPieces(nKing, nBlack) == sqToBB[E8]);
        REQUIRE(b.getPieces(nKnight, nWhite) == b.getWhiteKnights());
        REQUIRE(b.getPieces(nBishop, nWhite) == b.getWhiteBishops());
        REQUIRE(b.getPieces(nRook, nWhite) == b.getWhiteRooks());
        REQUIRE(b.getPieces(nQueen, nWhite) == b.getWhiteQueens());
        REQUIRE(b.getPieces(nKing, nWhite) == b.getWhiteKing());
        REQUIRE(b.getPieces(nKnight, nBlack) == b.getBlackKnights());
        REQUIRE(b.getPieces(nBishop, nBlack) == b.getBlackBishops());
        REQUIRE(b.getPieces(nRook, nBlack) == b.getBlackRooks());
        REQUIRE(b.getPieces(nQueen, nBlack) == b.getBlackQueens());
        REQUIRE(b.getPieces(nKing, nBlack) == b.getBlackKing());
    }
}

TEST_CASE("Test Attacks", "[classic]") {
    SECTION("Test Pawn Attacks") {
        initBitboards();
        REQUIRE(pawnAttacks[nWhite][B2] == (sqToBB[C3] | sqToBB[A3]));
        REQUIRE(pawnAttacks[nWhite][A7] == (sqToBB[B8]));
        REQUIRE(pawnAttacks[nBlack][A7] == (sqToBB[B6]));
        REQUIRE(pawnAttacks[nBlack][C5] == (sqToBB[B4] | sqToBB[D4]));
        REQUIRE(pawnAttacks[nWhite][B8] == 0);
    }

    SECTION("Test Knight Attacks") {
        initBitboards();
        REQUIRE(knightAttacks[A1] == (sqToBB[B3] | sqToBB[C2]));
        REQUIRE(knightAttacks[C2] == (sqToBB[A1] | sqToBB[A3] 
                    | sqToBB[B4] | sqToBB[D4] | sqToBB[E3] | sqToBB[E1]));
        REQUIRE(knightAttacks[D4] == (sqToBB[C2] | sqToBB[E2] | sqToBB[F3]
                    | sqToBB[F5] | sqToBB[E6] | sqToBB[C6] | sqToBB[B5] |
                    sqToBB[B3]));
    }

    SECTION("Test King Attacks") {
        initBitboards();
        REQUIRE(kingAttacks[A1] == (sqToBB[B1] | sqToBB[A2] | sqToBB[B2]));
        REQUIRE(kingAttacks[H8] == (sqToBB[H7] | sqToBB[G8] | sqToBB[G7]));
        REQUIRE(kingAttacks[D4] == (sqToBB[D3] | sqToBB[D5] | sqToBB[E3]
                    | sqToBB[E4] | sqToBB[E5] | sqToBB[C3] | sqToBB[C4] |
                    sqToBB[C5]));
    }
}

TEST_CASE("Test Generation", "[classic]") {
    SECTION("Test Pawn Generation") {
        Board b = Board();
        initBitboards();
        vector<Move> v;
        getPawnMoves<nWhite>(v, b);
    }

    SECTION("Test Bishop Generation") {
        Board b = Board();
        initBitboards();
    }

    SECTION("Test Knight Generation") {
        Board b = Board();
        initBitboards();
        vector<Move> v;
        getMoves<nWhite, nKnight>(v, b);
    }
    SECTION("Test Attacks") {
        Board b = Board();
        initBitboards();
        REQUIRE(b.attacked(A1, nWhite) == false);
        REQUIRE(b.attacked(B3, nWhite) == true);
        REQUIRE(b.attacked(C4, nWhite) == false);
    }
    SECTION("Test All") {
        Board b = Board();
        initBitboards();
        string start, end;
        int flags;
        vector<Move> v;
        while (true) {
            cin >> start >> end >> flags;
            int startSq = find(squareNames, squareNames+64, start) - squareNames;
            int endSq = find(squareNames, squareNames+64, end) - squareNames;
            if (flags >= 16) {
                b.unmakeMove();
            } else {
                b.makeMove(Move(startSq, endSq, flags));
            }
            b.getToMove() == nWhite ? getPseudoLegalMoves<nWhite>(v, b) : getPseudoLegalMoves<nBlack>(v, b);
            for (Move m : v) {
                cout << m << endl;
                cout << b.isLegal(m) << endl;
            }
            v.clear();
        }
    }
}
*/


long perft(Board& b, int depth, int MAX) {
    if (depth == 0) {
        return 1;
    }
    vector<Move> v;

    b.getToMove() == nWhite ? getAllMoves<nWhite>(v, b) : getAllMoves<nBlack>(v, b);

    long count = 0;
    string fen = b.getFEN();
    for (Move m : v) {
        if (b.isLegal(m)) {
            b.makeMove(m);
            long curr = perft(b, depth - 1, MAX);
            count = count + curr;
            b.unmakeMove();
            if (fen != b.getFEN()) {
                cout << endl;
                cout << fen << endl;
                cout << b.getFEN() << endl;
                cout << m << endl;
                while (b.moveList.size() != 0) {
                    cout << b.moveList.top() << endl;
                    b.moveList.pop();
                }
                Board(fen).printBoard();
                cout << endl;
                b.printBoard();
                assert(false);
            }
            /*
            if (depth == MAX) {
                cout << m << " ";
                cout << curr << endl;
            }*/
        }
    }
    return count;
}

int main() {
    std::string fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    Board b = Board(fen);
    initBitboards();

    b.printBoard();

    for (int i = 1; i <= 4; i++) {
        long count = perft(b, i, i);
        cout << count << endl;
    }

    return 0;
}
