#include "board.h"
#include "stdlib.h"


/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}

 
/*
 * Returns true if the game is finished; false otherwise. The game is finished 
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == NULL) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y )) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A NULL move means pass.
    if (m == NULL) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}



/**
 * heuristic: A very basic heuristic for weighting different boards. Polarized
 * such that positive is better for black.
 */
int16_t Board::heuristic()
{
    int base = this->countBlack() - this->countWhite();
    int sign = (base != 0 ? abs(base)/base : 0);

    int ret = (this->hasMoves(WHITE) || this->hasMoves(BLACK)) ? base : base +
        sign*WINSC;

    if(this->countBlack() + this->countWhite() > NEAREND){   // if near end, just count stones
        ret = this->countBlack() - this->countWhite();
    }
    else{
        //check corners
        ret += this->get(BLACK, 0, 0) ? CORNSCR : 0;
        ret -= this->get(WHITE, 0, 0) ? CORNSCR : 0;

        ret += this->get(BLACK, 7, 0) ? CORNSCR : 0;
        ret -= this->get(WHITE, 7, 0) ? CORNSCR : 0;
        
        ret += this->get(BLACK, 0, 7) ? CORNSCR : 0;
        ret -= this->get(WHITE, 0, 7) ? CORNSCR : 0;
        
        ret += this->get(BLACK, 7, 7) ? CORNSCR : 0;
        ret -= this->get(WHITE, 7, 7) ? CORNSCR : 0;


        // Penalize spaces near corners:

        ret -= this->get(BLACK, 1, 0) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 1, 0) ? ADJCORNSCR : 0;

        ret -= this->get(BLACK, 0, 1) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 0, 1) ? ADJCORNSCR : 0;
        
        ret -= this->get(BLACK, 1, 1) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 1, 1) ? ADJCORNSCR : 0;

        ////////////////////////////////////////////
        
        ret -= this->get(BLACK, 0, 6) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 0, 6) ? ADJCORNSCR : 0;

        ret -= this->get(BLACK, 1, 7) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 1, 7) ? ADJCORNSCR : 0;
        
        ret -= this->get(BLACK, 1, 6) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 1, 6) ? ADJCORNSCR : 0;

        ////////////////////////////////////////////
        
        ret -= this->get(BLACK, 6, 0) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 6, 0) ? ADJCORNSCR : 0;

        ret -= this->get(BLACK, 7, 1) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 7, 1) ? ADJCORNSCR : 0;
        
        ret -= this->get(BLACK, 6, 1) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 6, 1) ? ADJCORNSCR : 0;

        ////////////////////////////////////////////
        
        ret -= this->get(BLACK, 6, 7) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 6, 7) ? ADJCORNSCR : 0;

        ret -= this->get(BLACK, 7, 6) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 7, 6) ? ADJCORNSCR : 0;
        
        ret -= this->get(BLACK, 6, 6) ? ADJCORNSCR : 0;
        ret += this->get(WHITE, 6, 6) ? ADJCORNSCR : 0;

        // Check edge spaces

        for(int i = 1; i < 7; i++){
            ret += this->get(BLACK, i, 0) ? EDGESCR : 0;
            ret -= this->get(WHITE, i, 0) ? EDGESCR : 0;
        }
        for(int i = 1; i < 7; i++){
            ret += this->get(BLACK, 0, i) ? EDGESCR : 0;
            ret -= this->get(WHITE, 0, i) ? EDGESCR : 0;
        }
        for(int i = 1; i < 7; i++){
            ret += this->get(BLACK, i, 7) ? EDGESCR : 0;
            ret -= this->get(WHITE, i, 7) ? EDGESCR : 0;
        }
        for(int i = 1; i < 7; i++){
            ret += this->get(BLACK, 7, i) ? EDGESCR : 0;
            ret -= this->get(WHITE, 7, i) ? EDGESCR : 0;
        }
    }
    return((int16_t)ret);
}







