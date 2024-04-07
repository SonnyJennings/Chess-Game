/*!
 * \file Gameplay.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Gameplay Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

// ------------------------- Dependencies ------------------------- //

#include "GameData.h"
#include "main.h"
#include "Moves.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Status enum type 
 * \details Used to indicate game state, even entries are black as before and white are odd
 * To get the base game state divide by 2 or right shift one 
 */
typedef enum
{

    InProgress = 0,
    BlackWins = 2,
    WhiteWins = 3,
    Tie = 4,
    StaleMate = 5,
    BlackQuit = 6,
    WhiteQuit = 7

} Status;

// ------------------------- Functions ------------------------- //

/// Generates a movenode given the guts of a move
Move MakeMove(GameData* const data, Player* const player, const Move move);

/// Gets the current game status
Status GetGameStatus(const GameData* const data, Player* const player);

/// Check if player checked is in checkmate
bool IsInCheckMate(const GameData* const data, Player* const player);

/// Checks if the player is in stalemate
bool IsInStalemate(const GameData* const data, Player* const player);

/// Checks if there is enough material to win
bool IsLackOfMaterial(Player* const player);

/// Returns the number of valid moves
uint32_t GetNumberOfValidMoves(const GameData* const data, Player* const player);

/// Checks If there are Valid Moves
bool AreValidMoves(const GameData* const data, Player* const player);

/// Checks to see if the piece can be chosen for a player
bool IsValidPiece(const Player* const player, const Piece piece);

/// Checks if the Player Moves Into Check With the Given Move
bool MovesIntoCheck(const GameData* const data, Player* const player, const Move move);

#endif

// EOF //