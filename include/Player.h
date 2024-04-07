/*!
 * \file Player.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Player Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef PLAYER_H
#define PLAYER_H

// ------------------------- Dependencies ------------------------- //

#include "Moves.h"
#include "main.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Players status
 * \details Stores the data for the player related to whether a player can castle in either direction
 * Also whether theyre in check or not
 */
typedef struct 
{

    bool canCastle[2];     ///< Status of whether the player can castle
    bool InCheck;          ///< Status of whether the player is in check

} PlayerFlags;

/*!
 * \brief Player type
 * \details The player struct stores all the information relating to a player such as its color
 * It also stores an array of pieces it has captured whether it is castling and its own pieces along with its playerflags
 */
typedef struct
{  

    Piece capturedPieces[15];       ///< Array of pieces to be filled by captured pieces
    size_t numcapturedpieces;       ///< Number of pieces the player has captured 
    
    bool isWhite;                   ///< Color of the player

    Index checkpiece;               ///< What piece is putting the player in check

    Index king;                     ///< Location of the king
    
    Index queen[9];                 ///< Location of the queen
    uint8_t numqueens;              ///< The number of queens the player has
    
    Index bishop[10];               ///< Location of the bishops
    uint8_t numbishops;             ///< The number of bishops the player has
    
    Index knight[10];               ///< Location of the knights
    uint8_t numknights;             ///< The number of Knights

    Index rook[10];                 ///< Location of the rooks
    uint8_t numrooks;               ///< The number of rooks

    Index pawn[8];                  ///< Location of the pawns      

    PlayerFlags flags;              ///< Flags for move ability 

} Player;

// ------------------------- Functions ------------------------- //

/// Adds a piece to the players list of captured pieces
void AddCapturedPiece(Player* const player, const Piece piece);

/// Removes a piece matching the given piece from the players list of captured pieces
void RemoveCapturedPiece(Player* const player, const Piece piece);

/// Gets the empty default player
void ResetPlayer(Player* const player, const bool isWhite);

/// Sees if the player is white
bool IsPlayerWhite(const Player* const player);

/// Gets the starting location of a piece
Index GetStartingIndex(const Piece piece);

/// Prompts the player On which color they choose
void PromptPlayerSelection();

/// Gets what color the player chooses
bool GetPlayerSelection();

/// Gets the Number of pieces on the Board
uint8_t GetNumPieces(const Player* const player, const uint8_t pieceid );

/// Remove Piece From a Player
void RemovePiece(Player* const player, const Piece piece);

/// Add a Piece to a Player, return the added player
Piece AddPiece(Player* const player, const uint8_t pieceid, const Index index);

/// Gets the index of a given piece with ID piece ID and number piece number
Index GetPieceLoc(const Player* const player, const uint8_t pieceid, const uint8_t piecenum); 

/// Sets the location of the piece assuming its found to the location
void SetPieceLoc(Player* const player, const Piece piece, const Index location);

/// Checks if the player is in check
bool IsInCheck(const Player* const player);

/// Set whether or not the player is In check
void SetInCheck(Player* const player, const bool val);

/// Checks if the player can castle
bool CanCastle(const Player* const player, const uint8_t whichrook);

/// Set whether or not a player can castle in a direction
void SetCanCastle(Player* const player, const uint8_t rooknum, const bool val);

/// Get the index of the piece putting the player in check
Index GetCheckIndex(const Player* const player);

/// Sets the index of the piece putting a player in check
void SetCheckIndex(Player* const player, const Index index);

#endif

// EOF //