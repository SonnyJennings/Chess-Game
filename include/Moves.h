/*!
 * \file Moves.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Moves Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef MOVES_H
#define MOVES_H

// ------------------------- Dependencies ------------------------- //

#include "Board.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Move structure
 * \details This struct stores the move and the intended starting and ending location
 */
typedef struct
{

    Piece piece;    ///< Piece being moved
    
    Index start;    ///< Starting location of the piece
    Index end;      ///< Ending location of the piece

} Move;

/*!
 * \brief The node to fit in the list
 * \details This struct links the moves together into a list to move back and forth between
 */
typedef struct MoveNode
{
    
    Move move;                 ///< Piece, start and end
    
    struct MoveNode* next;     ///< Next element pointer
    struct MoveNode* prev;     ///< Previous element pointer
    
} MoveNode;

/*!
 * \brief Movestack to track all of the moves
 * \details This struct stores the first and last element of the move list to be able to pop and add moves
 */
typedef struct
{

    MoveNode* head;     ///< Pointer to first element      
    MoveNode* tail;     ///< Pointer to last element
   
    size_t size;        ///< How many elements are in the stack

} MoveStack;

// ------------------------- Functions ------------------------- //
 
/// Creates a move with the parameters
Move CreateMove(const Piece piece, const Index from, const Index to);

/// Gets the index of where the move start
Index GetMoveStart(const Move move);

/// Gets the index of where the move goes to
Index GetMoveEnd(const Move move);

/// Gets the piece involved in a move
Piece GetMovePiece(const Move move);

/// Add a move to the top of a stack
void PushMove(MoveStack* const stack, const Move move);

/// Remove the top move From the stack
void PopMove(MoveStack* const stack);

/// Deletes the movestack
void DeleteMoveStack(MoveStack* const stack);

/// Look at the top move of the stack
Move PeekMove(const MoveStack* const stack);

/// Gets a defaulted movestack (all 0s)
void ResetMoveStack(MoveStack* const stack);

/// Print a move to a file
void fprintmove(FILE* const file, const Move move);

/// Print a move to the stdout
#define PrintMove(move) (fprintmove(stdout, move))

/// Print the movestack to a file
void fprintmovestack(FILE* const file, const MoveStack* const stack);

/// Print the movestack to the standard output
#define PrintMoveStack(stack) fprintmovestack(stdout, stack)

#endif

// EOF //