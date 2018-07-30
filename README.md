# chess-engine
A simple chess engine written in C++ that implements the UCI protocol. Currently uses alpha-beta pruning with LMR, null-move pruning, PVS, and other enhancements. Move evaluation is still rather rudimentary, but includes basic passed pawns, king safety, material evaluation, pawn structure, etc.

### To-do list:
1. Improve evaluation with regards to phases, iron out performance issues in endgame.
2. Add time management and other more advanced search features.

### Usage instructions:
The chess.exe file included in this repository can be imported and used in GUI interfaces such as [Arena](http://playwitharena.com).

Current magic moves implementation created by [Pradyumna Kannan](http://pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf).
