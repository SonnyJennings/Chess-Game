/*!
 * \file Game.c
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Game Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-25
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Game.h"
#include "Settings.h"
#include "Gameplay.h"
#include "AI.h"
#include "AIGameplay.h"
#include "MoveValidation.h"
#include "tcpClient.h"

static int FD = -1; //Player Ctrl C exit flag
// ------------------------- Functions ------------------------- //

/// Game where both players are humans
static Status PlayerVsPlayerGame(GameData* const data);

/// Game where one of the players is an AI
static Status PlayerVsAIGame(GameData* const data);

/// Game where both players are AI
static Status AIVsAIGame(GameData* const data);

/// Online Game with another player through CS connection
static Status OnlinePvPGame(GameData* const data);

//Make move and send to Server
Move MakeOnlineMove(const GameData* const data, Player* const player, char *hostname, int PortNo, int DataFD);

//Get move from server and make move
Move ReceiveOnlineMove(const GameData* const data, char *hostname, int PortNo, int DataFD);

/// Log the game to the file with name
static void LogGame(const GameData* const data, const char* name);

/// Print the Message for the given status
static void PrintStatusMessage(const Status status);

/// Gets a Move From the User
Move GetPlayerMove(const GameData* const data, Player* const player);

void PlayerQuitHandler(int sig);

// ------------------------- Definintions ------------------------- //

void PlayerQuitHandler(int sig)
{
    if(FD < 0)
    {
        printf("\nPlayer Quit Game unsuccessful\n");
    }
    else
    {
	 EndConnection(FD);
	 printf("\nPlayer quit game\n");
     exit(10);
    }
}

/*!
 * \brief The main game function, redirects to local mode-specific games
 * \param data: The gamedata to use
 */
void Game(GameData* const data)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetPlayer(data, WHITE), "Invalid White Reference");
    STATIC_ASSERT(GetPlayer(data, BLACK), "Invalid Black Reference");
    STATIC_ASSERT(GetSettings(data), "Invalid Settings Pointer");
    STATIC_ASSERT(GetMoveStack(data), "Invalid Stack Pointer");

    Status status = InProgress;

    switch(GetGameMode(GetSettings(data))) // Uses the stored gamemode to set the game to that mode
    {

        case HumanVHuman: 
            status = PlayerVsPlayerGame(data); // The player vs player gamemode
            break;
        case HumanVAI: 
            status = PlayerVsAIGame(data); // The player vs AI gamemode
            break;
        case AIVAI: 
            status = AIVsAIGame(data); // The AI vs AI gamemode
            break;
        case OnlinePvP:
            status = OnlinePvPGame(data);
            break;
        default: break;

    }

    PrintBoard(GetBoard(data), GetSettings(data));
    
    PrintStatusMessage(status); // Print the message for the game result

    PromptGameLog(); // Calls the game log to start running

    char buffer[50]; // To store the name of the game log

    if(GetLogName(buffer)) 
        LogGame(data, strcat(buffer, ".txt")); // If it gets the game log name itll log the game  

    DeleteMoveStack(GetMoveStack(data)); // Deletes the movestack after the game

    ResetBoard(GetBoard(data));  // Zero out the players and board

    ResetPlayer(GetPlayer(data, WHITE), WHITE); // Resets the white player
    ResetPlayer(GetPlayer(data, BLACK), BLACK); // Resets the black player
    
}

/*!
 * \brief The player vs player game starting point
 * \param data: The gamedata for the Game
 */
static Status PlayerVsPlayerGame(GameData* const data)
{

    Status status = InProgress; // Sets game status to start
    Player* currplayer = GetPlayer(data, WHITE); // Sets the current player to the white player
    Move move = CreateMove(0, 0, 0); // initializes an empty move

    do // The actual game
    {   

        ClearScreen(); // Clears the screen
        
        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board

        if(IsInCheck(currplayer)) 
            puts("You are in check"); // Print if current player is in check
        
        move = GetPlayerMove(data, currplayer); // Gets the players move
        
        if(move.piece == QUIT)
            return IsPlayerWhite(currplayer)? WhiteQuit: BlackQuit; // If the player types QQ it quits
        
        MakeMove(data, currplayer, move); // Executes the moves and modifies all of the data

        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board

        currplayer = IsPlayerWhite(currplayer)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // Changes which player is up

        status = GetGameStatus(data, currplayer); // Updates the game status

    } while (status == InProgress); // Repeats as long as the game is still going
    

    return status;

}

/*!
 * \brief The player vs AI game starting point
 * \param data: Gamedata for current game
 */
static Status PlayerVsAIGame(GameData* const data)
{  

    Status status = InProgress; // Sets the game to start
    AI ai[1]; // Initializes an AI
    Player* currplayer = GetPlayer(data, WHITE); // Sets the current player to white
    Move move = CreateMove(0, 0, 0); // Creates an empty move

    PromptPlayerSelection(); // Prompts the player to choose a side
    SetAIPlayer(ai, GetPlayerSelection()? GetPlayer(data, BLACK): GetPlayer(data, WHITE)); // Sets the AI to the opposite of the player 

    do // The actual game
    {   

        ClearScreen(); // Clears the screen
        
        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board

        if(currplayer != GetAIPlayer(ai))
            move = GetPlayerMove(data, currplayer);        
        
        if(move.piece == QUIT)
            return IsPlayerWhite(currplayer)? WhiteQuit: BlackQuit; // If the player types QQ it quits
        
        if(currplayer == ai->player)
            move = GenerateBestMove(data, ai);

        MakeMove(data, currplayer, move);

        currplayer = IsPlayerWhite(currplayer)? GetPlayer(data, BLACK): GetPlayer(data, WHITE);

        status = GetGameStatus(data, currplayer); // Updates the game status

    } while (status == InProgress); // Repeats as long as the game is still going

    return status;

}

/*!
 * \brief The AI vs AI game starting point
 * \param settings: The settings struct for the game
 */
static Status AIVsAIGame(GameData* const data)
{
    Status status = InProgress; // Sets the status to start
    AI ai[2] = {GetDefaultAI(), GetDefaultAI()}; // Initializes two AIs

    SetAIPlayer(&ai[0], GetPlayer(data, WHITE)); // Sets one to white
    SetAIPlayer(&ai[1], GetPlayer(data, BLACK)); // Sets the other to black
    
    Player* currplayer = GetPlayer(data, WHITE); // Sets the current player to white
    Move move = CreateMove(0, 0, 0); // Creates a blank move

    do // The actual game
    {   

        ClearScreen(); // Clears the screen
        
        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board

        if(currplayer == ai[0].player)
            move = GenerateBestMove(data, &ai[0]);

        else move = GenerateBestMove(data, &ai[1]);

        MakeMove(data, currplayer, move); // Executes the moves and modifies al of the data

        currplayer = IsPlayerWhite(currplayer)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // Changes which player is up

        status = GetGameStatus(data, currplayer); // Updates the game status

    } while (status == InProgress); // Repeats as long as the game is still going

    return status;

}

static Status OnlinePvPGame(GameData* const data)
{

    Status status = InProgress; // Sets game status to start
    Player* currplayer = GetPlayer(data, WHITE); // Sets the current player to the white player
    Move move = CreateMove(0, 0, 0); // initializes an empty move
    
    char hostname[30]; //Sever hostname
    strcpy(hostname,data->settings->connect.hostname); //copy from global var host since already set from login
    int PortNo = data->settings->connect.PortNo; //copy PortNo from Port

    //Don't want to get player num again to prevent conflict from another login
    int playerNum = data->settings->connect.playerNum; //copy from global var playerNumber since already set from login

    int DataFD = data->settings->connect.DataFD;

    FD = DataFD;

    int turnNum = 1; //Keep track of turn number

    signal(SIGINT,PlayerQuitHandler);


    do // The actual game
    {   

        ClearScreen(); // Clears the screen
        
        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board


        if(IsInCheck(currplayer)) 
            puts("You are in check"); // Print if current player is in check

        if(((turnNum % 2 == 1) && (playerNum == 1)) || ((turnNum % 2 == 0) && (playerNum == 2)))
        {
         move = MakeOnlineMove(data, currplayer, hostname, PortNo, DataFD); // Prompt user for move and send to server
        }
        else
        {
         move = ReceiveOnlineMove(data, hostname, PortNo, DataFD); //Get move from server
        }
        
        if(move.piece == QUIT)
            return IsPlayerWhite(currplayer)? WhiteQuit: BlackQuit; // If the player types QQ it quits
        
        MakeMove(data, currplayer, move); // Executes the moves and modifies all of the data

        PrintBoard(GetBoard(data), GetSettings(data)); // Prints the board

        currplayer = IsPlayerWhite(currplayer)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // Changes which player is up

        status = GetGameStatus(data, currplayer); // Updates the game status

        turnNum++;
        sleep(2);

    } while (status == InProgress); // Repeats as long as the game is still going

    GameResetClient(hostname,PortNo, DataFD); //Reset number of players in server to 0

    return status;

}

/*!
 * \brief Sends the message to the player asking if they want a log or not
 */
void PromptGameLog()
{

    printf("Would you like to save a log of this game (y/n)? ");

}

/*!
 * \brief Used to set the name of the log
 * \param buffer: The string that holds name of the log 
 * \returns bool: returns true or false based on whether they made a log or not 
 */
bool GetLogName(char* buffer)
{
    
    STATIC_ASSERT(buffer, "Invalid Buffer Pointer");

    char response; // Blank char to let a player respond

    if(scanf(" %c", &response) && response == 'y') // Checks if the player responded and said yes
        return printf("What would you like the log to be called? ") && scanf(" %s", buffer); // Returns true if the player inputed a log name
    
    return false;

}

/*!
 * \brief Logs the game to the file with name
 * \param data: Takes in the gamedata
 * \param name: Name of log file
 */
void LogGame(const GameData* const data, const char* name)
{

    FILE* file = fopen(name, "w");

    fputs("Ultimate Chess Verison 1.0\n", file); // Fills the log
    fputs("Game Log: \n", file);
    fputs(__DATE__"\n", file);
    fputs(__TIME__"\n", file);
    fprintf(file, "\nFile Name: %s\n", name);

    fputs("Moves Made: \n\n", file);

    fprintmovestack(file, GetMoveStack(data)); // Prints the move log based on the movestack in the gamelog

    fclose(file);

}

/*!
 * \brief Prints the game ending message
 * \param status: The game status
 */
static void PrintStatusMessage(const Status status)
{

    switch (status) // print a game over message
    {

        case StaleMate:
            puts("Stalemate!\n");
            break;
        case Tie:
            puts("Tie! \n");
            break;
        case WhiteWins:
            puts("Checkmate, White Wins! \n");
            break;
        case BlackWins:
            puts("Checkmate, Black Wins! \n");
            break;
        case WhiteQuit:
            puts("White Quit, Black Wins! \n");
            break;
        case BlackQuit:
            puts("Black Quit, White Wins! \n");
            break;
        default:
            break;

    }
}

/**
 * @brief Get the Player Move object
 * @param data: The game data
 * @param player: The player struct
 * @return Move: The players move
 */
Move GetPlayerMove(const GameData* const data, Player* const player)
{

    Move move;

    for(;;)
    { 

        PromptWhichPiece(IsPlayerWhite(player)); // Prompts the player for which piece they want to move

        move.start = GetIndex(); // Sets from to where the piece is located
    
        if(move.start == QUIT) 
            return (Move){QUIT, QUIT, QUIT}; // if the user puts in the QUIT code then quit the game

        move.piece = GetPiece(GetBoard(data), move.start); // Sets the piece to the piece at the selected location   

        if(!IsValidPiece(player, move.piece) && puts("Invalid piece, try again"))
            continue;

        PromptWhereTo(); // Prompts the player to submit where they intend to move to
        move.end = GetIndex(); // Gets the ending index

        if(move.end == QUIT) 
            return (Move){QUIT, QUIT, QUIT}; // if the user puts in the QUIT code then quit the game

        bool CanTakePiece = IsValidMove(data, player, move);
        bool PutInCheck = MovesIntoCheck(data, player, move);

        if(CanTakePiece && !PutInCheck) break; // if the move can be made exit with the move saved

        puts("Invalid move, try again");
            
    } // Prompts the player to try again if they have selected an invalid move

    return move;

}

Move MakeOnlineMove(const GameData* const data, Player* const player, char *hostname, int PortNo, int DataFD)
{

    Move move;

    for(;;)
    { 

        PromptWhichPiece(IsPlayerWhite(player)); // Prompts the player for which piece they want to move

        move.start = GetIndex(); // Sets from to where the piece is located

        /*if(Bye == 1)
	    {
        EndConnection(DataFD);
		exit(10);
	    }*/
    
        if(move.start == QUIT) 
            return (Move){QUIT, QUIT, QUIT}; // if the user puts in the QUIT code then quit the game

        move.piece = GetPiece(GetBoard(data), move.start); // Sets the piece to the piece at the selected location   

        if(!IsValidPiece(player, move.piece) && puts("Invalid piece, try again"))
            continue;

        PromptWhereTo(); // Prompts the player to submit where they intend to move to
        move.end = GetIndex(); // Gets the ending index

        /*if(Bye == 1)
	    {
        EndConnection(DataFD);
		exit(10);
	    }*/

        if(move.end == QUIT) 
            return (Move){QUIT, QUIT, QUIT}; // if the user puts in the QUIT code then quit the game

        bool CanTakePiece = IsValidMove(data, player, move);
        bool PutInCheck = MovesIntoCheck(data, player, move);

        if(CanTakePiece && !PutInCheck) 
        {
        MakeMoveClient (hostname, PortNo,  DataFD, move);
        break; // if the move can be made exit with the move saved
        }
        puts("Invalid move, try again");
            
    } // Prompts the player to try again if they have selected an invalid move

    return move;

}

Move ReceiveOnlineMove(const GameData* const data, char *hostname, int PortNo, int DataFD)
{

    Move move;
    while(IsWaiting(hostname,PortNo,DataFD)) //Check if waiting for other player to make move
    {
     /*if(Bye == 1)
	 {
      EndConnection(DataFD);
	  exit(10);
	 }*/
     continue;
    }

    move = ReceiveMoveClient (hostname,PortNo, DataFD); //Get move from server
    move.piece = GetPiece(data->board, move.start); //Set move piece

    return move; //return move

}




// EOF //
