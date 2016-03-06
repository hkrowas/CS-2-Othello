#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <stdint.h>
#include "common.h"
#include "board.h"
using namespace std;



/**
 * Node: this struct represents a node of the game tree that holds information
 * about its parent, its board, and its ancestor, the node which, if chosen now,
 * can eventually lead to it.
 *
 */
struct Node 
{
    Node *ancestor;

    int8_t score;
    uint8_t level;
    uint8_t x;
    uint8_t y;

    Board board;
};



class Player {

public:
    Player(Side side);
    ~Player();
    
    Board board;
    Side side;

    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;

    int buildLevel(int start, int end, Node *tree);
};





#endif
