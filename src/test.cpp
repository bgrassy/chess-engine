#include "board.hpp"
#include "search.hpp"

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
    Board b = Board();

    string start, end;
    int flags;
    vector<Move> goodMoves;
    b.printBoard();
    while (true) {
        if (goodMoves.size() == 0) {
            b.getToMove() == nWhite ? getAllMoves<nWhite>(goodMoves, b) : getAllMoves<nBlack>(goodMoves, b);
        }
        cout << "move: ";
        cin >> start >> end >> flags;
        int startSq = find(squareNames, squareNames+64, start) - squareNames;
        int endSq = find(squareNames, squareNames+64, end) - squareNames;
        Move m = Move(startSq, endSq, flags);
        if (find(goodMoves.begin(), goodMoves.end(), m) != goodMoves.end() && b.isLegal(m)) {
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
        cout << moveD.move << endl;
        cout << moveD.score << endl;
        cout << endl;
        b.printBoard();
        cout << b.getFEN() << endl;
        goodMoves.clear();
    }

    return 0;
}
