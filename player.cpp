#include "player.h"


using namespace std;

/**
 * Brain: initializer for the "Brain" class (see `player.h')
 */
Brain::Brain()
{
    this->tree = new Node [(int)(MEMSIZE/sizeof(Node))];
}

Brain::~Brain()
{
    delete [] this->tree;
}




/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    this->side = side;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}



inline void initNode(Node &current, Node *ancestor, uint8_t level, int8_t score,
        Board board)
{
    current.ancestor = ancestor;
    current.level = level;
    current.score = score;
    current.board = board;
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
    Move * return_move = new Move(0, 0);          // return move
    // update board
    if(this->side == BLACK){
        this->board.doMove(opponentsMove, WHITE);
    }
    else{
        this->board.doMove(opponentsMove, BLACK);
    }

    // check if game is over
    if(!this->board.hasMoves(this->side)){
        return NULL;        // if game is over, no move is possible
    }


    //// Allocate space for our tree:
    //Node *tree = new Node [(int)(MEMSIZE/sizeof(Node))];
    int start = 0, end = 1, newend;

    // Construct the first node
    initNode(this->brain.tree[0], NULL, 0, 0, this->board);

    // Fill the "tree"
    for(int i = 0; i < SEARCH_DEPTH; i++)
    { 
        newend = this->buildLevel(start, end, this->brain.tree);
        if(!newend)
        {
            break; // If we've run out of memory
        }
        start = end;
        end = newend;
    }

    return_move->x = this->brain.tree[1].x;
    return_move->y = this->brain.tree[1].y;


    //delete [] tree;

    this->board.doMove(return_move, this->side);
    return return_move;
}


/**
 * buildLevel: This function reads through a specified range of nodes in the
 * tree (intended to be all of the nodes in a particular level) and adds all of
 * their children to the end of the tree after end. This effectively builds the
 * next level into our data structure.
 *
 * return: It returns the index which is just past the last node added so that
 * this value can be subsequently be used on the next level of depth in the
 * tree.
 */
int Player::buildLevel(int start, int end, Node *tree)
{
    int idx, outidx, i, j;
    Board currBrd, newBrd;
    Move move (0, 0);
    int8_t level;
    
    for(idx = start, outidx = end; idx < end; idx++)
    {
        level = this->brain.tree[idx].level;
        currBrd = this->brain.tree[idx].board; // fetch the board

        for(i = 0; i < BRDSIZE; i++)
        {
            for(j = 0; j < BRDSIZE; j++)
            {
                move.x = i;
                move.y = j;
                if(currBrd.checkMove(&move, this->side))
                {
                    newBrd = currBrd;
                    newBrd.doMove(&move, this->side);
                    // Come back here to implement heuristic
                    initNode(this->brain.tree[outidx], NULL, level+1, 0,
                             newBrd);

                    if(!level) // If level is zero we just made the first level
                    {
                        this->brain.tree[outidx].ancestor = 
                        &(this->brain.tree[outidx]);
                        this->brain.tree[outidx].x = i;
                        this->brain.tree[outidx].y = j;
                    } else {
                        this->brain.tree[outidx].ancestor = 
                        this->brain.tree[idx].ancestor;
                    }
                    outidx++;
                    if(outidx >= MEMLEN)
                    {
                        cerr << "OUT OF MEMORY!" << endl;
                        return 0;
                    }
                }
            }
        }
    }
    
    return outidx;
}
