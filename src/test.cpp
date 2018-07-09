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
    initBitboards();

    string start, end;
    int flags;
    while (true) {
        b.printBoard();
        cout << "move: ";
        cin >> start >> end >> flags;
        int startSq = find(squareNames, squareNames+64, start) - squareNames;
        int endSq = find(squareNames, squareNames+64, end) - squareNames;
        Move m = Move(startSq, endSq, flags);
        if (b.isLegal(m)) {
            b.makeMove(Move(startSq, endSq, flags));
        } else {
            continue;
        }
        MoveData moveD = alphabeta(b, 5, -MAX_VALUE, MAX_VALUE);
        if (moveD.move == Move()) {
            cout << "checkmate!" << endl;
            break;
        }
        b.makeMove(moveD.move);
        if (moveD.score >= MATE_VALUE) {
            cout << "checkmate!" << endl;
            break;
        }
        cout << moveD.move << endl;
        cout << moveD.score << endl;
        cout << endl;
    }

    return 0;
}
