/*!
 * \file Movevalidation.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Movevalidation Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-17
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "MoveValidation.h"
#include "Gameplay.h"
#include "MoveList.h"

// ---------------------------- Custom Types -------------------- //

/*!
 * \brief Stores the threaddata 
 * \details Store the game data player data and move data
 */
typedef struct 
{
    
    GameData* data;       ///< Gamedata for board and stack
    Player* player;             ///< Player to check move For
    Move move;                  ///< Move to check 

} ThreadData;


// ------------------------- Functions ------------------------- //

/// Checks if the move is valid for a pawn
static bool IsValidPawnMove(const GameData* const data, const Player* const player, const Move move);

/// Checks if the move is valid for a knight
static bool IsValidKnightMove(const Move move);

/// Checks if the move is valid for a queen
static bool IsValidQueenMove(const GameData* const data, const Move move);

/// Checks if the move is valid for a rook
static bool IsValidRookMove(const GameData* const data, const Move move);

/// Checks if the move is valid for a bishop
static bool IsValidBishopMove(const GameData* const data, const Move move);

/// Checks if the move is valid for a king
static bool IsValidKingMove(const GameData* const data, const Player* const player, const Move move);

/// Checks if a player can castle in a given direction
static bool IsAbleToCastle(const GameData* const data, const Player* const player, const int8_t direction);

/// Thread Implementation Of Checking Valid Moves
static void* CheckMoveThread(void* threaddata);

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Checks the move and goes move.end the cooresponding valid move check based on the selected piece 
 * \param data: The gamedata
 * \param player: The player making the move
 * \param move: The players move
 * \returns bool: Value move.end show if a move is valid
 */
bool IsValidMove(const GameData* const data, const Player* const player, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    if(move.piece == EMPTY) return false;

    if(IsValidPiece(player, GetPiece(GetBoard(data), move.end))) return false; // If the end point is one of your pieces return false
    
    if(move.start == move.end) return false; // Dont allow anyone to not move
    
    if(move.start > 63 || move.end > 63) return false; // Dont allow out of bounds moves
    
    switch(GetPieceID(move.piece)) // The piece ID
    {

        case PAWN: return IsValidPawnMove(data, player, move);
        case KNIGHT: return IsValidKnightMove(move);
        case BISHOP: return IsValidBishopMove(data, move);
        case ROOK: return IsValidRookMove(data, move);
        case QUEEN: return IsValidQueenMove(data, move);
        case KING: return IsValidKingMove(data, player, move);
        default: return false;

    }
}

/*!
 * \brief Checks if the Move is Valid for a pawn
 * \param data: Current gamedata
 * \param move: Move to Check
 * \returns bool: If the Move is Valid for a Pawn
 */
static bool IsValidPawnMove(const GameData* const data, const Player* const player, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(player, "Invalid Current Player Pointer");

    Player* next = IsPlayerWhite(player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // The next player based on the current one

    int direction = IsPieceWhite(move.piece)? 1: -1; // Direction the pawn will be moving

    int8_t tocol = GetColumn(move.end); // Initial column
    int8_t fromcol = GetColumn(move.start); // Final column
    int8_t torow = GetRow(move.end); // Initial row
    int8_t fromrow = GetRow(move.start); // Final row
    
    if(torow == (fromrow + direction) && (tocol == fromcol) && !GetPiece(GetBoard(data), move.end)) // Checking if moving one space forward is valid
        return true;
    
    if((torow == (fromrow + direction)) && (abs(tocol - fromcol) == 1) && IsValidPiece(next, GetPiece(GetBoard(data), move.end))) // Checking if pawn can take a piece is a valid move
        return true;
    
    if(torow == (fromrow + 2 * direction) && (tocol == fromcol) && (move.start == GetStartingIndex(move.piece)) && !GetPiece(GetBoard(data), move.end) && !GetPieceAt(GetBoard(data), fromcol, fromrow + direction)) // Checking if pawn can move 2 spaces because it is in the default starting position and there isn't something there
        return true;
    
    Move temp = PeekMove(GetMoveStack(data)); // The prior move

    if(GetPieceID(temp.piece) == PAWN && abs(GetRow(temp.start) - GetRow(temp.end)) == 2) // if the last move was a double move 
        if(abs(GetColumn(move.start) - GetColumn(temp.end)) == 1 && tocol == GetColumn(temp.end) && torow == (fromrow + direction)) // Check if can enpassant
            return true;

    return false;

}

/*!
 * \brief Checks if the move is valid for a knight
 * \param data: Current gamedata
 * \param player: The player making the move
 * \param move: Move to check
 * \returns bool: If the move is valid
 */
static bool IsValidKnightMove(const Move move)
{
    
    Index to = move.end; // Start
    Index from = move.start; // End

    int diff = to - from; // The difference in location

    uint8_t fromRow, fromCol;

    fromRow = GetRow(from); // Initial row
    fromCol = GetColumn(from); // Initial Column

    switch (diff) //checking all knight moves
    {

        case 17: return ((fromRow > 5) ? false : (fromCol == 7) ? false : true); // If its trying to move 2 forward 1 right it cant be in the top 2 rows or the right column
        case 15: return ((fromRow > 5) ? false : (fromCol == 0) ? false : true); // If its trying to move 2 forward 1 left it cant be in the top 2 rows or the left column
        case 10: return ((fromRow == 7) ? false : (fromCol > 5) ? false : true); // If its trying to move 1 forward 2 right it cant be in the top row or the 2 right columns
        case 6: return ((fromRow == 7) ? false : (fromCol < 2) ? false : true); // If its trying to move 1 forward 2 left it cant be in the top row or the 2 left columns
        case -6: return ((fromRow == 0) ? false : (fromCol > 5) ? false : true); // If its trying to move 1 backward 2 right it cant be in the bottom row or the 2 right columns
        case -10: return ((fromRow == 0) ? false : (fromCol < 2) ? false : true); // If its trying to move 1 backward 2 left it cant be in the bottom row or the 2 left columns
        case -15: return ((fromRow < 2) ? false : (fromCol == 7) ? false : true); // If its trying to move 2 backward 1 right it cant be in the bottom 2 rows or the right column
        case -17: return ((fromRow < 2) ? false : (fromCol == 0) ? false : true); // If its trying to move 2 backward 1 left it cant be in the bottom 2 rows or the left column
        default: return false;

    }
}

/*!
 * \brief Checks if a move is valid for a bishop
 * \param data: Current gamedata
 * \param player: The player making the move
 * \param move: Move to check
 * \returns bool: If the move is valid
 */
static bool IsValidBishopMove(const GameData* const data, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");

    Index to = move.end; // The end
    Index from = move.start; // The start

    uint8_t fromRow, fromCol, toRow, toCol, diffRow, diffCol;

    fromRow = GetRow(from); // Initial row
    fromCol = GetColumn(from); // Initial column
    toRow = GetRow(to); // Final row
    toCol = GetColumn(to); // Final column
    diffRow = abs(toRow - fromRow); // Difference in rows
    diffCol = abs(toCol - fromCol); // Difference in columns

    if(diffRow != diffCol) return false; // If the differences dont match its not a diagonal move

    int8_t colDirection, rowDirection;

    colDirection = ((int8_t)(fromCol - toCol) < 0) ? 1 : -1; // Horizontal direction
    rowDirection = ((int8_t)(fromRow - toRow) < 0) ? 1 : -1; // Vertical direction

    for (uint8_t i = 1; i < diffRow; i++)
    {

        if(GetPieceAt(GetBoard(data), fromCol + colDirection * i, fromRow + rowDirection * i)) // If there is a piece in way its invalid
            return false;

    }

    return true;

}

/*!
 * \brief Checks if the Move is valid for a Rook
 * \param data: Current gamedata
 * \param player: The player making the move
 * \param move: Move to Check
 * \returns bool: If the Move Is Valid
 */
static bool IsValidRookMove(const GameData* const data, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");

    Index to = move.end; // The end
    Index from = move.start; // The start

    int8_t fromRow, fromCol, toRow, toCol, vertDir, horzDir;

    uint8_t dif;

    fromRow = GetRow(from); // Initial row
    fromCol = GetColumn(from); // Initial column
    toRow = GetRow(to); // Final row
    toCol = GetColumn(to); // Final Column
    vertDir = (toRow > fromRow)? 1: -1; // If the final position is above the initial its 1 otherwise -1
    horzDir = (toCol > fromCol)? 1: -1; // If the final position is to the right of the initial its 1 otherwise -1

    if(toRow == fromRow) // If its on the same row it doesnt move up or down
    {
        
        vertDir = 0;
        dif = abs(toCol - fromCol); // The length is the difference in columns

    }
    
    else if (fromCol == toCol) // If its on the same column it doesnt move right or left
    {
        
        horzDir = 0;
        dif = abs(toRow - fromRow); // The length is the difference in rows

    }

    else return false; // If its not on the same row or column its not a rook move

    for (uint8_t i = 1; i < dif; i++) // Scans through all locations in between the initial and final positions
        if(GetPiece(GetBoard(data), from + ((i*vertDir) << 3) + (i*horzDir))) return false; // If theres a piece in the way its invalid

    return true;

}

/*!
 * \brief Checks to See if the Move Is Valid for a Queen
 * \param data: Current gamedata
 * \param player: The player making the move
 * \param move: Move to Check
 * \returns bool: If the move Is valid
 */
static bool IsValidQueenMove(const GameData* const data, const Move move)
{

    return IsValidRookMove(data, move) || IsValidBishopMove(data, move); // Checks if its trying to do a rook move or a bishop move

}

/*!
 * \brief Checks if a Move is Valid for a Knight
 * \param data: Current gamedata
 * \param player: The player making the move
 * \param move: Move to Check
 * \returns bool: If the Move is valid
 */
static bool IsValidKingMove(const GameData* const data, const Player* const player, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");

    Index to = move.end; // The end
    Index from = move.start; // The start

    int diff = to - from; // The difference

    uint8_t fromRow, fromCol;

    fromRow = GetRow(from); // The starting row
    fromCol = GetColumn(from); // The starting column

    switch (diff)
    {

        case 1: return ((fromCol == 7) ? false : true); // Cant move right if your on the far right
        case 2: return IsAbleToCastle(data, player, 1); // Checks if you can castle
        case -2: return IsAbleToCastle(data, player, -1); // Checks if you can castle
        case 9: return ((fromRow == 7) ? false : (fromCol == 7) ? false : true); // Cant move up and right if your on the right column or top row
        case 8: return ((fromRow == 7) ? false : true); // Cant move up if your on the top row
        case 7: return ((fromRow == 7) ? false : (fromCol == 0) ? false : true); // Cant move up and left if your on the left column or the top row
        case -1: return ((fromCol == 0) ? false : true); // Cant move left if youre on the far left
        case -9: return ((fromRow == 0) ? false : (fromCol == 0) ? false : true); // Cant move down and left if your on the bottom or far left column
        case -8: return ((fromRow == 0) ? false : true); // Cant move down if your on the bottom row
        case -7: return ((fromRow == 0) ? false : (fromCol == 7) ? false : true); // Cant move down and right if your on the bottom row or right most column
        default: return false;

    }
}

/*!
 * \brief Checks to see if the King can castle in a given direction
 * \param data: Current gamedata
 * \param direction: Direction to Castle, -1 or 1
 * \returns bool: If the King is Able to Castle
 */
bool IsAbleToCastle(const GameData* const data, const Player* const player, const int8_t direction)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(player, "Invalid Current Player Pointer");

    Index kingloc = GetPieceLoc(player, KING, 0); // The kings initial location

    MoveList list[1];

    Player* next = IsPlayerWhite(player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // The next player

    uint8_t rooknum = (direction < 1)? 0: 1; // Sets the direction intending to castle too

    if(!CanCastle(player, rooknum)) // If you cant castle return false
        return false;

    if(direction < 1) // If its left direction if theres a piece next to the rook you cant castle
        if(GetPiece(GetBoard(data), kingloc - 3)) return false;
        
    if(GenerateValidMoves(data, list, next, kingloc)) return false; // Gnerates any moves to see if anything is threatening the king

    for(uint8_t i = 1; i < 3; i++) // check to see if there is something in the way or castling goes through check
    {

        if(GetPiece(GetBoard(data), kingloc + direction * i)) return false; // if there are pieces between the rook and king then 
        
        if(GenerateValidMoves(data, list, next, kingloc + direction * i)) return false; // if the king goes through check, return you cant do it
        
    }

    return true;

}

/*!
 * \brief Creates the move thread
 * \param data: The game data
 * \param player: The player currently up
 * \param move: The intended move
 * \returns ThreadData: Data about the move for the thread
 */
ThreadData CreateThreadData(const GameData* const data, Player* const player, const Move move)
{
    
    ThreadData temp;
    temp.player = player;
    temp.data = (GameData*)data;
    temp.move = move;

    return temp;

}

/*!
 * \brief Function pointer to feed a thread
 * \param CheckMoveFunc: Move Checking Function
 * \param data: Current Game Data
 * \param player: Current Player
 * \param move: Move to Check
 */
void CreateCheckMoveThread(pthread_t* const thread, const GameData* const data, Player* const player, const Move move)
{

    STATIC_ASSERT(thread, "Invalid Thread Pointer");

    static ThreadData td;
    td = CreateThreadData(data, player, move); // Creates the thread data

    if(pthread_create(thread, NULL, CheckMoveThread, &td)) // Checks for bad threads
        perror("Faulty Creation of Thread");


}

/*!
 * \brief Checks the move thread
 * \param data: The game data
 */
void* CheckMoveThread(void* threaddata)
{
    
    STATIC_ASSERT(threaddata, "Invalid Thread Data Pointer");

    ThreadData* td = (ThreadData*)threaddata; // Takes the threaddata

    STATIC_ASSERT(td->player, "Invalid Player Pointer");
    STATIC_ASSERT(td->data, "Invalid Game Data Pointer");

    if(td->move.start > 64 || td->move.end > 64 || !td->move.piece) // Checks foe bad indexes
        pthread_exit(NULL);

    if(IsValidMove(td->data, td->player, td->move) && !MovesIntoCheck(td->data, td->player, td->move)) // Checks if the move is valid
        pthread_exit((void*)0xff);

    pthread_exit(NULL);

}

// EOF //