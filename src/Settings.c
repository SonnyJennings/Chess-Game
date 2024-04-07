/*!
 * \file Settings.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Settings Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-27
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Settings.h"

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Returns the default settings
 * \returns DefaultSettings: The default settings
 */
void ResetSettings(Settings* const settings)
{
    Connection connection  = {.logined = 0, .playerNum = 0, .PortNo = 11400, .hostname = "zuma"};
    settings->blackbg = Black;
    settings->blackcolor = LightRed;
    settings->whitebg = LightBlue;
    settings->whitecolor = White;
    settings->mode = HumanVHuman;
    settings->connect = connection;

}

/*!
 * \brief Sets the player color in the settings
 * \param settings: Settings to modify
 * \param isWhite: If the player is the white player
 * \param color: Color to set the Player to
 */
void SetPieceColor(Settings* const settings, const bool isWhite, const Color color)
{

    if(isWhite) settings->whitecolor = color; // If the selection is white set the piece color to the selected piece color
    else settings->blackcolor = color; // If the selection is black set the piece color to the selected piece color

}

/*!
 * \brief Sets the square color in the settings
 * \param settings: Settings to modify
 * \param isWhite: If the player is the white player
 * \param color: Color to set the Player to
 */
void SetSquareColor(Settings* const settings, const bool isWhite, const Color color)
{

    if(isWhite) settings->whitebg = color; // If the selection is white set the background color to the selected background color
    else settings->blackbg = color; // If the selection is black set the background color to the selected backround color

}

/*!
 * \brief Gets the color of the pieces
 * \param settings: Settings to look at
 * \param isWhite: If the piece is the white piece
 * \returns Color: The color of the piece
 */
Color GetPieceColor(const Settings* const settings, const bool isWhite)
{
    
    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    if(isWhite) return settings->whitecolor;
    return settings->blackcolor;

}

/*!
 * \brief Gets the square color from settings
 * \param settings: Settings to get color from
 * \param isWhite: If we are looking to get the white square color
 * \returns Color: The color of the requested square
 */
Color GetSquareColor(const Settings* const settings, const bool isWhite)
{

    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    if(isWhite) return settings->whitebg;
    return settings->blackbg;

}

/*!
 * \brief Gets the gamemode from the settings
 * \param settings: Settings to get from
 * \returns GameMode: Gamemode for the game
 */
GameMode GetGameMode(const Settings* const settings)
{

    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    return settings->mode;

}

/*!
 * \brief Sets the gamemode
 * \param settings: Settings to set gamemode in
 * \param mode: Mode to set the gamemode to
 */
void SetGameMode(Settings* const settings, const GameMode mode)
{
    
    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    settings->mode = mode;

}

/*!
 * \brief Sets the connection info
 * \param settings: Settings to set connection info in
 * \param mode: Connection info to set
 */
void SetConnection(Settings* const settings, Connection connection)
{
    
    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    settings->connect = connection;

}

// EOF //