/*! 
 * \file main.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the Ultimate Chess Entry Point
 * \version 1.0
 * \date 2021-04-17
 * \copyright Copyright (c) 2021
 */

/*!
 * \mainpage Ultimate Chess For EECS 22L at UCI Spring 2021
 * Team 14: \n
 * Members: \n
 * Orion Serup: Reflector \n
 * Ian Poremba: Manager \n
 * Raymond Duong: Recorder \n
 * Chris Rodriguez: Presenter \n
 * Sunshine Jennings: Reflector \n\n
 * 
 * Professor: \n 
 * Dr. Q.V. Dang \n
 */

// ------------------------- Dependencies ------------------------- //

#include "Game.h"
#include "Menu.h"
#include "Gameplay.h"
#include "AI.h"
#include "Moves.h"

// ------------------------- Definition ------------------------- //

/*!
 * \brief The program entry point
 * \returns Status of exit
 */
int main(int argc, char** kwargs)
{
    #ifdef DEBUG
    if(argc > 1 && !strcmp("--test", kwargs[1]))
    {

        Board board[1];
        Player player[1];
        GameData data[1];

        ResetBoard(board);
        ResetGameData(data);
        PrintBoard(board, GetSettings(data));

        Move move = CreateMove(GetPiece(board, 4), 4, 6);

        ResetPlayer(player, WHITE);

        puts("\nTesting if castling works for white with no pieces in the way\n");
        
        RemovePiece(player, GetPieceAt(board, 6, 0));
        RemovePiece(player, GetPieceAt(board, 5, 0));

        SetPiece(board, 6, EMPTY);
        SetPiece(board, 5, EMPTY);

        PrintBoard(board, GetSettings(data));

        if(CanCastle(player, (uint8_t)1)) 
            puts("Yes you can castle\n");

        else puts("No you can not castle\n");
        MakeMove(data, player, move);
        PrintBoard(board, GetSettings(data));

        puts("Checking to see if en passants work when it should");
        move = CreateMove(GetPieceAt(board, 3, 6), CreateIndex(3, 6), CreateIndex(3, 4));

        PushMove(GetMoveStack(data), move);
        SetPieceLoc(player, GetPieceAt(board, 4, 1), CreateIndex(4, 4));
        SetPiece(board, CreateIndex(4, 1), EMPTY);
        SetPiece(board, CreateIndex(4, 4), CreatePiece(WHITE, PAWN, 0));
        PrintBoard(board, GetSettings(data));

        Move temp = PeekMove(GetMoveStack(data)); // Looks at the previous move

        if(GetPieceID(temp.piece) == PAWN && abs(GetRow(temp.start) - GetRow(temp.end)) == 2 && GetColumn(temp.end) == GetColumn(move.end) && GetPieceID(move.piece) == PAWN && ((GetRow(move.end) == 5 && GetRow(temp.end) == 4) || (GetRow(move.end) == 2 && GetRow(temp.end) == 3))) 
            puts("Yes you can En Passant\n");
        else puts("No you can not En Passant\n");

    }
    else
    {
    
    #endif

        GameData data;
        ResetGameData(&data);

        while(1) // Loops indefinitely
        {
        
            Menu(data.settings); // Triggers the menu to start
            Game(&data); // Triggers the game to start based on the gamedata

        }

    #ifdef DEBUG
    }
    #endif
}

// EOF //