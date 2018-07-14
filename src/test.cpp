#include "board.hpp"
#include "search.hpp"
#include "uci.hpp"

long perft(Board& b, int depth, int MAX) {
    if (depth == 0) {
        return 1;
    }
    vector<Move> v;

    b.getToMove() == nWhite ? getAllMoves<nWhite>(v, b) :
        getAllMoves<nBlack>(v, b);

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

Move stringToMove(string s, Board& b) {
    vector<Move> moveList;     
    b.getToMove() == nWhite ? getAllMoves<nWhite>(moveList, b) :
        getAllMoves<nBlack>(moveList, b);

    for (Move m : moveList) {
        if (s == m.shortStr()) {
            return m;
        }
    }
    cout << "INVALID" << endl;
    return Move();

}

int main() {
    //string start = "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -";
    string start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    initBitboards();

    Board b = Board();
    b.printBoard();
    std::string line;
    std::string token;
    while (getline(cin, line)) {
        istringstream is(line);
        token.clear();
        is >> skipws >> token;

        if (token == "uci") {
            cout << "id name Engine" << endl;
            cout << "id author Brock Grassy" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            b.setPosition(start);
        } else if (token == "position") {
            is >> token;
            if (token == "startpos") {
                b.setPosition(start);
            } else if (token == "fen") {
                string fen;
                while (is >> token && token != "moves") {
                    fen += token + " ";
                }
                fen.pop_back();
                b.setPosition(fen);
                b.printBoard();
            }

            while (is >> token) {
                if (token != "moves") { // make move
                    Move m = stringToMove(token, b);
                    b.makeMove(m);
                }
            }
        } if (token == "go") {
            vector<Move> moveList;
            Move bestMove;

            for (int depth = 1; depth <= 5; depth++) {
                int nodes = 0;
                MoveData mt = Search::alphabeta(b, depth, -MAX_VALUE, MAX_VALUE, &nodes);
                bestMove = mt.move;
                cout << "info depth " << depth << " nodes " << nodes << " score ";
                cout << mt.score << " pv";
                b.printPV();
                cout << endl;
            }
            b.makeMove(bestMove);
            cout << endl;
            b.printBoard();
            cout << "bestmove " << bestMove.shortStr() << endl;
        }
    }

    /*
       Board b = Board();

       string start, end;
       int flags;
       vector<Move> goodMoves;
       b.printBoard();
       while (true) {
       if (goodMoves.size() == 0) {
       b.getToMove() == nWhite ? getAllMoves<nWhite>(goodMoves, b) :
       getAllMoves<nBlack>(goodMoves, b);
       }
       cout << b.getIsolatedPawns(nWhite) << endl;
       cout << "move: ";
       cin >> start >> end >> flags;
       int startSq = find(squareNames, squareNames+64, start) - squareNames;
       int endSq = find(squareNames, squareNames+64, end) - squareNames;
       Move m = Move(startSq, endSq, flags);
       if (find(goodMoves.begin(), goodMoves.end(), m) != goodMoves.end() &&
       b.isLegal(m)) {
       b.makeMove(Move(startSq, endSq, flags));
       } else {
       continue;
       }
       MoveData moveD;
       for (int depth = 1; depth <= SEARCH_DEPTH; depth++) {
       moveD = Search::alphabeta(b, depth, -MAX_VALUE, MAX_VALUE);
       }
       if (moveD.move == Move()) {
       cout << "no moves" << endl;
       cout << "checkmate!" << endl;
       break;
       }
       b.makeMove(moveD.move);
       cout << moveD.move.shortStr() << endl;
       cout << moveD.score << endl;
       cout << endl;
       b.printBoard();
       cout << b.getFEN() << endl;
       goodMoves.clear();
       }
       */

    return 0;

}
