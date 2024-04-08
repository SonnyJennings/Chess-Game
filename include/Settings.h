/*!
 * \file Settings.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Settings Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef SETTINGS_H
#define SETTINGS_H

// ------------------------- Dependencies ------------------------- //

#include "main.h"

// ------------------------- Types ------------------------- //

/*!
 * \brief Gamemodes
 * \details The type of gamemodes possible such as human vs human AI vs human and AI vs AI
 */
typedef enum
{

    HumanVHuman,
    HumanVAI,
    AIVAI,
    OnlinePvP

} GameMode;

typedef struct 
{
 int logined;
 int playerNum;
 int PortNo;
 int DataFD;
 char hostname[30];
}Connection;

/*!
 * \brief Settings struct
 * \details Stores details of the settings such as board and player color along with the gamemode and connection info
 */
typedef struct
{

    Color whitecolor;     ///< The color of white pieces
    Color blackcolor;     ///< The color of black pieces
    Color whitebg;        ///< The color of white tiles
    Color blackbg;        ///< The color of black tiles
    
    GameMode mode;        ///< The gamemade currently being used
    Connection connect;

} Settings;



// ------------------------- Functions ------------------------- //

/// Gets the default settings
void ResetSettings(Settings* const settings);

/// Sets the piece color
void SetPieceColor(Settings* const settings, bool iswhite, const Color color);

/// Sets the square color
void SetSquareColor(Settings* const settings, bool iswhite, const Color color);

/// Get the gamemode
GameMode GetGameMode(const Settings* const settings);

/// Sets the gamemode
void SetGameMode(Settings* const settings, const GameMode mode);

//Set connection info
void SetConnection(Settings* const settings, Connection connection);

/// Gets the color of the pieces
Color GetPieceColor(const Settings* const settings, const bool isWhite);

/// Gets the color of the squares
Color GetSquareColor(const Settings* const settings, const bool isWhite);

#endif

// EOF //
