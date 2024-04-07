/*!
 * \file Gamedata.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Gamedata Module of Ultimate Chess
 * \version 1.0
 * \date 2021-05-02
 * \copyright Copyright (c) 2021
 */

#ifndef GAMEDATA_H
#define GAMEDATA_H

// ------------------------- Dependencies ------------------------- //

#include "Settings.h"
#include "Player.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Gamedata struct to store game details
 * \details Stores all the game settings along with the board itself and both player structs
 * It also stores the players as curr and next along with the movestack
 */
typedef struct
{

    Settings settings[1];      ///< Stores the settings data
    
    Board board[1];            ///< Stores the board
    
    Player white[1];           ///< Stores the current player
    Player black[1];           ///< Stores the next player

    MoveStack stack[1];        ///< Stores the movestack

    uint8_t turncounter[1];       ///< Stores the number of moves

} GameData;

// ------------------------- Functions ------------------------- //

/// Gets the default gamedata
void ResetGameData(GameData* const data);

// Resets the turn counter
void ResetTurnCounter(GameData* const data);

/// Gets the turn count
uint8_t GetTurnCounter(const GameData* const data);

/// Increments the turn count
void IncrementTurnCounter(GameData* const data);

/// Gets one of the players from the data
Player* GetPlayer(const GameData* const data, bool isWhite);

/// Sets the moveStack in the data
void SetMoveStack(GameData* const data, const MoveStack* const stack);

/// Gets the movestack from the gamedata
MoveStack* GetMoveStack(const GameData* const data);

/// Gets the board from the gamedata
Board* GetBoard(const GameData* const data);

/// Sets the board in the gamedata
void SetBoard(GameData* const data, const Board* const board);

/// Gets the settings in the gamedata
Settings* GetSettings(const GameData* const data);

/// Sets the settings in the gamedata
void SetSettings(GameData* const data, const Settings* const settings);

#endif

// EOF //