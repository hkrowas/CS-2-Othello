#include "player.h"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    self.side = side;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    Move * return_move = new Move;          // return move
    // update board
    if(self.side == BLACK){
        self.board.doMove(opponentsMove, WHITE);
    }
    else{
        self.board.doMove(opponentsMove, BLACK);
    }
    // check if game is over
    if(self.board.isDone()){
        return NULL;        // if game is over, no move is possible
    }
    // now determine next move
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board.checkMove(Move(i, j), self.side);
        }
    }

    return NULL;
}
