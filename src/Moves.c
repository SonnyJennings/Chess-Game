/*!
 * \file Moves.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Moves Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-17
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Moves.h"

// ------------------------- Functions ------------------------- //

/// Creates a movenode on the heap
static MoveNode* CreateMoveNode(const Move move);

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Resets the movestack to the default
 * \param stack: The movestack 
 */
void ResetMoveStack(MoveStack* const stack)
{

    stack->head = (MoveNode*)NULL;
    stack->tail = (MoveNode*)NULL;
    stack->size = 0;

}

/*!
 * \brief Creates the move based on the starting and ending point the player selects
 * \param piece: The piece the player selects 
 * \param from: The starting index 
 * \param to: The ending index
 * \returns Move: The completed move
 */
Move CreateMove(const Piece piece, const Index from, const Index to)
{

    Move temp;
    temp.piece = piece;
    temp.end = to;
    temp.start = from; 

    return temp; // The created of the move

}

/*!
 * \brief Creates the movenode based on the move
 * \param move: The move used by the player
 * \returns Move: Movenode* created movenode
 */
static MoveNode* CreateMoveNode(const Move move)
{

    MoveNode* temp = (MoveNode*)calloc(1, sizeof(MoveNode)); // The created blank movenode
    
    temp->move = move; // Sets the move in the movenode
    temp->next = (MoveNode*)NULL;
    temp->prev = (MoveNode*)NULL;

    return temp;

}

/*!
 * \brief Pushes a new move onto the movestack
 * \param stack: Stack to push to
 * \param move: Move to push
 */
void PushMove(MoveStack* const stack, const Move move)
{

    STATIC_ASSERT(stack, "Invalid MoveStack Pointer");

    MoveNode* temp = CreateMoveNode(move); // Creates a new movenode

    if(stack->size) // If there are already moves on the stack
    {

        temp->next = stack->head; // Sets the new movenodes next to be the old top
        stack->head->prev = temp; // Sets the previous previous to be the temp
        stack->head = temp; // Sets the temp to be the new top

    }

    else
    {

        stack->head = temp; // Sets the temp to be the top
        stack->tail = temp; // Sets the temp to be the bottom

    }   
    
    stack->size++; // Increases the stacks size

}

/*!
 * \brief Deletes all of the moves from a movestack
 * \param stack: Stack to delete
 */
void DeleteMoveStack(MoveStack* const stack)
{

    STATIC_ASSERT(stack, "Bad Stack Pointer");

    while(stack->head) PopMove(stack);

    stack->size = 0; // Resets the size
    stack->head = NULL; // Resets the head
    stack->tail = NULL; // Resets the tail

}

/*!
 * \brief Removes the top element of the stack
 * \param stack: Stack to remove the element from
 */
void PopMove(MoveStack* const stack)
{

    STATIC_ASSERT(stack, "Invalid MoveStack Pointer");

    MoveNode* temp = stack->head; // Sets a temporary to hold the top move in the stack
    
    if(!stack->head) return; // If the stack is empty then return

    else if(stack->size == 1) // If theres only one element in the stack
    {

        free(stack->head); // Frees the memory for the head

        stack->head = (MoveNode*)NULL; // Resets the head
        stack->tail = (MoveNode*)NULL; // Resets the tail

    }

    else // If theres more than one element in the stack
    { 

        stack->head->next->prev = (MoveNode*)NULL; // Sets the second in the stack to not a previous
        stack->head = stack->head->next; // Sets the new stacks head to the second in the stack
        free(temp); // Frees the top of the stack

    }

    stack->size--; // Reduces the size of the stack

}

/*!
 * \brief Gets the top move on the stack
 * \param stack: Stack to peek 
 * \returns Move: Move on the top of the stack
 */
Move PeekMove(const MoveStack* const stack)
{

    STATIC_ASSERT(stack, "Invalid MoveStack Pointer");

    if(stack->size)
        return stack->head->move;
    else return (Move){0, 0, 0};

}

/*!
 * \brief Prints a nove to a file
 * \param file: File to print to
 * \param move: Move to print
 */
void fprintmove(FILE* const file, const Move move)
{

    STATIC_ASSERT(file, "Invalid File Pointer");

    char buffer[200]; // Sets a blank string to hold the piece

    strcpy(buffer, (IsPieceWhite(move.piece)? "White ": "Black ")); // Sets buffer to hold the white or black

    switch(GetPieceID(move.piece)) // Piece ID is the piece/2
    { 

        case PAWN:
            strcat(buffer,"Pawn "); // Adds pawn to the name of the move
            break;
        case ROOK:
            strcat(buffer, "Rook "); // Adds rook to the name of the move
            break;
        case KNIGHT:
            strcat(buffer, "Knight "); // Adds knight to the name of the move
            break;
        case BISHOP:
            strcat(buffer, "Bishop "); // Adds bishop to the name of the move
            break;
        case QUEEN: 
            strcat(buffer, "Queen "); // Adds queen to the name of the move
            break;
        case KING:
            strcat(buffer, "King "); // Adds king to the name of the move
            break;
        default:
            break;

    }

    char to[2]; // Blank string to hold where the piece is moving to
    char from[2]; // Blank string to hold where the piece is moving from
    char start[20];
    char end[20];

    IndexToString(move.start, from); // Sets to
    IndexToString(move.end, to); // Sets from

    sprintf(start, "From: %c%c ", from[0], from[1]); // Prints the move
    
    sprintf(end, "To: %c%c \n", to[0], to[1]); // Prints the move

    strcat(buffer, start);
    strcat(buffer, end);

    fprintf(file, "%s", buffer); // Puts the move into the file

}

/*!
 * \brief Prints the stack of moves to a file
 * \param file: File to print to
 * \param stack: Stack to print
 */
void fprintmovestack(FILE* const  file, const MoveStack* const stack)
{

    STATIC_ASSERT(file, "Invalid File Pointer");
    STATIC_ASSERT(stack, "Invalid Stack Handle");

    MoveNode* temp = stack->tail; // Sets the movenode to the bottom of the movestack 

    if(temp)
    {

        for(size_t i = 0; i < stack->size; i++) // For all the moves in the stack
        { 

            fprintmove(file, temp->move); // Prints the list of moves
            temp = temp->prev;
            
        }
    }
}

// EOF //