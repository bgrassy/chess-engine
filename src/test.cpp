#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "board.hpp"
#include "movegen.hpp"

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
        cout << slidingAttacksBB<nRook>(D4, b.getOccupied()) << endl;
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
        REQUIRE(b.attacked(b.getOccupied(), A1, nWhite) == false);
        REQUIRE(b.attacked(b.getOccupied(), B3, nWhite) == true);
        REQUIRE(b.attacked(b.getOccupied(), C4, nWhite) == false);
    }
}
