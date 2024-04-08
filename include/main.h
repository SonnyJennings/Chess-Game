/*!
 * \file main.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains all of the Random Utilities and Macros Used in Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef MAIN_H
#define MAIN_H

// ------------------------- Shared Dependencies ------------------------- //

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

// ------------------------- Types ------------------------- //

/*!
 * \brief Color type used to color background and foreground
 * \details The codes coorespond to the unix color codes for terminal colors,
 * The background colors are the same colors except the value is ten higher
 */
typedef enum
{

    Default = 0,
    Black = 30,
    Red = 31, // (don't use with green background)
    Green = 32,
    Yellow = 33, 
    Blue = 34, 
    Magenta = 35,
    Cyan = 36,
    LightGray = 37, 
    Reset = 39,
    DarkGray = 90,
    LightRed = 91,
    LightGreen = 92,
    LightYellow = 93,
    LightBlue = 94,
    Pink = 95,
    LightCyan = 96,
    White = 97,
    
} Color;

// ------------------------- Macros ------------------------- //

/// Macro for white 
#define WHITE true

/// Macro for black
#define BLACK false

/// Sets the terminal foreground and background to specific colors
#define SetColor(bg, fg) printf("\x1B[%dm\x1B[%dm", bg + 10, fg)

/// Clear the screen
#define ClearScreen() printf("\033[2J\033[1;1H")

/// Special assert method that adds color to assert messages
#define STATIC_ASSERT(cond, str) assert((!(cond)? (fprintf(stderr, "\n %s \n File: %s \n Line: %u\n\n", str, __FILE__, __LINE__) && false): true))

#endif

// EOF //
