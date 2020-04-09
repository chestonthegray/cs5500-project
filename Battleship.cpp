#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stack>
#include <array>
#include <unistd.h>
#include <string>
#include <iomanip>
#define MCW MPI_COMM_WORLD
using namespace std;
//global board dimensions
const int DIM = 10;

// struct that will hold individual ship information
struct ship
{
    int ID;
    int length;
    int attemps;
    int alive;
    string name;
};

// struct that will hold coordinate information used by the program
struct coordinates
{
    int x;
    int y;
    int valid;
    int hit;
};

// clears the board of any previously stored values
void clearBoard(int array[DIM][DIM])
{
    for(int i = 0; i < DIM; i++)
    {
        for(int j = 0; j < DIM; j++)
        {
            array[i][j] = -1;
        }
    }
}

// prints the board that holds the ships;
// This function is really only used for debugging purposes
void printBoard(int array[DIM][DIM])
{
    for(int i = 0; i < DIM; i++)
    {
        for(int j = 0; j < DIM; j++)
        {
            if(array[i][j] == -1)
            {
                cout << left << setw(5) << "'";
            }
            else
            {
                cout << left << setw(5) << array[i][j];
            }
            
        }
        cout << endl;
    }
}

// Prints the players board
void printPlayersBoard(int array[DIM][DIM])
{
    for(int k = 0; k < DIM; k++)
    {
        if(k==0)
        {
            cout << left << setw(5) << " ";
        }
        cout << left << setw(5) << k+1;
    }
    cout << endl;

    for(int i = 0; i < DIM; i++)
    {
        cout << left << setw(5) << char(i + 97);
        
        for(int j = 0; j < DIM; j++)
        {
            if(array[i][j] == -1)
            {
                cout << left << setw(5) << "-";
            }
            if(array[i][j] == 1)
            {
                cout << left << setw(5) << "O";
            }
            if(array[i][j] == 2)
            {
                cout << left << setw(5) << "X";
            }
            
            
        }
        cout << endl;
    }
}

// places a ship onto the board array and onto the startingboard array
void placeShip(int board[DIM][DIM], int startingBoard[DIM][DIM], ship current)
{
    // if this function tries more than 50 times to place a ship; it gives up and doesn't place it
    if(current.attemps > 50)
    {
        cout << "Couldn't find a valid location; didn't place the ship." << endl;
    }
    else
    {        
        // randomly determine if the ship will be placed horizontally or vertically
        int orientation = rand() % 2;

        // makes sure that the placement of the ship is valid
        bool valid = true;

        // vertical placement
        if(orientation)
        {
            // select a random starting point
            int X = rand() % DIM;
            int Y = rand() % (DIM - current.length + 1);

            // make sure there isn't anything in the way
            for(int j = 0; j < current.length; j++)
            {   
                // if there was something in the way the attempt is invalid
                if(board[Y + j][X] != -1)
                {
                    valid = false;
                    break;
                }
            }

            // invalid attempts are attempted once more
            if(!valid)
            {
                current.attemps++;
                placeShip(board, startingBoard, current);
            }
            // valid attempts means that the ship is placed successfully onto the board
            else
            {
                for(int j = 0; j < current.length; j++)
                {
                    board[Y + j][X] = current.ID;
                    startingBoard[Y + j][X] = current.ID;
                }
            }
        }
        //horizontal placement
        else
        {
            // select a random starting point
            int X = rand() % DIM;
            int Y = rand() % (DIM - current.length + 1);

            // make sure there isn't anything in the way
            for(int j = 0; j < current.length; j++)
            {
                // if there was something in the way the attempt is invalid
                if(board[X][Y + j] != -1)
                {
                    valid = false;
                    break;
                }
            }

            // invalid attempts are attempted once more
            if(!valid)
            {
                current.attemps++;
                placeShip(board, startingBoard, current);
            }
            // valid attempts means that the ship is placed successfully onto the board
            else
            {
                for(int j = 0; j < current.length; j++)
                {
                    board[X][Y + j] = current.ID;
                    startingBoard[X][Y + j] = current.ID;
                }
            }
        }
    }
}

// prints all the ships that have been sunk
void printSunken(ship *fleet, int fleetSize)
{

    cout << "Sunken Ships: " << endl;
    for(int i = 0; i < fleetSize; i++)
    {
        if(!fleet[i].alive)
        {
            cout << "-" << fleet[i].name << endl;
        }
    }
    cout << endl;
}

// updates the fleet with ships that are sunk/still afloat
void getSurvivors(int array[DIM][DIM], ship *fleet, int fleetSize)
{
    int temp = 0;
    for(int current = 0; current < fleetSize; current++)
    {
        //tag it as dead
        fleet[current].alive = 0;
        temp = 0;
        for(int i = 0; i < DIM; i++)
        {
            for(int j = 0; j < DIM; j++)
            {
                if(array[i][j] == current)
                {
                    //if it's found it gets tagged as alive
                    fleet[current].ID = current;
                    fleet[current].alive = 1;
                    temp = 1;
                    break;
                }
            }
            if(temp)
            {
                break;
            }
        }
    }
}

// if the fleet has no floating ships the game ends
bool gameOver(ship *fleet, int fleetSize)
{
    bool temp = false;
    for(int i = 0; i < fleetSize; i++)
    {
        if(fleet[i].alive)
        {
            temp = true;
        }
    }

    return temp;
}

// gets the coordinates for the next shot from the player
// there is probably an easier way to do this; but whatever, it works
coordinates obtainUserCoordinates(int playersBoard[DIM][DIM], int board[DIM][DIM], coordinates current)
{
    // initial coordinate is invalid until proven otherwise
    current.valid = 0;
    bool valid_x = false;
    bool valid_y = false;
    char X;
    int y;
    int x;

    // prompt the user for coordinates
    cout << "Enter the coordinates '(a-j) (1-10)' for your next shot: ";

    // keep the user here until he/she enters valid coordinates
    while(current.valid == 0)
    {   
        cin.clear();
        cin >> X;
        cin >> y;
        x = X;
        // validate the first coordinate
        if(x > 96 && x < 107)
        {
            valid_x = true;
            // convert the coordinate into terms the rest of the program understands
            // the values from (a-j) are converted into (0-9) (hurray for ascii value convertion)
            x = x - 97;
            current.x = x;
        }
        // validate the second coordinate
        if(y >= 1 && y <= 10)
        {
            valid_y = true;
            // convert the coordinate into terms the rest of the program understands
            // the values from (1-10) are converted into (0-9)
            y = y - 1;
            current.y = y;
        }
        // valid coordinates were entered
        if(valid_x && valid_y)
        {
            // Make sure the user isn't trying to shoot the same place twice
            if(playersBoard[current.x][current.y] != -1)
            {
                // politely inform the user that they're an idiot
                cout << "You've already shot here. Try somewhere else." << endl;
                cout << "Enter the coordinates '(a-j) (1-10)' for your next shot: ";
                current.valid = 0;
            }
            // determine if the coordinates result in a hit or a miss
            else
            {
                // hit
                if(board[current.x][current.y] != -1)
                {
                    cout << endl << "Hit!" << endl << endl;
                    current.hit = 1;

                    // update the player's board
                    playersBoard[current.x][current.y] = 2;

                    // remove that part of the ship from the board
                    // we'll check for a sunk ship later
                    board[current.x][current.y] = -1;
                }
                // miss
                else
                {
                    cout << endl << "Miss..." << endl << endl;
                    current.hit = 0;

                    // update the player's board
                    playersBoard[current.x][current.y] = 1;
                }
                current.valid = 1;
            }
        }
        // invalid coordinates were entered
        else
        {
            // prompt the user once again
            cout << "Invalid coordinates found. Try again." << endl;
            cout << "Enter the coordinates '(a-j) (1-10)' for your next shot: ";
        }
    }
    // return the current coordinate which contains whether the shot hit or missed
    return current;
}

int main(int argc, char **argv){

    // timer to measure how long the calculations after each turn take
    double t1;
    double t2;

    // timer to start how long the game takes
    double timerStart;
    double timerEnd;

    // an array that will hold the ships
    int startingBoard[DIM][DIM];

    // array that will hold the ships as they're deleted piece by piece by the user
    int board[DIM][DIM];

    // an array that will hold all of the shots fired by the user
    int shotsFired[DIM][DIM];

    // array of ships that need to be placed on the board:
    int fleetSize = 5;
    ship fleet[fleetSize];
    fleet[0] = ship{0, 5, 0, 1, "carrier"};
    fleet[1] = ship{1, 4, 0, 1, "battleship"};
    fleet[2] = ship{2, 3, 0, 1, "destroyer"};
    fleet[3] = ship{3, 3, 0, 1, "submarine"};
    fleet[4] = ship{4, 2, 0, 1, "patrol"};    

    //MPI world start info
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size); 

    // seed the rng
    srand(rank + 1 * time(0));

    //set up a clear board
    clearBoard(board);
    clearBoard(startingBoard);

    //set up a clear shotsFired array
    clearBoard(shotsFired);

    //place the ships
    for(int i = 0; i < fleetSize; i++)
    {
        placeShip(board, startingBoard, fleet[i]);
    }

    // // print the coordinates of the board:
    // printBoardIJ(board);
    // //print the board
    // cout << "Board after ship placements:" << endl;
    // printBoard(startingBoard);

    // START THE GAME
    if(rank == 0)
    {
        //start the game timer 
        timerStart = MPI_Wtime();

        cout << "Welcome to Battleship!" << endl;

        // initialize the game variables:
        bool playing = true;
        int shotCount = -1;
        coordinates currentShot = {0, 0, 0, fleetSize};

        // keep going until all of the ships have been sunk by the player
        while(playing)
        {
            //increment shot counter            
            shotCount++;
            // print revelant info to the player
            cout << "Current shot count: " << shotCount << endl;
            cout << "Current Board:" << endl;
            printPlayersBoard(shotsFired);

            // get the players next shot coordinates
            currentShot = obtainUserCoordinates(shotsFired, board, currentShot);

            // if the shot hits; get the survivors
            if(currentShot.hit)
            {
                getSurvivors(board, fleet, fleetSize);
            }

            // print the remaining ships to the console
            printSunken(fleet, fleetSize);

            // determine if the game should keep going or not
            playing = gameOver(fleet, fleetSize);
        }


        // Prints how long the game took and the final score
        timerEnd = MPI_Wtime();
        printBoard(startingBoard);
        cout << "Congratulations! You win! You've sunk all of the ships." << endl;
        cout << "Final shot count: " << shotCount << endl;
        cout << "Final time: "<< timerEnd-timerStart << endl;
    }
   
    MPI_Barrier(MCW);
    MPI_Finalize();

    return 0;
}