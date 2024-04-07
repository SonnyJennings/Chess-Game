/*!
 * \file Board.h
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Board Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef BOARD_H
#define BOARD_H

// ------------------------- Dependencies ------------------------- //

#include "Settings.h"

// ------------------------- Piece IDs ------------------------- //

/// PAWN ID/Character
#define PAWN 'P'

/// ROOK ID/Character
#define ROOK 'R'

/// Knight ID/Character
#define KNIGHT 'N'

/// Bishop ID/Character
#define BISHOP 'B'

/// Queen ID/Character
#define QUEEN 'Q'

/// King ID/Character
#define KING 'K'

/// EMpty Piece 
#define EMPTY 0

/// Black Pawn
/// Multiply by 2 to shift bit 1 left
#define BPAWN PAWN * 2

/// White Pawn
/// Add one, lowest bit determine b or w
#define WPAWN PAWN * 2 + 1

/// Black Rook
/// Multiply by 2 to shift bit 1 left
#define BROOK ROOK * 2

/// White Rook
/// Add one, lowest bit determine b or w
#define WROOK ROOK * 2 + 1

/// Black Knight
/// Multiply by 2 to shift bit 1 left
#define BKNIGHT KNIGHT * 2

/// White Knight
/// Add one, lowest bit determine b or w
#define WKNIGHT KNIGHT * 2 + 1

/// Black Bishop
/// Multiply by 2 to shift bit 1 left
#define BBISHOP BISHOP * 2

/// White Bishop
/// Add one, lowest bit determine b or w
#define WBISHOP BISHOP * 2 + 1

/// Black Queen
/// Multiply by 2 to shift bit 1 left
#define BQUEEN QUEEN * 2

/// White Queen
/// Add one, lowest bit determine b or w
#define WQUEEN QUEEN * 2 + 1

/// Black King
/// Multiply by 2 to shift bit 1 left
#define BKING KING * 2

/// White King
/// Add one, lowest bit determine b or w
#define WKING KING * 2 + 1

// ------------------------- Piece Numbers ------------------------- //

/// Piece 0 for piece tracking
#define P0 (uint16_t)0x0000

/// Piece 1 for piece tracking
#define P1 (uint16_t)0x1000

/// Piece 2 for piece tracking
#define P2 (uint16_t)0x2000

/// Piece 3 for piece tracking
#define P3 (uint16_t)0x3000

/// Piece 4 for piece tracking
#define P4 (uint16_t)0x4000

/// Piece 5 for piece tracking
#define P5 (uint16_t)0x5000

/// Piece 6 for piece tracking
#define P6 (uint16_t)0x6000

/// Piece 7 for piece tracking
#define P7 (uint16_t)0x7000

/// Piece 8 for piece tracking
#define P8 (uint16_t)0x8000

/// Piece 9 for piece tracking
#define P9 (uint16_t)0x9000

/// Piece 10 for piece tracking
#define P10 (uint16_t)0xA000

/// Piece 11 for piece tracking
#define P11 (uint16_t)0xB000

/// Piece 12 for piece tracking
#define P12 (uint16_t)0xC000

/// Piece 13 for piece tracking
#define P13 (uint16_t)0xD000

/// Piece 14 for piece tracking
#define P14 (uint16_t)0xE000

/// Piece 15 for piece tracking
#define P15 (uint16_t)0xF000

/// Maximum index value
#define INDEX_MAX UINT8_MAX

#define QUIT (Index)0xC0

// ------------------------- Types ------------------------- //

/// Type for pieces
typedef uint16_t Piece;

/// Index type for accessing the board elements
typedef uint8_t Index;

/*!
 * \brief Board Type
 * \details The board is made of a grid of pieces
 * The grid is made of a 1-D array of size 64 and the 2-D mapping is accomodated
 * The conversion from chess terms to index is done in a helper function called StringToIndex 
 */
typedef struct
{

    Piece grid[64];     ///< grid of pieces

} Board;

// ------------------------- FUNCTIONS ------------------------- //

/// Generates a default board
void ResetBoard(Board* const board);

/// Prints the given board
void PrintBoard(const Board* const board, const Settings* const settings);

/// Gets the piece at the given index
Piece GetPiece(const Board* const board, const Index index);

/// Get the piece at the given x y coordinate
Piece GetPieceAt(const Board* const board, const uint8_t x, const uint8_t y);

/// Sets the board at the index with the piece given
void SetPiece(Board* const board, const Index index, const Piece piece);

/// Gets an index from user
Index GetIndex();

/// Creates an index for a x y pair
Index CreateIndex(const uint8_t x, const uint8_t y);

/// Utility function to get the board index from a location string eg 'C3'
Index CharToIndex(const char row, const char col);

/// Utility function to get a friendly location string from an index
void IndexToString(const Index index, char str[2]);

/// Sets the color 
bool IsPieceWhite(const Piece piece);

/// Gets the piece ID
uint8_t GetPieceID(const Piece piece);

/// Gets the piece Number
uint8_t GetPieceNumber(const Piece piece);

/// Returns row number
uint8_t GetRow(const Index index);

/// Returns column number
uint8_t GetColumn(const Index index);

/// Prompt the user to select a piece
void PromptWhichPiece(bool isWhite);

/// Prompt where the player should go
void PromptWhereTo();

/// Creates a piece with ID and number
Piece CreatePiece(bool isWhite, uint8_t PieceID, uint8_t PieceNum);

#endif

// EOF //