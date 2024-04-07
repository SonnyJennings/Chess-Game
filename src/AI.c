/*!
 * \file AI.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the AI Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "AI.h"

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Initializes the AI with a fixed difficulty
 * \returns ai: Returns novice level AI
 */
AI GetDefaultAI()
{

    return (AI){(Player*)NULL, Novice};
    
}

/*!
 * \brief Gets the player of the AI
 * \param ai: Takes in the AI
 * \returns Player*: returns the player struct of the AI
 */
Player* GetAIPlayer(const AI *const ai)
{

    return ai->player;

}

/*!
 * \brief Gtes the difficulty of the AI
 * \param ai: Takes in the AI struct
 * \returns Difficulty: Returns the difficulty of the AI
 */
Difficulty GetDifficulty(const AI *const ai)
{

    return ai->difficulty;

}

/*!
 * \brief Sets the difficulty of the AI
 * \param ai: Takes in the AI
 * \param difficulty: Takes in a difficulty level
 */
void SetDifficulty(AI *const ai, const Difficulty difficulty)
{

    ai->difficulty = difficulty;

}

/*!
 * \brief Sets the player struct of the AI
 * \param ai: Takes in the AI struct
 * \param player: Takes in a player struct
 */
void SetAIPlayer(AI *const ai, const Player *const player)
{

    ai->player = (Player *)player;
    
}

// EOF //