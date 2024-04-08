/*!
 * \file AIGameplay.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the AIGameplay Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-30
 * \copyright Copyright (c) 2021
 */

#ifndef AIGAMEPLAY_H
#define AIGAMEPLAY_H

// ------------------------- Dependencies ------------------------- //

#include "AI.h"
#include "GameData.h"

// ---------------------- Piece Values ----------------------- //

/// Value for the King
#define KINGVAL 100

/// Value for the Queen
#define QUEENVAL 9

/// Value for the BISHOP
#define BISHOPVAL 3

/// Value for the Knight
#define KNIGHTVAL 3

/// Value for the ROOK
#define ROOKVAL 5

/// Value for the Pawn
#define PAWNVAL 1

// ----------------------------- Types ------------------------- //

/*!
 * \brief Stores th move and rank
 * \details Stores the best move 
 */
typedef struct
{

    Move move;         ///< Move to look at
    uint32_t rank;     ///< How good the move is
    uint8_t value;     ///< How much the move is worth

} RankedMove;

// ------------------------- Functions ------------------------- //

/// Generates the best move for the AI based off the possible moves
Move GenerateBestMove(const GameData* const data, const AI* const ai);

/// Calculates max value and best possible move for white
uint32_t Max(const GameData* const data, const AI* const ai);

// Calculates min value and best possible move for min
uint32_t Min(const GameData* const data, const AI* const ai);

// evaluates total score for each of pieces
double Evaluate(const GameData* const data);

// Determines and returns the predetermined values for each of the pieces on each team
uint32_t GetPieceValue(const Piece piece);

#endif

// EOF //
