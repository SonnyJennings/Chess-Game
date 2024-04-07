/*!
 * \file Movevalidation.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Movevalidation Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef MOVEVALIDATION_H
#define MOVEVALIDATION_H

// ------------------------- Dependencies ------------------------- //

#include "GameData.h"
#include "main.h"

// ------------------------- Functions ------------------------- //

/// Sees if the move is valid
bool IsValidMove(const GameData* const data, const Player* const player, const Move move);

/// Move checker for threading
void CreateCheckMoveThread(pthread_t* const thread, const GameData* const data, Player* const player, const Move move);

#endif

// EOF //