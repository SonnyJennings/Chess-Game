/*!
 * \file AIGameplay.c
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the AIGameplay Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-30
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "AIGameplay.h"
#include "Gameplay.h"
#include "GameData.h"
#include "Moves.h"
#include "Board.h"
#include "Player.h"
#include "MoveValidation.h"
#include "MoveList.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief The movebranch that holds all the branches information
 * \details The branch holds the root the leaves the ranked moves
 * The number of branches and the worst value of a move
 */
typedef struct leaf
{

    struct leaf* root;           ///< The leaf struct of the previous move
    struct leaf* leaves[20];     ///< The leaf structs of future moves
    RankedMove* move[20];        ///< The Ranked moves connected to the leaves
    uint8_t branches;            ///< The number of moves stored
    int8_t lowestvalue;          ///< The least value of a move

} MoveBranch;

// // ------------------------- Functions ------------------------- //

// /// Creates an initial movebranch
// static MoveBranch* CreateMoveBranch();

// /// Adds a new branch to an existing movebranch
// static void AddBranch(MoveBranch* const moveBranch, const RankedMove* const move);

// /// Resets the lowest value of a branch
// static void setBranchLowestValue(MoveBranch* const movebranch);

// /// Sets a new root to the branch
// static void SetBranchRoot(MoveBranch* const movebranch, const MoveBranch* const root);

// /// Gets the branches root
// static MoveBranch* GetBranchRoot(const MoveBranch* const moveBranch);

// /// Gets the leaves of a branch
// static MoveBranch* GetBranchLeaves(const MoveBranch* const moveBranch, uint8_t i);

// /// Gets the ranked move at an index in the branch
// static RankedMove* GetBranchRankedMove(const MoveBranch* const moveBranch, uint8_t i);

// /// Gets the number of branches in the current branch
// static uint8_t GetBranchNumber(const MoveBranch* const moveBranch);

// /// Gets the branches lowest value
// static uint8_t GetBranchLowestValue(const MoveBranch* const moveBranch);

// /// Creates a new rankedmove
// static RankedMove* CreateRankedMove(const Move move, const uint8_t value, const uint32_t rank);

// /// Sets the value of a rankedmove
// static void SetRankedMoveValue(RankedMove* const rankedMove, const uint8_t value);

// /// Sets the rank of a rankedmove
// static void SetRankedMoveRank(RankedMove* const rankedMove, const uint32_t rank);

// /// Gets the move of a rankedmove
// static Move GetRankedMoveMove(const RankedMove* const rankedMove);

// /// Gets the value of a ranked move
// static uint8_t GetRankedMoveValue(const RankedMove* const rankedMove);

// /// Gets the ranke of a rankedmove
// static uint32_t GetRankedMoveRank(const RankedMove* const rankedMove);

// // ------------------------- Definintions ------------------------- //

/**
 * @brief 
 * 
 * @param data 
 * @param player 
 * @return Move 
 */
static Move NoviceMove(const GameData* data, Player* const player)
{   
    
    STATIC_ASSERT(data, "Invalid Game Data Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    srand(time(NULL));

    MoveList list[1];
    
    while(!GenerateTakingMoves(data, list, player, rand() & 63));

    return GetListMove(list, GetListSize(list) - 1);

}

/*!
 * \brief Generates the best move for the AI
 * \param data: The current gamedata
 * \param ai: The AI struct
 * \returns Move: The best possible move
 */
Move GenerateBestMove(const GameData* const data, const AI* const ai)
{

    return NoviceMove(data, ai->player);

}

// /*!
//  * \brief Generates the best move for the AI with the most value
//  * \param data: The current gamedata
//  * \param ai: The AI struct
//  * \returns ?
//  */
// uint32_t Max(const GameData* const data, const AI* const ai)
// {
//     STATIC_ASSERT(data, "Invalid Game Data Pointer");
//     STATIC_ASSERT(ai, "Invalid AI Pointer");

//     //MoveBranch branch;

//     Player* next = IsPlayerWhite(ai->player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE);

//     MoveList queen, king, bishop[2], rook[2], knight[2], pawn[8];

//     GenerateTakingMoves(data, &queen, ai->player, GetPieceLoc(next, QUEEN, 0)); 
//     GenerateTakingMoves(data, &king, ai->player, GetPieceLoc(next, KING, 0));
    
//     for(uint8_t i = 0; i < 2; i++)
//     {
//         GenerateTakingMoves(data, &bishop[i], ai->player, GetPieceLoc(next, BISHOP, i));
//         GenerateTakingMoves(data, &rook[i], ai->player, GetPieceLoc(next, ROOK, i));
//         GenerateTakingMoves(data, &knight[i], ai->player, GetPieceLoc(next, KNIGHT, i));   
//     }

//     for(uint8_t i = 0; i < 8; i++)
//         GenerateTakingMoves(data, &pawn[i], ai->player, GetPieceLoc(next, PAWN, i));


//     return 0;


// }

// /*!
//  * \brief Generates the worst move for the AI with the least value by the opponent
//  * \param data: The current gamedata
//  * \param ai: The AI struct
//  * \returns ?
//  */
// uint32_t Min(const GameData* const data, const AI* const ai)
// {

//     return 0;
    
// }

// /*!
//  * \brief ?
//  * \param data: The current gamedata
//  * \returns ?
//  */
// double Evaluate(const GameData* const data)
// {

//     return 0.0f;

// }

/*!
 * \brief Predetermines values for each players piece used for minmax algorithm
 * \param piece player piece
 * \returns Determined piece value
 */
uint32_t GetPieceValue(const Piece piece)
{

    switch (GetPieceID(piece))
    {

        case PAWN: return PAWNVAL;
        case KNIGHT: return KNIGHTVAL;
        case BISHOP: return BISHOPVAL;
        case ROOK: return ROOKVAL;
        case QUEEN: return QUEENVAL;
        case KING: return KINGVAL;    
        default: return 0;

    }
}

// /*!
//  * \brief Initializes a blank movebranch
//  * \returns MoveBranch*: Returns a pointer to the created movebranch
//  */
// MoveBranch* CreateMoveBranch()
// {

//     MoveBranch* moveBranch = malloc(sizeof(MoveBranch)); // The movebranch

//     moveBranch->root = NULL; // There is no root when initialized
//     moveBranch->leaves[20]; // The blank leaves
//     moveBranch->move[20]; // The blank move slots
//     moveBranch->branches = 0; // The number is set to 0 on initialization
//     moveBranch->lowestvalue = -128; // The minimum possible value of a move

//     return moveBranch;

// }

// uint8_t MaxValue(const int* const array, uint8_t size)
// {

// }

// /*!
//  * \brief Adds a branch to the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \param move: The rankedmove intended to be added
//  */
// void AddBranch(MoveBranch* const moveBranch, const RankedMove* const move)
// {

//     MoveBranch* leaf = CreateMoveBranch(); // Creates a new branch to be added

//     leaf->root = moveBranch; // The root of the new branch is the current leaf
    
//     uint8_t branches = GetBranchNumber(moveBranch); // The number of branches in the current movebranch
//     int8_t lowestValue = GetBranchLowestValue(moveBranch); // The lowest value of any move in the current branch

//     if(branches < 20) // If there are less than 20 branches
//     {   

//         moveBranch->move[branches] = move; // Adds the new move at the end of the array
//         moveBranch->leaves[branches] = leaf; // Adds the new leaf at the end of the array

//         if(lowestValue > GetRankedMoveValue(move)) moveBranch->lowestvalue = GetRankedMoveValue(move); // If the value of the move being added is lower than the lowes value thats the new lowest value
        
//         moveBranch->branches++; // increments the number of branches

//     }

//     else // If theres 20 branches
//     {    

//         for(uint8_t i = 0; i < branches; i++) // Scans through all branches in the movebranch
//         {
//             if(GetRankedMoveValue(GetBranchRankedMove(moveBranch, i)) == lowestValue) // If the move has the lowest value
//             {

//                 moveBranch->move[i] = move; // Replace the move
//                 moveBranch->leaves[i] = leaf; // Replace the branch
//                 setBranchLowestValue(moveBranch); // Resets the lowest value of the branch

//             }
//         }
//     }    
// }

// /*!
//  * \brief Resets the lowest value of a branch
//  * \param moveBranch: The currnet movebranch
//  */
// void setBranchLowestValue(MoveBranch* const movebranch)
// {
    
//     int8_t lowest = GetRankedMoveValue(GetBranchRankedMove(movebranch, 0)); // Gets the value of the first move

//     for(uint8_t i = 1; i < GetBranchNumber(movebranch); i++) // Scans through each branch
//     {

//         if(GetRankedMoveValue(GetBranchRankedMove(movebranch, 0)) < lowest) // If the branch has a lower value than the previous ones
//             lowest = GetRankedMoveValue(GetBranchRankedMove(movebranch, 0)); // Set the new lowest

//     }
// }

// /*!
//  * \brief Sets the root of the current branch
//  * \param moveBranch: The currnet movebranch
//  */
// void SetBranchRoot(MoveBranch* const movebranch, const MoveBranch* const root)
// {

//     movebranch->root = root;

// }

// /*!
//  * \brief Returns the root of the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \returns MoveBranch*: The pointer to the root
//  */
// MoveBranch* GetBranchRoot(const MoveBranch* const moveBranch)
// {

//     return moveBranch->root;

// }

// /*!
//  * \brief Returns the leaves of the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \param i: The index of the leaf to be grabbed
//  * \returns MoveBranch*: The pointer to the root
//  */
// MoveBranch* GetBranchLeaves(const MoveBranch* const moveBranch, uint8_t i)
// {

//     return moveBranch->leaves[i];

// }

// /*!
//  * \brief Returns the moves of the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \param i: The index of the leaf to be grabbed
//  * \returns RankedMove*: The pointer to the RankedMove
//  */
// RankedMove* GetBranchRankedMove(const MoveBranch* const moveBranch, uint8_t i)
// {

//     return moveBranch->move[i];

// }

// /*!
//  * \brief Returns the number of branches of the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \returns uint8_t: The number of branches
//  */
// uint8_t GetBranchNumber(const MoveBranch* const moveBranch)
// {

//     return moveBranch->branches;

// }

// /*!
//  * \brief Returns the lowest value of a move of the current movebranch
//  * \param moveBranch: The currnet movebranch
//  * \returns uint8_t: The value of the lowest move
//  */
// uint8_t GetBranchLowestValue(const MoveBranch* const moveBranch)
// {

//     return moveBranch->lowestvalue;

// }

// /*!
//  * \brief Creates a rankedmove
//  * \param move: The move for the rankedmove
//  * \param value: The value of the rankedmove
//  * \param rank: The value of the branch of the move
//  * \returns RankedMove*: A pointer to the created rankedmove
//  */
// RankedMove* CreateRankedMove(const Move move, const uint8_t value, const uint32_t rank)
// {

//     RankedMove* temp;
//     temp->move = move;
//     temp->value = value;
//     temp->rank = rank;

//     return temp;

// }

// /*!
//  * \brief Sets the value of a rankedmove
//  * \param RankedMove: The current rankedmove
//  * \param value: The value to set to
//  */
// void SetRankedMoveValue(RankedMove* const rankedMove, const uint8_t value)
// {

//     rankedMove->value == value;

// }

// /*!
//  * \brief Sets the rank of a rankedmove
//  * \param RankedMove: The current rankedmove
//  * \param rank: The value to add
//  */
// void SetRankedMoveRank(RankedMove* const rankedMove, const uint32_t rank)
// {

//     rankedMove->rank += rank;

// }

// /*!
//  * \brief Gets the move of the ranked move
//  * \param RankedMove: The current rankedmove
//  * \returns Move: The move
//  */
// Move GetRankedMoveMove(const RankedMove* const rankedMove)
// {

//     return (rankedMove->move);

// }

// /*!
//  * \brief Gets the value of the ranked move
//  * \param RankedMove: The current rankedmove
//  * \returns uint8_t: The value of the move
//  */
// uint8_t GetRankedMoveValue(const RankedMove* const rankedMove)
// {

//     return (rankedMove->value);

// }

// /*!
//  * \brief Gets the rank of the ranked move
//  * \param RankedMove: The current rankedmove
//  * \returns uint32_t: The rank of the move
//  */
// uint32_t GetRankedMoveRank(const RankedMove* const rankedMove)
// {

//     return (rankedMove->rank);

// }

// // EOF //
