#include "eightpuzzle.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <time.h>
using std::random_shuffle;
using namespace std;

//variant define
/* Your A* search implementation for 8 puzzle problem
NOTICE:
1. iniState in the input state, iniState.state is a 3x3 matrix, the "space" is indicated as -1, for example
	1 2 -1              1 2
	3 4 5   stands for  3 4 5
	6 7 8               6 7 8
2. moves is the output, it contains directions that transform initial state to final state. Here
	0 stands for up
	1 stands for down
	2 stands for left
	3 stands for right
   There might be several ways to understand "moving up/down/left/right". Here we define
   that "moving up" means to move other numbers up, not move "space" up. For example
   	1 2 -1              1 2 5
	3 4 5   move up =>  3 4 -1
	6 7 8               6 7 8
   This definition is actually consistent with where your finger moves to
    when you are playing 8 puzzle game.
*/
void AStarSearchFor8Puzzle(EightPuzzleState& iniState, vector<int>& moves)
{
	priority_queue<EightPuzzleState*, vector<EightPuzzleState*>, cmpLarge> openlist;
	vector<EightPuzzleState*> closelist;
	/*operations of openlist
    EightPuzzleState* state;
    openlist.push(state);   push a node into openlist
    openlist.pop();         remove a node with smallest g + h,
                            note this function does not return a value, thus state = openlist.pop() is illegal.
    state = openlist.top(); return a node with smallest g + h
    operations of closelist
    closelist.push_back(state); push a node into closelist*/
    //priority_queue<EightPuzzleState*, vector<EightPuzzleState*>, cmpLarge> temp_openlist;
    EightPuzzleState* current;
    EightPuzzleState* sonState;
    int command;
	int g_temp = 0;
    
	sonState = (EightPuzzleState*)malloc(sizeof(EightPuzzleState));
    
    openlist.push(&iniState);
    
    while(!openlist.empty())
    {
        current = openlist.top();
        if(checkFinalState(current))
            return reconstructPath(current, &iniState, moves);
        openlist.pop();
        closelist.push_back(current);
        for(command=0;command<4;command++)
        {
            if(runOneMove(current, sonState, command))
            {
				g_temp++;
				//sonState->g = g_temp;
                if(checkCloseList(sonState, closelist))
					continue;
				else if (!checkOpenList(sonState, openlist))
					openlist.push(sonState);
				else
					if (current->g + 1 >= sonState->g)
						continue;
					else
					{
						sonState->g = g_temp;
						sonState->h = h_caculation(sonState);
						sonState->preMove = command;
						sonState->preState = current;
					}
            }
        }
     }

}

// You may need the following code, but you may not revise it

/* Play 8 puzzle game according to "moves" and translate state from "iniState" to  "resultState"
   This function is used to check your AStarSearchFor8Puzzle implementation.
   It will return a state to indicate whether the vector moves will lead the final state.
   return 1 means moves are correct!
   return -1 means moves can not turn iniState to final state
   return -2 means moves violate game rule, see runOneMove();
   You should not revise this function.
*/
int runMoves(const EightPuzzleState* iniState, const vector<int>& moves)
{
	if (moves.size() == 0)
	{
		return -1;
	}
	//memcpy(&resultState[0][0], &iniState[0][0], sizeof(*iniState));
	EightPuzzleState currentState = *iniState;
	EightPuzzleState nextState;
	for (int i = 0; i < (int)moves.size(); ++i)
	{
		if (!runOneMove(&currentState, &nextState, moves[i]))
		{
			return -2;
		}
		currentState = nextState;
	}
	if (checkFinalState(&nextState))
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

bool checkMove(const EightPuzzleState* state, const int move,
	int& r_1, int& c_1, int& r_1_move, int& c_1_move)
{
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			if (state->state[r][c] == -1)
			{
				r_1 = r;
				c_1 = c;
			}
		}
	}

	switch (move)
	{
		//up
	case 0:
		r_1_move = r_1 + 1;
		c_1_move = c_1;
		break;
		//down
	case 1:
		r_1_move = r_1 - 1;
		c_1_move = c_1;
		break;
		//left
	case 2:
		c_1_move = c_1 + 1;
		r_1_move = r_1;
		break;
		//right
	case 3:
		c_1_move = c_1 - 1;
		r_1_move = r_1;
	}

	// if move out of boundary
	if (r_1_move < 0 || r_1_move > 2 || c_1_move < 0 || c_1_move > 2)
	{
		return false;
	}
	return true;
}

bool runOneMove(EightPuzzleState* preState, EightPuzzleState* nextState, const int move)
{
	// find the position of -1
	int r_1, c_1, r_1_move, c_1_move;
	*nextState = *preState;
	bool flag = checkMove(nextState, move, r_1, c_1, r_1_move, c_1_move);

	// if move out of boundary
	if (r_1_move < 0 || r_1_move > 2 || c_1_move < 0 || c_1_move > 2)
	{
		return false;
	}
	int v = nextState->state[r_1_move][c_1_move];
	nextState->state[r_1][c_1] = v;
	nextState->state[r_1_move][c_1_move] = -1;
	nextState->preState = preState;
	nextState->preMove = move;
	return true;
}

bool checkFinalState(const EightPuzzleState* resultState)
{
	const int finalState[3][3] = {{1,2,3},{4,5,6},{7,8,-1}};

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (finalState[i][j] != resultState->state[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

void generateState(EightPuzzleState* state, int nMoves)
{
	EightPuzzleState finalState;
	for (int i =1; i < 10; ++i)
	{
		finalState.state[(i-1)/3][(i-1)%3] = i;
	}
	finalState.state[2][2] = -1;
	EightPuzzleState preState, nextState;
	preState = finalState;
	srand((int)time(0));
	for (int i =0; i < nMoves; ++i)
	{
		int rdmove = rand()%4;
		runOneMove(&preState, &nextState, rdmove);
		preState = nextState;
	}
	*state = nextState;
}

void printMoves(EightPuzzleState* state, vector<int>& moves)
{
	cout << "Initial state " << endl;
	printState(state);
	EightPuzzleState preState, nextState;
	preState = *state;
	for (int i =0; i < (int)moves.size(); ++i)
	{
		switch (moves[i])
		{
		case 0:
			cout << " The " << i << "-th move goes up" << endl;
			break;
		case 1:
			cout << " The " << i << "-th move goes down" << endl;
			break;
		case 2:
			cout << " The " << i << "-th move goes left" << endl;
			break;
		case 3:
			cout << " The " << i << "-th move goes right" << endl;
		}

		runOneMove(&preState, &nextState, moves[i]);
		printState(&nextState);
		preState = nextState;
	}
}

void printState(EightPuzzleState* state)
{

	for (int i = 0; i < 3; ++i)
	{
		cout << state->state[i][0] << " " << state->state[i][1] << " " << state->state[i][2] << endl;
	}
	cout << "---------------" << endl;
}

int h_caculation(EightPuzzleState* preState)
{
    const int finalState[3][3] = {{1,2,3},{4,5,6},{7,8,-1}};
    int i,j;
    int h=0,sum=0;
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(preState->state[i][j]==-1)
            {
                sum = abs(i-2)+abs(j-2);
            }
            else
            {
                if(preState->state[i][j]!=finalState[i][j])
                {
                    sum = abs((preState->state[i][j]-1)/3-i)+abs((preState->state[i][j]-1)%3-j);
                }
            }
            h = h+sum;
        }
    }
    return h;
}

/*
if nextState in closelist return TRUE
else return FALSE
*/
bool checkCloseList(EightPuzzleState* nextState,vector<EightPuzzleState*> closelist)
{
    int length,j;
	length = closelist.size();
	for(j=0;j<length;j++)
	{
		if(nextState == closelist[j])
		{
			return true;
		}
	}
	return false;
}

/*
if nextState in closelist return TRUE
else return FALSE
*/
bool checkOpenList(EightPuzzleState* nextState,priority_queue<EightPuzzleState*, vector<EightPuzzleState*>, cmpLarge> openlist)
{
	priority_queue<EightPuzzleState*, vector<EightPuzzleState*>, cmpLarge> temp_openlist;
	EightPuzzleState* pState;
	temp_openlist = openlist;
	while (!temp_openlist.empty())
	{
		pState = temp_openlist.top();
		if (pState == nextState)
			return true;
	}
	return false;
}

void reconstructPath(EightPuzzleState* current, EightPuzzleState* iniState, vector<int>& moves)
{
	vector<int> temp_moves;
	int i, j, steps;
	i = 0; 
	steps = 0;
	while (current->preState != iniState)
	{
		temp_moves[i] = current->preMove;
		i++;
	}
	steps = temp_moves.size();
	for (j = 0; j < steps; j++)
	{
		moves[j] = temp_moves[steps-j-1];
	}
}