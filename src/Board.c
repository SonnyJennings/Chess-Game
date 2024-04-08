/*!
 * \file Board.c
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Board Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Board.h"

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Default board (board at start of game)
 */
static const Board DefaultBoard = 
{

    .grid = 
    {

        (WROOK)|(P0),   (WKNIGHT)|(P0), (WBISHOP)|(P0), WQUEEN,         WKING,          (WBISHOP)|(P1), (WKNIGHT)|(P1), (WROOK)|(P1),
        (WPAWN)|(P0),   (WPAWN)|(P1),   (WPAWN)|(P2),   (WPAWN)|(P3),   (WPAWN)|(P4),   (WPAWN)|(P5),   (WPAWN)|(P6),   (WPAWN)|(P7),
        EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,
        EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,
        EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,
        EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,          EMPTY,
        (BPAWN)|(P0),   (BPAWN)|(P1),   (BPAWN)|(P2),   (BPAWN)|(P3),   (BPAWN)|(P4),   (BPAWN)|(P5),   (BPAWN)|(P6),   (BPAWN)|(P7),
        (BROOK)|(P0),   (BKNIGHT)|(P0), (BBISHOP)|(P0), BQUEEN,         BKING,          (BBISHOP)|(P1), (BKNIGHT)|(P1), (BROOK)|(P1)

    }
};

/*!
 * \brief Gets the default board
 * \param board: Takes in the game board
 */
void ResetBoard(Board* const board)
{
    
    *board = DefaultBoard; // Sets the game board to the default board

}

/*!
 * \brief Get the piece at the index
 * \param board: Board to get piece from
 * \param index: Index to get piece from
 * \returns Piece: at that index in that Board
 */
Piece GetPiece(const Board* const board, const Index index)
{

    STATIC_ASSERT(board, "Invalid Board Pointer");

    if(index > 63) return EMPTY;

    return board->grid[index]; // The (x, y) is a one dimensional array

}

/*!
 * \brief Gets the piece At the given x y coordinate
 * \param board: Board to get the value from
 * \param x: Value column
 * \param y: Value row
 * \returns Piece: Piece at that position
 */
Piece GetPieceAt(const Board* const board, const uint8_t x, const uint8_t y)
{

    STATIC_ASSERT(board, "Invalid Board Pointer");
    STATIC_ASSERT((x < 8), "X Value Too Big");
    STATIC_ASSERT((y < 8), "Y Value Too Big");

    return GetPiece(board, (y << 3) + (x & 0x7)); // Multiply y(rows) by 8 and add x(columns) to get index

}

/*!
 * \brief Set the piece at the Index
 * \param board: Board to set the piece on
 * \param index: Where to set the piece
 * \param piece: The piece to set
 */
void SetPiece(Board* const board, const Index index, const Piece piece)
{

    STATIC_ASSERT(board, "Invalid Board Pointer");
    
    if(index > 64) return;

    board->grid[index] = piece; // Places the piece on the grid of the board at the inputed index

}

/*!
 * \brief Gets an index from the player and converts it to an appropriate string
 * \returns Index: that was inputted by the player 
 */
Index GetIndex()
{

    char row;
    char col;

    if(scanf(" %c%c", &col, &row)) // To take in row and column
    {

        if(toupper(row) == 'Q' && toupper(col) == 'Q') // If the player types QQ they quit
            return QUIT;
        else
            return CharToIndex(row, col); // Fills the string with an inputed index 
            
    }

    return INDEX_MAX; 

}

/*!
 * \brief Converts from a numerical index to a more friendly coordinate string eg 'B2'
 * \param index: Index to convert
 * \param str: String to write to
 */
void IndexToString(const Index index, char str[2])
{

    STATIC_ASSERT(str, "Invalid String");

    str[0] = GetColumn(index) + 'A'; // Index % 8 is the column
    str[1] = GetRow(index) + '1'; // Index / 8 is the row    

}

/*!
 * \brief Converts from a friendly coordinate string to an index
 * \param row: row to convert from
 * \param col: col to convert from
 * \returns Index: Index that is converted
 */
Index CharToIndex(const char row, const char col)
{ 

    Index index = (col - 'A') & 0x7; // Set the column
    index += (((row - '1') & 0x7) << 3); // Set the row

    return index;

}

/*!
 * \brief Prints the board to the terminal
 * \param board: Board to print
 * \param settings: Settings for color
 */
void PrintBoard(const Board* const board, const Settings* const settings)
{

    STATIC_ASSERT(board, "Invalid Board Pointer");
    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    Color bg = settings->whitebg; // Set the starting background color to be the white background color

    puts("   A  B  C  D  E  F  G  H "); // Label the columns
    puts(" --------------------------"); // Print top wall

    for (int i = 7; i >= 0; i--) // Each row from 8 to 1
    {
        
        printf("%d|", i + 1); // Draw the left wall

        for(int j = 0; j < 8; j++) // Each column from A to H
        {

            Piece piece = GetPieceAt(board, j, i); // Get the piece at the index
            
            SetColor(bg, GetPieceColor(settings, IsPieceWhite(piece))); // Set the text color according to the color of the piece
            
            if(piece) printf(" %c ", GetPieceID(piece)); // If the piece isn't empty print the value 
            else printf("   "); // Else print a space

            bg = GetSquareColor(settings, bg == GetSquareColor(settings, BLACK)); // Switch square color every move

        }

        SetColor(Reset, Reset); // Reset the colors to the terminal default

        bg = (bg == settings->whitebg)? settings->blackbg: settings->whitebg; // Switch square color every move

        printf("|%d", i + 1); // Draw the right hand edge
        putchar('\n');

    }

    puts(" --------------------------"); // Print the bottom edge
    puts("   A  B  C  D  E  F  G  H  "); // Label the columns

}

/*!
 * \brief Creates a piece for a player and returns it
 * \param isWhite: Takes in a bool to say whether a player is white or black
 * \param PieceID: Takes in the ID of the piece it intends to set
 * \param PieceNum: Takes in the number of the piece it intends to set
 * \returns Piece: Returns the piece
 */
Piece CreatePiece(const bool isWhite, const uint8_t PieceID, const uint8_t PieceNum)
{

    STATIC_ASSERT(PieceNum < 10, "Too Big of a Piece Number");

    Piece piece = PieceID << 1; // Sets the piece to be the correct piece by shifting the ID by 2 to get it to be the piece type
    piece |= (PieceNum << 12); // Removes a large portion of the binary sop its correctly set
    
    if(isWhite) piece |= 1; // Sets the piece to the correct color by checking which color it is and adjusting the binary

    return piece;

}

/*!
 * \brief Creates an index from to inputs
 * \param x: The inputed column 
 * \param y: The inputed row
 * \returns Index: Returns the created index 
 */
Index CreateIndex(const uint8_t x, const uint8_t y)
{
    
    return (Index)((x & 7) + ((y & 7) << 3)); // Creates an index out of an inputed row and column

}

/*!
 * \brief Gets the color of a piece
 * \param piece: Piece to get color of
 * \returns bool: Returns a bool representing the color of the piece
 */
bool IsPieceWhite(const Piece piece)
{

    return (piece & 1); // Returns 1 or 0 based on if the piece is white or not 

}

/*!
 * \brief Gets the piece and returns the corresponding piece ID
 * \param piece: The piece to retrieve the data from 
 * \returns uint_t: The pieces ID
 */
uint8_t GetPieceID(const Piece piece)
{

    return (uint8_t)(piece >> 1); // Divide two or omit last bit to get piece ID

}

/*!
 * \brief Gets the Number of a Piece  
 * \param piece: Piece to get number of
 * \returns uint8_t: Number of the Piece
 */
uint8_t GetPieceNumber(const Piece piece)
{

    return (piece >> 12) & 0xf; // Returns the number of the piece by right shifting until its in the range 0-7 and comparing with 7

}

/*!
 * \brief Gets the row of the selected location
 * \param index: The index of a location on the board
 * \returns uint_8: The number of the row 
 */
uint8_t GetRow(const Index index)
{ 

    return (index >> 3) & 7; // Right shifts the index until its in range 0-7 to show the row

}

/*!
 * \brief Gets the column of the selected location
 * \param index: The index of a location on the board
 * \returns uint_8: The number of the column
 */
uint8_t GetColumn(const Index index)
{ 

    return (index & 0x7); // Ands with 7 to determine the column from the index

}

/*!
 * \brief Prompts which piece you would like to move
 * \param isWhite: The bool that stores if something is white
 */
void PromptWhichPiece(bool isWhite)
{

    printf("Choose your piece, %s: ", (isWhite? "white": "black"));

}

/*!
 * \brief Prompts where you would like to move
 */
void PromptWhereTo()
{

    printf("Where would you like to move: ");

}

// EOF //
