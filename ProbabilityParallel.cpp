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

// used to double make sure coordinates were matching
void printBoardIJ(int array[DIM][DIM])
{
    for(int i = 0; i < DIM; i++)
    {
        for(int j = 0; j < DIM; j++)
        {
            cout << i << "," << j << "  ";
        }
        cout << endl;
    }
}
// prints the board that holds the ships;
void printBoard(int array[DIM][DIM])
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

// prints all the ships that have been sunk
void printSurvivors(ship *fleet, int fleetSize)
{

    cout << "Surviving Ships: " << endl;
    for(int i = 0; i < fleetSize; i++)
    {
        if(fleet[i].alive)
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
int gameOver(ship *fleet, int fleetSize)
{
    int temp = 0;
    for(int i = 0; i < fleetSize; i++)
    {
        if(fleet[i].alive)
        {
            temp = 1;
        }
    }

    return temp;
}

// gets the coordinates for the next shot from the player
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


// gets the probability density for a specific square on the board
int getProbability(int board[DIM][DIM], ship *fleet, int fleetSize, int index)
{
    int probabilty = -1;
    // x coordinate of the square
    int x = index / DIM;
    // y coordinate of the square
    int y = index % DIM;

    // basically the calculation needs to step through the following process:
    // the ship has a length; meaning it consists of 'length' number of segments
    // each 'segment' of the ship needs to be tested both vertically and horizontally
    // to see if it would be a possible fit.

    // if the board has a hit/miss marker there then it's impossible for different ship to be there
    if(board[x][y] != -1)
    {
        return probabilty;
    }
    else
    {
        // need to check both horizontally and vertically for each remaining ship
        for(int s = 0; s < fleetSize; s++)
        {
            // only checking live ships
            if(fleet[s].alive)
            {
                bool possible;
                int start, end;
                // loop for the number of ships the square could be a part of for all horizontal locations
                for(int i = 1; i <= fleet[s].length; i++)
                {
                    possible = true;
                    start = y-(fleet[s].length-i);
                    end = y-(fleet[s].length-i)+fleet[s].length-1;
                    // loop through all the squares in the ship
                    for(int j = start; j <= end; j++)
                    {
                        // if a ship can't be placed here because it's not entirely on the board
                        // or a confirmed miss is there don't increase the probability
                        if(j < 0 || j > DIM || board[x][j] == 1)
                        {
                            possible = false;
                        }
                    }
                    // if no invalid spaces are in the possible ship increase the squares probability
                    if(possible)
                    {
                        probabilty++;
                    }
                }
                // loop for the number of ships the square could be a part of for all vertical locations
                for(int i = 1; i <= fleet[s].length; i++)
                {
                    possible = true;
                    start = x-(fleet[s].length-i);
                    end = x-(fleet[s].length-i)+fleet[s].length-1;
                    // loop through the number of squares in the possible ship
                    for(int j = start; j <= end; j++)
                    {
                        // check that the ship space is on the board and not a miss
                        if(j < 0 || j > DIM || board[j][y] == 1)
                        {
                            possible = false;
                        }
                    }
                    // if a ship can be placed in the location increase the probability
                    if(possible)
                    {
                        probabilty++;
                    }
                }
            }
        }
        return probabilty;  
    }
}


int main(int argc, char **argv){

    // keeps the process alive until the game ends
    int playing = 1;

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

    // temp board for debugging
    int temp[DIM][DIM];

    // an array that will hold all of the shots fired by the user
    int playersBoard[DIM][DIM];

    // an array that will display the probability density for each square
    // this array MUST be square to work; no wonky rectangles allowed.
    int probability[DIM][DIM];

    // value that will be used to pass the probability data
    int square[2];

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
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size); 

    // MPI Tag table - makes sure the are always higher than the number of squares possible
    // we're doing this because we're going to be using specific tags later on that are 
    // dependent on the number of squares possible on the board
    int playingTag = 1;
    int boardTag = 2;
    int playersBoardTag = 3;
    int fleetTag = 4;
    int squareTag = 5;

    // seed the rng
    srand(rank + 1 * time(0));

    //set up a clear board
    clearBoard(board);
    clearBoard(startingBoard);
    clearBoard(temp);
    clearBoard(playersBoard);


    //place the ships
    for(int i = 0; i < fleetSize; i++)
    {
        placeShip(board, startingBoard, fleet[i]);
    }


    // START THE GAME
    if(rank == 0)
    {
        // update the other processors with this game's initial setup
        for(int i = 0; i < size; i++)
        {
            if(i)
            {
                MPI_Send(&board,DIM*DIM,MPI_INT,i,boardTag,MCW);
                MPI_Send(&playersBoard,DIM*DIM,MPI_INT,i,playersBoardTag,MCW);
            }
        }

        // initialize the game variables:
        cout << "Welcome to Battleship!" << endl;
        int shotCount = -1;
        coordinates currentShot = {0, 0, 0, fleetSize};

        //start the game timer 
        timerStart = MPI_Wtime();

        // barrier makes sure everything was initialzed correctly before the calculations begin
        MPI_Barrier(MCW);

        // keep going until all of the ships have been sunk by the player
        while(playing)
        {
            // recieve the calculations from the other processors
            for(int i = 0; i < DIM*DIM; i++)
            {
                // probe for all of the incoming values 
                MPI_Probe(MPI_ANY_SOURCE,squareTag,MCW,&mystatus);
                int count;
                MPI_Get_count(&mystatus,MPI_INT,&count);
  
                // the square value is an array with 2 elements:
                // square[0] corresponds to where it belongs in the 2d array
                // square[1] holds the value that will go there.
                MPI_Recv(square,count,MPI_INT,MPI_ANY_SOURCE,squareTag,MCW,&mystatus);

                // square[0]/DIM gives us the x coordinate
                // square[0]%DIM gives us the y coordinate
                // we put the square[1] value into the probabilty[x][y] spot
                probability[square[0]/DIM][square[0]%DIM] = square[1];
            }

            // print revelant info to the player
            shotCount++;
            cout << "Current shot count: " << shotCount << endl;
            cout << "Current Board:" << endl;
            printPlayersBoard(playersBoard);

            // print the probability density functions for the player
            cout << endl;
            cout << "Most likely unknown ship locations." << endl;
            cout << "(The higher the number the better the chance.)" << endl;
            printBoard(probability);

            // get the players next shot coordinates
            currentShot = obtainUserCoordinates(playersBoard, board, currentShot);

            // if the shot hits; update the fleet status
            if(currentShot.hit)
            {
                getSurvivors(board, fleet, fleetSize);
            }

            // print the remaining ships to the console
            printSunken(fleet, fleetSize);

            // determine if the game should keep going or not
            playing = gameOver(fleet, fleetSize);

            // update the other processors with this round's data
            for(int i = 0; i < size; i++)
            {
                if(i)
                {
                    MPI_Send(&playing,1,MPI_INT,i,playingTag,MCW);
                    MPI_Send(&board,DIM*DIM,MPI_INT,i,boardTag,MCW);
                    MPI_Send(&playersBoard,DIM*DIM,MPI_INT,i,playersBoardTag,MCW);
                }
            }

        }
        //GAME OVER
        // Prints how long the game took and the final score (shotcount)
        timerEnd = MPI_Wtime();
        printBoard(startingBoard);
        cout << "Congratulations! You win! You've sunk all of the ships." << endl;
        cout << "Final shot count: " << shotCount << endl;
        cout << "Final time: "<< timerEnd-timerStart << endl;
    }
    // the other processors are going to be calculating the probabilty
    // that a ship would be placed into each and every square and sending
    // that information to the root process
    else
    {
        // Before the game starts:
        // get initial game board
        MPI_Recv(&board,DIM*DIM,MPI_INT,0,boardTag,MCW,MPI_STATUS_IGNORE);
        // get the initial players board
        MPI_Recv(&playersBoard,DIM*DIM,MPI_INT,0,playersBoardTag,MCW,MPI_STATUS_IGNORE);
        getSurvivors(board, fleet, fleetSize);

        // barrier makes sure everything was initialzed correctly before the calculations begin
        MPI_Barrier(MCW);

        // START GAME
        while(playing)
        {
            // index to help evenly distribute the squares to each of the processors
            int index = 0;
            // distribute the squares to the different processors
            for(int i = 0; i < DIM*DIM; i++)
            {
                index++;
                // job gets assigned to a process
                if(rank == index)
                {
                    // holds the squares linear number - gets converted into x,y by root process
                    square[0] = i;
                    // calculates and holds the probability for this square
                    square[1] = getProbability(playersBoard, fleet, fleetSize, i);

                    // send the square's location and value to root process
                    MPI_Send(&square,2,MPI_INT,0,squareTag,MCW);
                }
                //restart the index to keep jobs evenly distributed
                if(index == size -1)
                {
                    index = 0;
                }
            }
            // make sure that the game hasn't ended yet
            MPI_Recv(&playing,1,MPI_INT,0,playingTag,MCW,MPI_STATUS_IGNORE);
            // get the updated game board
            MPI_Recv(&board,DIM*DIM,MPI_INT,0,boardTag,MCW,MPI_STATUS_IGNORE);
            // get the updated players board
            MPI_Recv(&playersBoard,DIM*DIM,MPI_INT,0,playersBoardTag,MCW,MPI_STATUS_IGNORE);
            getSurvivors(board, fleet, fleetSize);
        }
    }
    

    MPI_Barrier(MCW);
    MPI_Finalize();

    return 0;
}