#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board.h"

#define MEMSIZE (750000000)
#define MEMLEN (MEMSIZE/sizeof(Node))
#define BRDSIZE (8)
#define SEARCH_DEPTH (7)

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

    Node *child;
    Node *sibling;

    int16_t score;
    uint8_t level;
    uint8_t x;
    uint8_t y;

    Side lastmove;
    Board board;
};

/**
 * Brain: this struct holds the contents of the players "brain". Most
 * important is the tree of moves that may happen from the current state.
 * However, this wrapper is used so that it may be extended to other fields,
 * such as current depth of the tree.
 */
struct Brain
{
    Node *tree;

    uint8_t bottomlevel;

    Brain();
    ~Brain();
};



class Player {

public:
    Player(Side side);
    ~Player();
    
    Board board;
    Side side;

    Brain brain;

    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;

    int buildLevel(int start, int end);
    int buildFirstLevel();

    int16_t minimax(Node *node, int8_t depth, bool maximizingPlayer);
    Node *findMinimax();
};





#endif
