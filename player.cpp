#include "player.h"
#include <map>

using namespace std;

/**
 * Brain: initializer for the "Brain" class (see `player.h')
 */
Brain::Brain()
{
    this->tree = new Node [(int)(MEMSIZE/sizeof(Node))];
    this->bottomlevel = 0;
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


inline Side enemyof(Side side)
{
    return (side == BLACK ? WHITE : BLACK);
}


inline void initNode(Node &current, Node *ancestor, uint8_t level, int8_t score,
        Board board, Side lastmove)
{
    current.ancestor = ancestor;
    current.level = level;
    current.score = score;
    current.board = board;
    current.lastmove = lastmove;
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
    initNode(this->brain.tree[0], NULL, 0, 0, this->board, enemyof(this->side));

    this->brain.bottomlevel = 0;

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
        this->brain.bottomlevel++;
    }


    Node *return_node = this->findMinimax();

    return_move->x = return_node->ancestor->x;
    return_move->y = return_node->ancestor->y;


    // Reset our tree so that it does not confuse our minimax method.
    for(unsigned int i = 0; i < MEMLEN; i++)
    {
        this->brain.tree[i].level = 127;
    }

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

    // Sign to account for the polarity of our heuristic. See `board.cpp'
    int8_t sign = (this->side == BLACK ? 1 : -1);
    int8_t score;

    Side currSide;
    
    for(idx = start, outidx = end; idx < end; idx++)
    {
        level = this->brain.tree[idx].level;
        currBrd = this->brain.tree[idx].board; // fetch the board


        currSide = enemyof(this->brain.tree[idx].lastmove);
        if(!currBrd.hasMoves(currSide))
        {
            currSide = enemyof(currSide);
        }

        if(!currBrd.hasMoves(currSide)) // In this case someone must have won we
                                        // need to just copy this state down so 
                                        // it gets properly compared with other 
                                        // alternatives.
        {
            newBrd = currBrd;

            // Use our heuristic:
            score = sign*(newBrd.heuristic());    
            
            initNode(this->brain.tree[outidx], NULL, level+1, score,
                     newBrd, currSide);

            if(!level) // If level is zero we just made a node in the first level
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
            if((unsigned int)outidx >= MEMLEN)
            {
                cerr << "OUT OF MEMORY!" << endl;
                return 0;
            }
        } else {
            for(i = 0; i < BRDSIZE; i++)
            {
                for(j = 0; j < BRDSIZE; j++)
                {
                    move.x = i;
                    move.y = j;
                    if(currBrd.checkMove(&move, currSide))
                    {
                        newBrd = currBrd;
                        newBrd.doMove(&move, currSide);

                        // Use our heuristic:
                        score = sign*(newBrd.heuristic()); 
                        
                        initNode(this->brain.tree[outidx], NULL, level+1, score,
                                 newBrd, currSide);

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
                        if((unsigned int)outidx >= MEMLEN)
                        {
                            cerr << "OUT OF MEMORY!" << endl;
                            return 0;
                        }
                    }
                }
            }
        }
    }
    
    return outidx;
}




/**
 * findMinimax: This function finds the minimum gain of any move by looking
 * at the ancestor of each node at the lowest level.
 * 
 * return: It returns the ancestor node of the minimax brach.
*/
Node * Player::findMinimax(){
    Node * curr_ancestor = NULL; // One should always be an 
                                                // ancestor if we have valid 
                                                // moves
    Node * max_ancestor = &this->brain.tree[1];
    int branch_min;             // min of the branch
    int curr_score;

    int minimax = -10000;

    std::map<Node *, int> branchmins;
    std::map<Node *, int>::iterator read;
    

    for(int i = 0; i < (int) MEMLEN;){
        if(this->brain.tree[i].level == this->brain.bottomlevel){
            branch_min = 10000;
            curr_ancestor = this->brain.tree[i].ancestor;

            while((this->brain.tree[i].level == this->brain.bottomlevel) && 
                  (this->brain.tree[i].ancestor == curr_ancestor))
            {
                curr_score = this->brain.tree[i].score;
                
                if(branch_min > curr_score);
                {
                    branch_min = curr_score;
                }
                i++;
            }

            branchmins[curr_ancestor] = branch_min;
        } else
        {
            i++;
        }
    }
    for(read = branchmins.begin(); read != branchmins.end(); read++)
    {
        if(read->second > minimax)
        {
            minimax = read->second;
            max_ancestor = read->first;
        } 
    }
    return max_ancestor;
}

//Node * Player::findMinimax(){
//    Node * curr_ancestor = NULL;
//    Node * min_ancestor = &(brain.tree[1]);
//    int branch_min = -10000;             // min of the branch
//    int current_min = -1000000;
//    for(int i = 0; i < (int) MEMLEN; i++){
//        if(this->brain.tree[i].level == this->brain.bottomlevel){
//            // change branch
//            if(this->brain.tree[i].ancestor != curr_ancestor){
//                // if better branch
//                if(branch_min > current_min){
//                    current_min = branch_min;
//                    if(curr_ancestor != NULL){          // only on first iteration
//                        min_ancestor = curr_ancestor;
//                    }
//                }
//                curr_ancestor = this->brain.tree[i].ancestor;
//                branch_min = this->brain.tree[i].score;
//            }
//            else{
//                if(this->brain.tree[i].score < branch_min){
//                    branch_min = this->brain.tree[i].score;
//                }
//            }
//        }
//    }
//    return min_ancestor;
//}
