/*!
 * \file Gameplay.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Gameplay Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-17
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Gameplay.h"
#include "MoveValidation.h"
#include "MoveList.h"
#include "Menu.h"

// ------------------------- Functions ------------------------- //

/// Prompts the user for the promotion
static void PromptPromotion();

static void PromotePawn(GameData* const data, Player* const player, const Index index);

// ------------------------- Definintions ------------------------- //

/*!
 * \brief  Checks if the Piece Belongs to The Player
 * \param  player: Player that Should Own Piece
 * \param  piece: Piece Thst Should be Owned
 * \returns bool If the Piece is owned by the Player
 */
bool IsValidPiece(const Player* const player, const Piece piece)
{

    STATIC_ASSERT(player, "Invalid Player Pointer");

    if(piece == EMPTY) return false; // If theres no piece return false

    return IsPieceWhite(piece) == IsPlayerWhite(player); // Returns true if piece is owned by player false otherwise

}

/*!
 * \brief Checks a board for how many Valid moves are available for a given Player
 * \param data: Gamedata for the Current Game
 * \returns uint32_t: Number of valid moves
 */
uint32_t GetNumberOfValidMoves(const GameData* const data, Player* const player)
{

    STATIC_ASSERT(data, "Invalid Game Data Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    uint8_t queen = 0;
    uint8_t rook = 0;
    uint8_t pawn = 0;
    uint8_t bishop = 0;
    uint8_t king = 0;
    uint8_t knight = 0;

    MoveList list[1]; // list to hold all of the moves

    for(uint8_t i = 0; i < 8; i++) // Goes through all the pawns
        pawn += GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), PAWN, i)); // Checks the number of moves and creates a list of moves for the pawns

    for(uint8_t i = 0; i < GetNumPieces(player, BISHOP); i++) // Goes through all the bishops
        bishop += GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), BISHOP, i)); // Checks the number of moves and creates a list of moves for the bishops

    for(uint8_t i = 0; i < GetNumPieces(player, QUEEN); i++) // Goes through all the queens
        queen += GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), QUEEN, i)); // Checks the number of moves and creates a list of moves for the queens

    for(uint8_t i = 0; i < GetNumPieces(player, ROOK); i++) // Goes through all the rooks
        rook += GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), ROOK, i)); // Checks the number of moves and creates a list of moves for the rooks

    for(uint8_t i = 0; i < GetNumPieces(player, KNIGHT); i++) // Goes through all the knights
        knight += GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), KNIGHT, i)); // Checks the number of moves and creates a list of moves for the knights     

    king = GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), KING, 0)); // Checks the number of moves and creates a list of moves for the king
    
    return knight + pawn + king + queen + rook + bishop;

}

/**
 * @brief 
 * 
 * @param data 
 * @param player 
 * @return true 
 * @return false 
 */
bool AreValidMoves(const GameData* const data, Player* const player)
{
    MoveList list[1];

    for(uint8_t i = 0; i < 8; i++) // Goes through all the pawns
        if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), PAWN, i))) return true; // Checks the number of moves and creates a list of moves for the pawns

    for(uint8_t i = 0; i < GetNumPieces(player, BISHOP); i++) // Goes through all the bishops
        if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), BISHOP, i))) return true; // Checks the number of moves and creates a list of moves for the bishops

    for(uint8_t i = 0; i < GetNumPieces(player, QUEEN); i++) // Goes through all the queens
        if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), QUEEN, i))) return true; // Checks the number of moves and creates a list of moves for the queens

    for(uint8_t i = 0; i < GetNumPieces(player, ROOK); i++) // Goes through all the rooks
        if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), ROOK, i))) return true; // Checks the number of moves and creates a list of moves for the rooks

    for(uint8_t i = 0; i < GetNumPieces(player, KNIGHT); i++) // Goes through all the knights
        if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), KNIGHT, i))) return true; // Checks the number of moves and creates a list of moves for the knights     

    if(GenerateMoveList(data, list, player, CreatePiece(IsPlayerWhite(player), KING, 0))) return true; // Checks the number of moves and creates a list of moves for the king

    return false;

}

/*!
 * \brief takes in the data and player and returns status of if the player is in checkmate
 * \param data: Current gamedata
 * \param player: The player
 * \returns bool: of whether the Index is in checkmate
 */
bool IsInCheckMate(const GameData* const data, Player* const player)
{
    
    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    Piece piece = IsPlayerWhite(player)? WKING: BKING;
    
    if(!IsInCheck(player)) return false; // if the other player can't attack the square we are good
    
    MoveList list[1]; // Blank list for moves
    
    if(GenerateMoveList(data, list, player, piece)) 
        return false; // Check if the king can move out of the way

    if(GenerateTakingMoves(data, list, player, GetCheckIndex(player))) 
        return false; // Check if there is a way to take the checking piece without going into check
    
    Move move;
    move.piece = GetPiece(GetBoard(data), GetCheckIndex(player));
    move.start = GetCheckIndex(player);
    move.end = GetPieceLoc(player, KING, 0);  

    // Check if the player can block the attacking piece
    if(GenerateBlockingMoves(data, list, player, move)) 
        return false; // Checks if theres a way to block the checkmate

    return true;

}

/*!
 * \brief takes in the data and player and returns staus of if the player is in stalemate
 * \param data: Current gamedata
 * \param player: The player
 * \returns bool: of whether the player is in stalemate
 */
bool IsInStalemate(const GameData* const data, Player* const player)
{

    if(IsInCheck(player)) return false;

    if(AreValidMoves(data, player)) 
        return false; // If there are valid moves for a player

    return true;

}

/*!
 * \brief takes in the data and player and returns status of if the player is in lack of mating material
 * \param data: Current gamedata
 * \param player: The player
 * \returns bool: of whether there is a lack of mating material
 */
bool IsLackOfMaterial( Player* const player)
{

    uint8_t counter = 0;

    for(uint8_t i = 0; i < 8; i++) // Goes through all the pawns
        if(GetPieceLoc(player, PAWN, i) < 64) return false; // Checks how many pieces there are of pawns if any return false

    for(uint8_t i = 0; i < GetNumPieces(player, BISHOP); i++) // Goes through all the bishops
        if(GetPieceLoc(player, BISHOP, i) < 64) counter++; // Checks how many pieces there are of bishops if any up the counter

    for(uint8_t i = 0; i < GetNumPieces(player, QUEEN); i++) // Goes through all the queens
        if(GetPieceLoc(player, QUEEN, i) < 64) return false; // Checks how many pieces there are of queens if any return false

    for(uint8_t i = 0; i < GetNumPieces(player, ROOK); i++) // Goes through all the rooks
        if(GetPieceLoc(player, ROOK, i) < 64) return false; // Checks how many pieces there are of rooks if any return false

    for(uint8_t i = 0; i < GetNumPieces(player, KNIGHT); i++) // Goes through all the knights
        if(GetPieceLoc(player, KNIGHT, i) < 64) counter++; // Checks how many pieces there are of knights if any up the counter

    return(counter < 2);

}

/*!
 * \brief Gets the Status of the Game
 * \param data: Current gamedata
 * \param player: The player
 * \returns Status: Status Of the Game
 */
Status GetGameStatus(const GameData* const data, Player* const player)
{

    Player* next = IsPlayerWhite(player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE);

    if(IsInCheckMate(data, player)) // if the player is in checkmate return who won
        return IsPlayerWhite(player)? BlackWins: WhiteWins;

    if(IsInStalemate(data, next)) 
        return StaleMate; // If its a stalemate its a draw

    if(IsLackOfMaterial(player) && IsLackOfMaterial(next)) 
        return Tie; // If theres not enough mating material its a draw

    if(GetTurnCounter(data) == 50) 
        return Tie; // If turn counter hits 50 its a tie

    return InProgress;
    
}

/*!
 * \brief takes in the current players move and if a piece is captured adds it to the captured list and then completes the move
 * \param data: the game board
 * \param move: the intended move of the player
 * \returns Move: the completed move 
 */
Move MakeMove(GameData* const data, Player* const player, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(player, "Invalid Current Player Pointer");
    STATIC_ASSERT(GetMoveStack(data), "Invalid Stack Pointer");

    SetInCheck(player, false); // Reset the Check Flag
    SetCheckIndex(player, INDEX_MAX); // Reset the check index

    bool castling = false;
    bool inenpassant = false;
    bool inpromotion = false;

    Piece topiece = GetPiece(GetBoard(data), move.end); // The piece you are attacking
    Player* next = IsPlayerWhite(player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // The other player

    switch(GetPieceID(move.piece)) // If you move a king or a rook that piece cant castle
    {

        case KING:
            SetCanCastle(player, 0, false);
            SetCanCastle(player, 1, false);
            break;
        case ROOK:
            SetCanCastle(player, GetPieceNumber(move.piece), false);
            break;
        case PAWN:
            ResetTurnCounter(data); // Resets turn counter on pawn move
            break;
        default: break;

    }

    if(topiece)
    {

        AddCapturedPiece(player, topiece); // Adds the captured piece to the list of captured pieces
        RemovePiece(next, topiece); // Removes the piece
        ResetTurnCounter(data); // Resets the turn counter if you capture a piece

    }

    Move temp = PeekMove(GetMoveStack(data)); // Looks at the previous move
    
    inenpassant = GetPieceID(temp.piece) == PAWN && abs(GetRow(temp.start) - GetRow(temp.end)) == 2 && GetColumn(temp.end) == GetColumn(move.end) && GetPieceID(move.piece) == PAWN && ((GetRow(move.end) == 5 && GetRow(temp.end) == 4) || (GetRow(move.end) == 2 && GetRow(temp.end) == 3)); // Checks if you just en passanted
    castling = GetPieceID(move.piece) == KING && abs(GetColumn(move.start) - GetColumn(move.end)) == 2; // if the king castles
    inpromotion = GetPieceID(move.piece) == PAWN && (GetRow(move.end) == 7 || GetRow(move.end) == 0); // if the pawn reached the end of the board

    PushMove(GetMoveStack(data), move); // Puts the new move onto the stack
    
    SetPiece(GetBoard(data), move.end, move.piece); // Set the piece at the end location of the move 
    SetPiece(GetBoard(data), move.start, EMPTY); // Set the piece of the index where it came from to 0
    SetPieceLoc(player, move.piece, move.end); // Update the location in the player

    if(castling)
    {

        uint8_t rooknum = (move.end - move.start == 2)? 1: 0; // Selects which rook is castling
        int8_t direction = (move.end - move.start == 2)? 1: -1; // Selects the direction of castling
        Piece rook = CreatePiece(IsPlayerWhite(player), ROOK, rooknum); // Creates the new rook

        SetPiece(GetBoard(data), GetPieceLoc(player, ROOK, rooknum), EMPTY); // Set the old rook space to empty
        SetPiece(GetBoard(data), move.start + direction, rook); // Move the rook to its new space
        SetPieceLoc(player, rook, move.start + direction); // Set the rook location to be where is now is

    }

    if(inenpassant)
    {

        SetPiece(GetBoard(data), temp.end, EMPTY); // If in en passant capture the piece
        SetPieceLoc(next, temp.piece, INDEX_MAX); // Set the piece to be index_max for the enemy
        AddCapturedPiece(player, temp.piece); // Add the captured piece to the list
        
    }
    
    if(inpromotion) PromotePawn(data, player, move.end); // If the pawn made it to the end promote it

    Index king = GetPieceLoc(next, KING, 0); // The opponents king
    
    if(IsValidMove(data, player, CreateMove(move.piece, move.end, king))) // If its a valid move to attack the king
    {
        
        SetInCheck(next, true); // Sets the opponent into check
        SetCheckIndex(next, move.end); // Set the check index to the piece thats attacking it

    }
    
    return move;
}

/*!
 * \brief Replaces A Pawn With a More Valuable Piece
 * \param data: Current gamedata
 * \param player: The player of the piece
 * \param index: Index to promote at
 */
void PromotePawn(GameData* const data, Player* const player, const Index index)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    uint8_t option = 0; // The selection for promoting the pawn
    
    do
    {

        PromptPromotion(); // The promotion message
        getchar(); // Gets rid of the new line 
        option = toupper(getchar()); // The user selection
    
    } while (option != 'Q' && option != 'R' && option != 'B' && option != 'N' && puts("\nInvalid selection, try again\n")); // If its a bad input retry

    Piece newpiece = AddPiece(player, option, index); // the New piece
    Piece oldpiece = GetPiece(GetBoard(data), index); // The old pawn

    SetPiece(GetBoard(data), index, newpiece); // Sets the new piece
    SetPieceLoc(player, oldpiece, INDEX_MAX); // Removes the old pawn

}

/*!
 * \brief Prompts the User to Promote the Pawn
 */
void PromptPromotion()
{

    puts("What piece would you like to promote the pawn too?\n");
    puts("\tQ: Queen");
    puts("\tR: Rook");
    puts("\tN: Knight");
    puts("\tB: Bishop\n");
    printf("Please select an option: ");

}

/*!
 * \brief Checks if Making a Move Puts a Player In Check
 * \param data: Current Game Data
 * \param player: Player Making the Move
 * \param move: Move to Check
 * \returns bool: If the Move Puts you in Check
 */
bool MovesIntoCheck(const GameData* const data, Player* const player, const Move move)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    Player* nextplayer = IsPlayerWhite(player)? GetPlayer(data, BLACK): GetPlayer(data, WHITE); // The next player

    Piece temp = GetPiece(GetBoard(data), move.end); // Make the move except don't put it on the stack
    
    MoveList list[1];

    SetPiece(GetBoard(data), move.end, move.piece); // partially execute the move to check if it puts the king in check
    SetPiece(GetBoard(data), move.start, EMPTY); // Sets the old position to blank
    SetPieceLoc(player, move.piece, move.end); // Moves the piece

    RemovePiece(nextplayer, temp); // Remove the piece from the enemy piece collection

    uint8_t numoftakes = GenerateValidMoves(data, list, nextplayer, GetPieceLoc(player, KING, 0)); // If the move puts you in check

    SetPiece(GetBoard(data), move.end, temp); // Undo the partiual move 
    SetPiece(GetBoard(data), move.start, move.piece); // Puts the piece back into its old spot
    SetPieceLoc(player, move.piece, move.start); // Moves the piece back

    SetPieceLoc(nextplayer, temp, move.end); // Sets the opponents piece back into its location

    return numoftakes != 0;

}

// EOF //