/*!
 * \file AI.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the AI Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef AI_H
#define AI_H

// ------------------------- Dependencies ------------------------- //

#include "Player.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Difficulty enum type
 * \details Sets up an enum for the difficulty setting of the AI
 * The scale goes from Novice to Beginner to Intermediate to Hard to Pro to Impossible 
 */
typedef enum
{

    Novice = 0,
    Beginner = 1,
    Intermediate = 2,
    Hard = 3,
    Pro = 4,
    Impossible = 5

} Difficulty;

/*!
 * \brief The AI struct stores the difficulty and player
 * \details In this struct the enum for difficulty of the AI is stored of 5 levels
 * There is also the player struct contained here with all the player data and flags
 */
typedef struct
{

    Player* player;            ///< AIs player struct
    
    Difficulty difficulty;     ///< AIs difficulty

} AI;

// ------------------------- Functions ------------------------- //

/// Initializes the AI with a fixed difficulty
AI GetDefaultAI();

/// Returns the player struct from the current AI
Player* GetAIPlayer(const AI* const ai);

/// Returns the difficulty of the AI
Difficulty GetDifficulty(const AI* const ai);

/// Sets the difficulty of the AI
void SetDifficulty(AI* const ai, const Difficulty difficulty);

/// Sets the player of the AI
void SetAIPlayer(AI* const ai, const Player* const player);

#endif

// EOF //
