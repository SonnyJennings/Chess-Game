/*!
 * \file Player.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Player Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-17
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Player.h"

// ------------------------- Functions ------------------------- //

/// Sets the pieces back to their origial locations
static void SetDefaultPieceLoc(Player* const player, const bool isWhite);

/// Increments the number of pieces
static void IncNumPieces(Player* const player, const uint8_t pieceid);

/// Decrements the number of pieces
static void DecNumPieces(Player* const player, const uint8_t pieceid);

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Gets the default player for the color 
 * \param isWhite: If the player is white
 * \returns Player: The default player of the color
 */
 void ResetPlayer(Player* const player, const bool isWhite)
{

    for(uint8_t i = 0; i < 15; i++) // Loops through the captured pieces
        player->capturedPieces[i] = EMPTY; // Sets the captured pieces to be empty
    
    player->numcapturedpieces = 0; // Sets the amount f captured pieces to be empty

    player->isWhite = isWhite; // Sets the player to be a color inputed

    SetDefaultPieceLoc(player, isWhite); // Sets pieces to their default locations

    player->flags.canCastle[0] = true; // Resets Castle flag
    player->flags.canCastle[1] = true; // Resets Castle flag

    player->flags.InCheck = false; // Resets incheck flag

    player->numbishops = 2; // set the default number of pieces
    player->numknights = 2;
    player->numqueens  = 1;
    player->numrooks = 2;

}

/*!
 * \brief Gets the default player piece locations for the color 
 * \param isWhite: If the player is white
 * \returns Player: The default player of the color
 */
void SetDefaultPieceLoc(Player* const player, const bool isWhite)
{
    STATIC_ASSERT(player, "Invalid Player Pointer");

    if(isWhite) // If the player is white
    {

        player->king = 4;
        player->queen[0] = 3;
        player->bishop[0] = 2;
        player->bishop[1] = 5;
        player->knight[0] = 1;
        player->knight[1] = 6;
        player->rook[0] = 0;
        player->rook[1] = 7;
        player->pawn[0] = 8;
        player->pawn[1] = 9;
        player->pawn[2] = 10;
        player->pawn[3] = 11;
        player->pawn[4] = 12;
        player->pawn[5] = 13;
        player->pawn[6] = 14;
        player->pawn[7] = 15;

    }

    else // If the player is black
    {

        player->king = 60;
        player->queen[0] = 59;
        player->bishop[0] = 58;
        player->bishop[1] = 61;
        player->knight[0] = 57;
        player->knight[1] = 62;
        player->rook[0] = 56;
        player->rook[1] = 63;
        player->pawn[0] = 48;
        player->pawn[1] = 49;
        player->pawn[2] = 50;
        player->pawn[3] = 51;
        player->pawn[4] = 52;
        player->pawn[5] = 53;
        player->pawn[6] = 54;
        player->pawn[7] = 55;

    }
}

/*!
 * \brief Gets the index of the piece that is putting the player in check
 * \param player: Player that is in check
 * \returns Index: Index of piece that Is checking the player 
 */
 Index GetCheckIndex(const Player* const player)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    return player->checkpiece;

}

/*!
 * \brief Get the Num Pieces object
 * \param player: The player to check
 * \param pieceid: The piece ID for the type of piece
 * \return uint8_t: The number of pieces
 */
uint8_t GetNumPieces(const Player* const player, const uint8_t pieceid )
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    switch(pieceid)
    {

        case ROOK: return player->numrooks;
        case KNIGHT: return player->numknights;
        case BISHOP: return player->numbishops;
        case QUEEN: return player->numqueens;
        case PAWN: return 8;
        case KING: return 1;
        default: return 0;

    }
}

/*!
 * \brief Increments the number of pieces
 * \param player: The player to increment
 * \param pieceid: The type of piece to increment
 */
void IncNumPieces(Player* const player, const uint8_t pieceid)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    switch(pieceid)
    {

        case ROOK: 
            player->numrooks++;
            break;
        case KNIGHT: 
            player->numknights++;
            break;
        case BISHOP: 
            player->numbishops++;
            break;
        case QUEEN: 
            player->numqueens++;
            break;

        default: break;

    }
}

/*!
 * \brief Decrements the numebr of pieces
 * \param player: The player to change
 * \param pieceid: The piece ID to decrement
 */
void DecNumPieces(Player* const player, const uint8_t pieceid)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    switch(pieceid)
    {

        case ROOK: 
            player->numrooks--;
            break;
        case KNIGHT: 
            player->numknights--;
            break;
        case BISHOP: 
            player->numbishops--;
            break;
        case QUEEN: 
            player->numqueens--;
            break;
        default: break;

    }
}

/*!
 * \brief Removes a piece from the board and decrements it
 * \param player: the player to remove the piece from
 * \param piece: The piece to remove
 */
void RemovePiece(Player* const player, const Piece piece)
{

    uint8_t piecenum = GetPieceNumber(piece); // Get the piece number of the one to be removed
    
    SetPieceLoc(player, piece, 0xff); // Sets the piece location to off the board
    
    switch(GetPieceID(piece))
    {
        case QUEEN: 
            if(piecenum > 1) DecNumPieces(player, QUEEN); // If more than one queen decrement
            break;
        case KNIGHT:
        case BISHOP:
        case ROOK:
            if(piecenum > 2) DecNumPieces(player, GetPieceID(piece)); // If more than one of anything else decrement
            break;
        default: break;

    }
}

/*!
 * \brief Adds a piece to the board
 * \param player: The player to have the piece
 * \param pieceid: The piece ID
 */
Piece AddPiece(Player* const player, const uint8_t pieceid, const Index index)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    Piece piece = CreatePiece(IsPlayerWhite(player), pieceid, GetNumPieces(player, pieceid)); // Creates the piece

    switch(pieceid) // Puts it at the right index
    {

        case ROOK: 
            player->rook[player->numrooks] = index;
            break;
        case KNIGHT: 
            player->knight[player->numknights] = index; 
            break;
        case BISHOP: 
            player->bishop[player->numbishops] = index;
            break;
        case QUEEN: 
            player->queen[player->numqueens] = index;
            break;
        default: break;

    }
    
    IncNumPieces(player, pieceid); // Increments the number of pieces

    return piece;

}

/*!
 * \brief Sets the index of the piece that is checking the player
 * \param player: Player to set index of
 * \param index: Index of checking piece
 */
 void SetCheckIndex(Player* const player, const Index index)
{
    
    STATIC_ASSERT(player, "Invalid Player Pointer");

    player->checkpiece = index;

}

/*!
 * \brief Checks if the Players check flag is set
 * \param player: Player to look in
 * \returns bool: If the players in check flag is set
 */
 bool IsInCheck(const Player* const player)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    return player->flags.InCheck;

}

/*!
 * \brief Sets a players check flag
 * \param player: Player to set check flag 
 * \param val: The value of the flag
 */
 void SetInCheck(Player* const player, const bool val)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    player->flags.InCheck = val;

}

/*!
 * \brief Gets the default starting position of a given piece
 * \param piece: Piece to get starting position of
 * \returns Index: Starting position of the piece
 */
Index GetStartingIndex(const Piece piece)
{

    bool isWhite = IsPieceWhite(piece); // Stores whether a piece is white or not
    Player defplayer; // Temporary player struct

    SetDefaultPieceLoc(&defplayer, isWhite); // Sets default piece locations
    
    uint8_t piecenumber = GetPieceNumber(piece); // Sets the piece number of the piece
    
    switch(GetPieceID(piece))
    {

        case PAWN: return defplayer.pawn[piecenumber];
        case KNIGHT: return defplayer.knight[piecenumber];
        case KING: return defplayer.king;
        case QUEEN: return defplayer.queen[piecenumber];
        case ROOK: return defplayer.rook[piecenumber];
        case BISHOP: return defplayer.bishop[piecenumber];
        default: return INDEX_MAX;

    }
}

/*!
 * \brief Adds a piece to a players list of captured pieces 
 * \param player: Player whose list to add to
 * \param piece: Piece to add
 */
void AddCapturedPiece(Player* const player, const Piece piece)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    player->capturedPieces[player->numcapturedpieces] = piece; // Puts a captured piece into the array of captured pieces
    player->numcapturedpieces++; // Increases the number of captured pieces

}

/*!
 * \brief Removes a piece from a players list of captured pieces
 * \param player: Player whose list to pull from
 * \param piece: Piece to pull from the players list
 */
void RemoveCapturedPiece(Player* const player, const Piece piece)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    for(uint8_t i = 0; i < player->numcapturedpieces; i++) // Loops through all the captured pieces
    {

        if(piece == player->capturedPieces[i]) // If an element matches the piece, replace it with the last element
        {         

            player->capturedPieces[i] = player->capturedPieces[player->numcapturedpieces - 1]; // Replaces with the last element in the list
            player->numcapturedpieces--; // Decremants the number of captured pieces
            return;

        }
    }
}

/*!
 * \brief Checks if a player is white
 * \param player: The player in question
 * \returns bool: boolean value for if a player is white or not
 */
bool IsPlayerWhite(const Player* const player)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    return player->isWhite;

}

/*!
 * \brief Checks to see if a player can castle in a certain direction
 * \param player: The player in question
 * \param rooknum: The rook of the player in question
 * \returns bool: boolean value for if the player can castle
 */
bool CanCastle(const Player* const player, const uint8_t rooknum)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    return player->flags.canCastle[rooknum];

}

/*!
 * \brief Sets whether the player can castle
 * \param player: Player to be set
 * \param rooknum: Rook to be set
 * \param val: Whether they can or cant castle
 */
 void SetCanCastle(Player* const player, const uint8_t rooknum, const bool val)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    player->flags.canCastle[rooknum] = val;

}

/*!
 * \brief prompts whether the player wants to be white or black
 */
 void PromptPlayerSelection()
{

    printf("Would you like to be white? (y/n): ");

}

/*!
 * \brief Gets the location of a certain piece
 * \param player: The player of the piece
 * \param pieceid: The pieces ID
 * \param piecenum: The pieces number
 * \returns Index: The location of the piece
 */
 Index GetPieceLoc(const Player* const player, const uint8_t pieceid, const uint8_t piecenum)
{
    
    STATIC_ASSERT(player, "Invalid Player Pointer");

    if(piecenum > 10) return 0xff;

    switch(pieceid)
    {

        case PAWN: return player->pawn[piecenum];
        case ROOK: return player->rook[piecenum];
        case BISHOP: return player->bishop[piecenum];
        case KNIGHT: return player->knight[piecenum];
        case QUEEN: return player->queen[piecenum];
        case KING: return player->king;
        
        default: return 0xff;

    }
}

/*!
 * \brief Sets the location of a piece
 * \param player: The player of the piece
 * \param piece: The piece that wants to move
 * \param location: The new location of the piece
 */
 void SetPieceLoc(Player* const player, const Piece piece, const Index location)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    uint8_t num = GetPieceNumber(piece); // Set to the numnber of the piece to be moved

    switch (GetPieceID(piece))
    {

        case PAWN: 
            player->pawn[num] = location;
            break;
        case BISHOP:
            player->bishop[num] = location;
            break;
        case KNIGHT:
            player->knight[num] = location;
            break;
        case ROOK:
            player->rook[num] = location;
            break;
        case QUEEN:
            player->queen[num] = location;
            break;
        case KING:
            player->king = location;
            break;
            
        default: break;

    }
}

/*!
 * \brief Gets the player to input which side they want to be on 
 * \returns bool: A boolean for their selection if white or black
 */
 bool GetPlayerSelection()
{

    char selection;

    if(scanf(" %c", &selection) && selection == 'y') return true; // If they say y they are white and returns true
    
    return false;

}

// EOF //