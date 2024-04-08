/*!
 * \file Gamedata.c
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Gamedata Module of Ultimate Chess
 * \version 1.0
 * \date 2021-05-02
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "GameData.h"

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Sets the default gamedata
 * \param data: Takes in the current gamedata
 */
void ResetGameData(GameData* const data)
{

    ResetPlayer(&data->white[0], WHITE);
    ResetPlayer(&data->black[0], BLACK);
    ResetSettings(&data->settings[0]);
    ResetMoveStack(&data->stack[0]);
    ResetBoard(&data->board[0]);
    ResetTurnCounter(data);

}

/*!
 * \brief Sets the default turncounter
 * \param data: Takes in the current gamedata
 */
void ResetTurnCounter(GameData* const data)
{

    data->turncounter[0] = 0;

}

/*!
 * \brief Returns the turncounter
 * \param data: Takes in the current gamedata
 * \returns uint8_t: The turncounter
 */
uint8_t GetTurnCounter(const GameData* const data)
{

    return data->turncounter[0];

}

/*!
 * \brief Increments the turncounter
 * \param data: Takes in the current gamedata
 */
void IncrementTurnCounter(GameData* const data)
{

    data->turncounter[0]++;

}

/*!
 * \brief Gets the board From the gamedata
 * \param data: Current gamedata 
 * \returns Board*: Pointer To the board
 */
Board* GetBoard(const GameData* const data)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(data->board, "Invalid Board Pointer");

    return (Board*)data->board;

}

/*!
 * \brief Sets the Board* field in the gamedata struct
 * \param data: Current gamedata
 * \param board: Board to set the data to
 */
void SetBoard(GameData* const data, const Board* const board)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(board, "Invalid Board Pointer");

    data->board[0] = *board;

}

/*!
 * \brief Gets the setings from the gamedata
 * \param data: Current gamedata
 * \returns Settings*: Pointer to the games settings
 */
Settings* GetSettings(const GameData* const data)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(data->settings, "Invalid Settings Pointer");

    return (Settings*)data->settings;

}

/*!
 * \brief Sets the games settings 
 * \param data: Current gamedata
 * \param settings: Settings to set
 */
void SetSettings(GameData* const data, const Settings* const settings)
{
    
    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    data->settings[0] = *settings;

}

/*!
 * \brief Sets the movestack in the gamedata struct to the actual movestack
 * \param data: The gamedata
 * \param stack: Stack to set to
 */
void SetMoveStack(GameData* const data, const MoveStack* stack)
{
    
    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(stack, "Invalid Stack Pointer");

    data->stack[0] = *stack; // Sets the movestack in the gamedata struct to the actual movestack

}

/*!
 * \brief Gets the stack from the gamedata
 * \param data: Current gamedata
 * \returns MoveStack*: Pointer to the games movestack
 */
MoveStack* GetMoveStack(const GameData* const data)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(data->stack, "Invalid Movestack Pointer");

    return (MoveStack*)data->stack;

}

/*!
 * \brief Gets one of the players in the game
 * \param data: Current gamedata
 * \param isWhite: If the player you want to get is white
 * \returns Player*: Pointer to the cooresponding player
 */
Player* GetPlayer(const GameData* const data, const bool isWhite)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(data->white, "Invalid White Player");
    STATIC_ASSERT(data->black, "Invalid Black Player");

    return (Player*)(isWhite? data->white: data->black);

}

// EOF //
