/*!
 * \file Game.c
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Game Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-25
 * \copyright Copyright (c) 2021
 */

// ------------------------------- Dependencies ----------------------------- //

#include "MoveList.h"
#include "MoveValidation.h"
#include "Gameplay.h"

// ----------------------------------- Static Functions ------------------------- //

/// Checks if the user can block a bishop check
static uint8_t CanBlockBishop(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock);

/// Checks if the user can block a rook check
static uint8_t CanBlockRook(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock);

/// Checks if the user can block a queen check
static uint8_t CanBlockQueen(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock);

/// Generates all moves for pawns
static uint8_t GeneratePawnMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index);

/// Generates all moves for bishops
static uint8_t GenerateBishopMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index);

/// Generates all moves for kings
static uint8_t GenerateKingMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index);

/// Generates all moves for Rooks
static uint8_t GenerateRookMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index);

/// Generates all moves for Knights
static uint8_t GenerateKnightMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index);

// ----------------------------- Function Definitions ------------------------------ //

/*!
 * \brief Clears the move list
 * \param list: The list to be cleared 
 */
void ClearMoveList(MoveList* const list)
{

    STATIC_ASSERT(list, "Invalid List Pointer");
    
    list->size = 0;

}

/*!
 * \brief Appends a move to the end of the move list and increases the size of the list
 * \param list: The list to append to
 * \param move: The move to append
 */
void AppendMove(MoveList* const list, const Move move)
{

    STATIC_ASSERT(list, "Invalid Move List Pointer");
    
    list->move[list->size] = move; // Appends the move
    list->size++; // Increments the list 

}

/*!
 * \brief Gets the size of the list
 * \param list: The list to get the size of 
 * \returns uint8_t: The size of the list
 */
uint8_t GetListSize(const MoveList* const list)
{

    STATIC_ASSERT(list, "Invalid Move List Pointer");

    return list->size;

}

/*!
 * \brief Gets the move from a list
 * \param list: List of move
 * \param index: Index of the move
 * \returns Move: Move from the list of moves
 */
Move GetListMove(const MoveList* const list, const uint8_t index)
{

    STATIC_ASSERT(list, "Invalid List Pointer");

    return list->move[index];

}

/*!
 * \brief Generates a list of all possible move for a piece   
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the moves
 * \param piece: The piece to make the move
 * \returns uint8_t: The number of moves
 */
uint8_t GenerateMoveList(const GameData* const data, MoveList* const list, Player* const player, const Piece piece)
{

    STATIC_ASSERT(data, "Invalid Game Data Pointer");
    STATIC_ASSERT(list, "Invalid Move List Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    ClearMoveList(list); // Clears the list

    Index index = GetPieceLoc(player, GetPieceID(piece), GetPieceNumber(piece)); // Gets the initial location of the piece
    
    switch(GetPieceID(piece))
    {

        case PAWN: return GeneratePawnMoves(data, list, player, index); // Generates a list of moves for pawns
        case BISHOP: return GenerateBishopMoves(data, list, player, index); // Generates a list of moves for bishops
        case KING: return GenerateKingMoves(data, list, player, index); // Generates a list of moves for kings
        case ROOK: return GenerateRookMoves(data, list, player, index); // Generates a list of moves for rooks
        case QUEEN: return GenerateBishopMoves(data, list, player, index) + GenerateRookMoves(data, list, player, index); // Generates a list of moves for queens
        case KNIGHT: return GenerateKnightMoves(data, list, player, index); // Generates a list of moves for knights
        default: return 0;

    }
}

/*!
 * \brief Generates all valid moves
 * \param data: The game data
 * \param list: The list to append the moves to
 * \param player: The player to make the moves
 * \param index: The index of the position to see if it can move to
 * \return uint8_t: The number of moves
 */
uint8_t GenerateValidMoves(const GameData* const data, MoveList* list, const Player* const player, const Index index)
{

    STATIC_ASSERT(list, "Invalid List Pointer");
    STATIC_ASSERT(data, "Invalid Game Data Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");

    Move move;
    Index location;
    Piece piece;

    ClearMoveList(list); // reset the move list

    const uint8_t pieces[6] = {KNIGHT, ROOK, QUEEN, KING, PAWN, BISHOP}; // All the pieces

    for(uint8_t j = 0; j < 6; j++) // Goes through all the piece types
    {

        for(uint8_t i = 0; i < GetNumPieces(player, pieces[j]); i++) // Goes through all the pieces of each type
        {

            location = GetPieceLoc(player, pieces[j], i); // The location of the piece
            piece = GetPiece(GetBoard(data), location); // The piece
            move = CreateMove(piece, location, index); // The move
        
            if(IsValidMove(data, player, move)) // Iv its a valid move it appends it to the list
                AppendMove(list, move);
        
        }
    }

    return GetListSize(list);

}

/*!
 * \brief Generates all valid moves for the knight
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the move
 * \param piece: The piece being moved
 * \returns uint8_t: The number of move
 */
uint8_t GenerateKnightMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index)
{

    Piece piece = IsPlayerWhite(player)? WKNIGHT: BKNIGHT; // Checks the color of the piece
    pthread_t threads[8]; // A thread for every possible move
    Move moves[8]; // Eight possible moves
    bool possible[8]; // Bool to track if possible

    moves[0] = CreateMove(piece, index, index + 10); // Creates the move
    CreateCheckMoveThread(&threads[0], data, player, moves[0]); // Checks the move

    moves[1] = CreateMove(piece, index, index + 6); // Creates the move
    CreateCheckMoveThread(&threads[1], data, player, moves[1]); // Checks the move

    moves[2] = CreateMove(piece, index, index + 15); // Creates the move
    CreateCheckMoveThread(&threads[2], data, player, moves[2]); // Checks the move

    moves[3] = CreateMove(piece, index, index + 17); // Creates the move
    CreateCheckMoveThread(&threads[3], data, player, moves[3]); // Checks the move

    moves[4] = CreateMove(piece, index, index - 10); // Creates the move
    CreateCheckMoveThread(&threads[4], data, player, moves[4]); // Checks the move

    moves[5] = CreateMove(piece, index, index - 6); // Creates the move
    CreateCheckMoveThread(&threads[5], data, player, moves[5]); // Checks the move

    moves[6] = CreateMove(piece, index, index - 15); // Creates the move
    CreateCheckMoveThread(&threads[6], data, player, moves[6]); // Checks the move

    moves[7] = CreateMove(piece, index, index - 17); // Creates the move
    CreateCheckMoveThread(&threads[7], data, player, moves[7]); // Checks the move

   for(uint8_t i = 0; i < 8; i++) // Goes through all moves
    {
        pthread_join(threads[i], (void*)&possible[i]); // Joins the threads back together and closes them
        if(possible[i]) AppendMove(list, moves[i]); // If its a valid move append it
    }

    return GetListSize(list);
    
}

/*!
 * \brief Generates all valid moves for the knight
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the move
 * \param piece: The piece being moved
 * \returns uint8_t: The number of move
 */
uint8_t GeneratePawnMoves(const GameData* const data, MoveList* const list, Player* const  player, const Index index)
{
    int8_t direction = IsPlayerWhite(player)? 1: -1; // Direction the pawns move
    Piece piece = GetPiece(GetBoard(data), index);

    pthread_t threads[4]; // a thread for every possible move
    Move moves[4]; // 4 possible moves
    bool possible[4]; // bool to track if possible

    moves[0] = CreateMove(piece, index, index + direction*8); // Creates the move
    CreateCheckMoveThread(&threads[0], data, player, moves[0]); // Checks the move

    moves[1] = CreateMove(piece, index, index + direction*16); // Creates the move
    CreateCheckMoveThread(&threads[1], data, player, moves[1]); // Checks the move

    moves[2] = CreateMove(piece, index, index + direction*8 + 1); // Creates the move
    CreateCheckMoveThread(&threads[2], data, player, moves[2]); // Checks the move

    moves[3] = CreateMove(piece, index, index + direction*8 - 1); // Creates the move
    CreateCheckMoveThread(&threads[3], data, player, moves[3]); // Checks the move

    for(uint8_t i = 0; i < 4; i++) // Goes through all moves
    {
        pthread_join(threads[i], (void*)&possible[i]); // Joins the threads back together and closes them
        if(possible[i]) AppendMove(list, moves[i]); // If its a valid move append it
    }

    return GetListSize(list);

}

/*!
 * \brief Generates all valid moves for the knight
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the move
 * \param piece: The piece being moved
 * \returns uint8_t: The number of move
 */
uint8_t GenerateKingMoves(const GameData* const data, MoveList* list, Player* const player, const Index index)
{
    Piece piece = IsPlayerWhite(player)? WKING: BKING; // Checks the color of the piece
    pthread_t threads[8]; // a thread for every possible move
    Move moves[8]; // 4 possible moves
    bool possible[8]; // bool to track if possible

    moves[0] = CreateMove(piece, index, index + 1);  // Creates the move
    CreateCheckMoveThread(&threads[0], data, player, moves[0]); // Checks the move

    moves[1] = CreateMove(piece, index, index + 9); // Creates the move
    CreateCheckMoveThread(&threads[1], data, player, moves[1]); // Checks the move

    moves[2] = CreateMove(piece, index, index + 8); // Creates the move
    CreateCheckMoveThread(&threads[2], data, player, moves[2]); // Checks the move

    moves[3] = CreateMove(piece, index, index + 7); // Creates the move
    CreateCheckMoveThread(&threads[3], data, player, moves[3]); // Checks the move

    moves[4] = CreateMove(piece, index, index - 1); // Creates the move
    CreateCheckMoveThread(&threads[4], data, player, moves[4]); // Checks the move

    moves[5] = CreateMove(piece, index, index - 7); // Creates the move
    CreateCheckMoveThread(&threads[5], data, player, moves[5]); // Checks the move

    moves[6] = CreateMove(piece, index, index - 8); // Creates the move
    CreateCheckMoveThread(&threads[6], data, player, moves[6]); // Checks the move

    moves[7] = CreateMove(piece, index, index - 9); // Creates the move
    CreateCheckMoveThread(&threads[7], data, player, moves[7]); // Checks the move

    for(uint8_t i = 0; i < 8; i++) // Goes through all moves
        pthread_join(threads[i], (void*)&possible[i]); // Joins the threads back together and closes them

    for(uint8_t i = 0; i < 8; i++) // Goes through all moves
        if(possible[i]) AppendMove(list, moves[i]); // If its a valid move append it

    return GetListSize(list);
}

/*!
 * \brief Generates all valid moves for the knight
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the move
 * \param piece: The piece being moved
 * \returns uint8_t: The number of move
 */
uint8_t GenerateRookMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index)
{
    Piece piece = GetPiece(GetBoard(data), index); // Checks the color of the piece
    pthread_t threads[14]; // a thread for every possible move
    Move moves[14]; // 4 possible moves
    bool possible[14]; // bool to track if possible

    uint8_t row = GetRow(index);
    uint8_t column = GetColumn(index);

    for(uint8_t i = 1; i < 8; i++) // check horizontally and vertially around the board
    {
        
        moves[i - 1] = CreateMove(piece, index, CreateIndex(row, column + i)); // loop around the left and right
        CreateCheckMoveThread(&threads[i - 1], data, player, moves[i - 1]); // Checks the move

        moves[i + 6] = CreateMove(piece, index, CreateIndex(row + i, column)); // loop around top and bottom
        CreateCheckMoveThread(&threads[i + 6], data, player, moves[i + 6]); // Checks the move

    }

    for(uint8_t i = 0; i < 14; i++) // Goes through all moves
        pthread_join(threads[i], (void*)&possible[i]); // Joins the threads back together and closes them

    for(uint8_t i = 0; i < 14; i++) // Goes through all moves
        if(possible[i]) AppendMove(list, moves[i]); // If its a valid move append it

    return GetListSize(list);

}

/*!
 * \brief Generates all valid moves for the knight
 * \param data: The game data
 * \param list: The list to append to
 * \param player: The player making the move
 * \param piece: The piece being moved
 * \returns uint8_t: The number of move
 */
uint8_t GenerateBishopMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index)
{
    
    Piece piece = GetPiece(GetBoard(data), index); // Checks the color of the piece

    pthread_t threads[14]; // a thread for every possible move
    
    Move moves[14]; // 4 possible moves
    bool possible[14]; // bool to track if possible

    uint8_t row = GetRow(index);
    uint8_t column = GetColumn(index);

    for(uint8_t i = 1; i < 8; i++) // check diagonally around the board
    {
        
        moves[i - 1] = CreateMove(piece, index, CreateIndex(row + i, column + i)); // loop around the +slope direction
        CreateCheckMoveThread(&threads[i - 1], data, player, moves[i - 1]); // Checks the move

        moves[i + 6] = CreateMove(piece, index, CreateIndex(row - i, column + i)); // loop around the -slope direction
        CreateCheckMoveThread(&threads[i + 6], data, player, moves[i + 6]); // Checks the move

    }

    for(uint8_t i = 0; i < 14; i++) // Goes through all moves
    {
        pthread_join(threads[i], (void*)&possible[i]); // Joins the threads back together and closes them
        if(possible[i]) AppendMove(list, moves[i]); // If its a valid move append it
    }

    return GetListSize(list);

}

/*!
 * \brief Generates a list of moves blocking pieces
 * \param data: The game data
 * \param list: The list to append the moves too
 * \param player: The player blocking the move
 * \param movetoblock: The move that needs to be blocked
 * \returns uint8_t: Returns the number of blocking moves
 */
uint8_t GenerateBlockingMoves(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock)
{
    
    STATIC_ASSERT(data, "Invalid Game Data Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");
    STATIC_ASSERT(list, "Invalid Move List");

    ClearMoveList(list); // Clears the movelist

    if(movetoblock.start > 63 || movetoblock.end > 63 || !movetoblock.piece) return 0; // If any index is out of range return

    switch(GetPieceID(movetoblock.piece))
    {

        case BISHOP: return CanBlockBishop(data, list, player, movetoblock); // Blocking a bishop
        case ROOK: return CanBlockRook(data, list, player, movetoblock); // Blocking a knight
        case QUEEN: return CanBlockQueen(data, list, player, movetoblock); // Blocking a queen
        default: return GetListSize(list);

    }
}

/*!
 * \brief Fills a move list with all of the possible moves to take an index
 * \param data: Current game data
 * \param list: MoveList to fill with the data
 * \param player: Player to look for move in
 * \param index: Index to see if they can take
 * \returns uint8_t: Number of valid moves found
 */
uint8_t GenerateTakingMoves(const GameData* const data, MoveList* const list, Player* const player, const Index index)
{

    STATIC_ASSERT(data, "Invalid Gamedata Pointer");
    STATIC_ASSERT(GetBoard(data), "Invalid Board Pointer");
    STATIC_ASSERT(list, "Invalid MoveList Pointer");
    STATIC_ASSERT(player, "Invalid Player Pointer");
    STATIC_ASSERT(index < 64, "Out Of Bounds Index");

    ClearMoveList(list); // Clears the move list

    pthread_t threads[10]; // maximum number of pieces 
    Move moves[10]; // maximum of 10 pieces per type thus ten moves
    bool possible[10]; // same as before

    Index location = 0xff; // Off the board
    Piece piece = EMPTY; // Blank piece

    const uint8_t pieces[7] = { ROOK, QUEEN, PAWN, KNIGHT, BISHOP, KING }; // All the pieces

    for(uint8_t i = 0; i < 7; i++) // Scans through all the pieces
    {   
        uint8_t numpieces = GetNumPieces(player, pieces[i]);
        for(uint8_t j = 0; j < numpieces; j++)  // start all of the threads
        {   
            piece = CreatePiece(IsPlayerWhite(player), pieces[i], j);
            location = GetPieceLoc(player, pieces[i], j);
            moves[j] = CreateMove(piece, location, index);
            CreateCheckMoveThread(&threads[j], data, player, moves[j]);
        }
        
        for(uint8_t k = 0; k < numpieces && k < 10; k++)
        {
            pthread_join(threads[k], (void*)&possible[k]);
            if(possible[k]) AppendMove(list, moves[k]);
        }
    }

    return GetListSize(list);
}
/*!
 * \brief Generates a list of moves blocking pieces
 * \param data: The game data
 * \param list: The list to append the moves too
 * \param player: The player blocking the move
 * \param movetoblock: The move that needs to be blocked
 * \returns uint8_t: Returns the number of blocking moves
 */
uint8_t CanBlockBishop(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock)
{

    Index pieces[7];
    uint8_t numpieces;
    uint8_t nummoves = 0;

    uint8_t torow = GetRow(movetoblock.end); // The row going to
    uint8_t fromrow = GetRow(movetoblock.start); // The row coming from

    uint8_t tocol = GetColumn(movetoblock.end); // The column going to
    uint8_t fromcol = GetColumn(movetoblock.start); // The column going from

    numpieces = abs(torow - fromrow); // The number of tiles

    if(!numpieces) return 0; // If theres no tiles

    int8_t ydir = (int8_t)(torow - fromrow) < 0? -1: 1; // The vertical direction
    int8_t xdir = (int8_t)(tocol - fromcol) < 0? -1: 1; // The horizontal direction

    for(uint8_t i = 0; i < numpieces; i++) // For all tiles
        pieces[i] = CreateIndex(fromcol + xdir*i, fromrow + ydir*i); // Create the tile index

    for(uint8_t i = 0; i < numpieces; i++) // For all tiles
        nummoves += GenerateTakingMoves(data, list, player, pieces[i]); // Generate all possible moves

    return nummoves;

}

/*!
 * \brief Generates a list of moves blocking pieces
 * \param data: The game data
 * \param list: The list to append the moves too
 * \param player: The player blocking the move
 * \param movetoblock: The move that needs to be blocked
 * \returns uint8_t: Returns the number of blocking moves
 */
uint8_t CanBlockRook(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock)
{

    Index pieces[7];
    uint8_t numpieces;
    uint8_t nummoves = 0;

    uint8_t torow = GetRow(movetoblock.end); // The row going to
    uint8_t fromrow = GetRow(movetoblock.start); // The row coming from

    uint8_t tocol = GetColumn(movetoblock.end); // The column going to
    uint8_t fromcol = GetColumn(movetoblock.start); // The column going from

    numpieces = abs(torow - fromrow)? abs(torow - fromrow): abs(tocol - fromcol); // The number of tiles

    if(!numpieces) return 0; // If theres no tiles

    int8_t ydir = (int8_t)(torow - fromrow) < 0? -1: (torow == fromrow)? 0: 1; // The vertical direction
    int8_t xdir = (int8_t)(tocol - fromcol) < 0? -1: (tocol == fromcol)? 0: 1; // The horizontal direction

    for(uint8_t i = 0; i < numpieces; i++) // For all tiles
        pieces[i] = CreateIndex(fromcol + xdir*i, fromrow + ydir*i); // Create the tile index

    for(uint8_t i = 0; i < numpieces; i++) // For all tiles
        nummoves += GenerateTakingMoves(data, list, player, pieces[i]); // Generate all possible moves

    return nummoves;

}

/*!
 * \brief Generates a list of moves blocking pieces
 * \param data: The game data
 * \param list: The list to append the moves too
 * \param player: The player blocking the move
 * \param movetoblock: The move that needs to be blocked
 * \returns uint8_t: Returns the number of blocking moves
 */
uint8_t CanBlockQueen(const GameData* const data, MoveList* const list, Player* const player, const Move movetoblock)
{

    int8_t kingRow = GetRow(movetoblock.end);
    int8_t kingCol = GetColumn(movetoblock.end);
    int8_t queenRow = GetRow(movetoblock.start);
    int8_t queenCol = GetColumn(movetoblock.start);

    return (kingRow != queenRow && kingCol != queenCol)? CanBlockBishop(data, list, player, movetoblock): CanBlockRook(data, list, player, movetoblock); // Depending on locations runs either bishop check or rook check

}

// EOF //
