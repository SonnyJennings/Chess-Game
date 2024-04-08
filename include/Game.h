/*!
 * \file Game.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Game Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-25
 * \copyright Copyright (c) 2021
 */

#ifndef GAME_H
#define GAME_H

// ------------------------- Dependencies ------------------------- //

#include "main.h"
#include "GameData.h"
#include <signal.h>
// ------------------------- Functions ------------------------- //

/// Runs the game according to the settings
void Game(GameData* const data);

/// Prompts the game log to start
void PromptGameLog();

/// Takes in a blank string and sets it to the file name
bool GetLogName(char* name);

#endif

// EOF //
