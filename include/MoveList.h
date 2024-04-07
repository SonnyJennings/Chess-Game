/*!
 * \file MoveList.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Menu Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-24
 * \copyright Copyright (c) 2021
 */  

#ifndef MOVELIST_H
#define MOVELIST_H

// --------------------------------- Dependencies -------------------------------- //

#include "Moves.h"
#include "GameData.h"

// ------------------------------------- Types ----------------------------------- //

/*!
 * \brief The struct for the move list stores the index and size
 * \details This stores the actual list of possible moves that a selected piece can make
 * Stored by an array of indexes and a size of the list
 */
typedef struct
{

    Move move[28];       ///< 1 Move for Each Piece Max
    size_t size;         ///< Size of list

} MoveList;

// --------------------------------------- Functions ------------------------------------ //

/// Fills a movelist with all of the possible moves for the piece at the index
uint8_t GenerateMoveList(const GameData* const data, MoveList* const list, Player* const player, const Piece piece);

uint8_t GenerateValidMoves(const GameData* const data, MoveList* list, const Player* const player, const Index index);

/// Generates a list of moves that takes a specific index
uint8_t GenerateTakingMoves(const GameData* const data, MoveList* list, Player* const player, const Index index);

/// Generates all valid moves
uint8_t GenerateValidMoves(const GameData* const data, MoveList* list, const Player* const player, const Index index);

/// Appends a move to the end of the moveList
void AppendMove(MoveList* const list, const Move move);

/// Clears a move list
void ClearMoveList(MoveList* const list);

/// Gets a move from the list 
Move GetListMove(const MoveList* const list, const uint8_t index);

/// Gets how many elements are in a list
uint8_t GetListSize(const MoveList* const list);

/// Fills a move list with possible moves to block the move
uint8_t GenerateBlockingMoves(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock);

#endif