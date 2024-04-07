/*!
 * \file Menu.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the Menu Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-25
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "Menu.h"
#include "tcpClient.h"

// ------------------------- Functions ------------------------- //

/// Get an int from a file
static uint32_t fgetint(FILE* const file);

/// Get an int from the user
#define GetInt() (fgetint(stdin)) 

/// The function for the gamemenu
static void GameMenu(Settings* const settings);

/// The function for the settings menu
static void SettingsMenu(Settings* const settings);

/// Prompts the user to set a piece color
static void PromptPieceColor(bool isWhite);

/// Prompts the user to set the background color
static void PromptBGColor(bool isWhite);

/// Gets a color from a user
static Color GetColor(bool isWhite, bool piece);

/// Menu For setting board color
static void BoardColorMenu(Settings* const settings);

/// Menu for setting piece color
static void PieceColorMenu(Settings* const settings);

//Print the sign in menu
void PrintSigninMenu();

//Menu for sign in
int SigninMenu(Settings* const settings);

//Call client login protocol and return 1 if login successful
Connection LoggingIn();

//Call client register protocol
void RegisterAccount();

// ------------------------- Definintions ------------------------- //

/*!
 * \brief Prints main menu 
 */
void PrintMenu()
{

    puts("Welcome to Ultimate Chess");
    puts("Final Version 1.0\n");
    puts("\t1 Start a game");
    puts("\t2 Settings");
    puts("\t3 Exit\n");
    printf("Please select an option: ");

}

/*!
 * \brief The main menu determines which messeges to be printed
 * \param settings: The game settings
 */
void Menu(Settings* const settings)
{

    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    while(1) // Loops indefinitely
    {

        ClearScreen(); // Clears the screen
        
        PrintMenu(); // Makes the main menu print

        uint8_t option = getchar(); // Gets the input to decide what to do next
        
        switch(option)
        {

            case '1': 
                GameMenu(settings); // Opens the game menu to decide which gamemode the player wants to play
                ClearScreen();
                return;
            case '2': 
                SettingsMenu(settings); // Opens the settings menu
                break;
            case '3':
                exit(0); // Exits the game
            default: break;

        }
    }
}

/*!
 * \brief Prints game menu
 */
void PrintGameMenu()
{

    puts("Ultimate Chess: Gamemodes\n");
    puts("\t1 Human vs Human");
    puts("\t2 Human vs AI");
    puts("\t3 AI vs AI");
    puts("\t4 Online PvP\n");
    printf("Please select an option: ");

}

/*!
 * \brief The game menu
 * \param settings: The settings for the game
 */
void GameMenu(Settings* const settings)
{   

    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    while (1)
    {
    
        ClearScreen(); // Clears screen
    
        PrintGameMenu(); // Prints the options for the gamemode

        uint8_t option = getchar(); // Gets the input to decide what to do next

        switch(option)
        {

            case '1':
                SetGameMode(settings, HumanVHuman); // Sets the gamemode to human vs human
                return;
            case '2': 
                SetGameMode(settings, HumanVAI); // Sets the gamemode to human vs AI
                return;
            case '3': 
                SetGameMode(settings, AIVAI); // Sets the gamemode to AI vs AI
                return;
            case '4': 
                if(SigninMenu(settings) == 0)
                {
                    break;
                }
                SetGameMode(settings, OnlinePvP); // Sets the gamemode to AI vs AI
                return;
            default: break;

        }
    }
}

/*!
 * \brief Prints Signin menu
 */
void PrintSigninMenu()
{
    puts("Ultimate Chess: Signin\n");
    puts("\t1. Login");
    puts("\t2. Register");
    puts("\t3. Exit\n");
    printf("Please select an option: ");
}

/*!
 * \brief The Signin
 * \param settings: Sign in for Online PvP
 */
int SigninMenu(Settings* const settings)
{
    Connection connection;

    while(1) // Loops indefinitely
    {

        ClearScreen(); // Clears screen
        
        PrintSigninMenu(); // Prints the settings menu for the game

        uint8_t option = getchar(); // Gets the input to decide what to do next

        switch(option)
        {

            case '1':
                connection = LoggingIn();
                if(connection.logined == 0)
                {
                 break;
                }
                else
                {
                 SetConnection(settings, connection);
                 return 1;
                }
            case '2': 
                RegisterAccount(); // Goes to the piece color menu
                break;
            case '3': return 0;
            default: break;

        }
    }
    return 0;
}

Connection LoggingIn()
{
 int PortNo;
 char hostname[30] = "";
 GameInfo game;
 Connection connection;

 printf("Please Enter Port Number:");

 (void) scanf("%d", &PortNo); //get the PortNo from user

 printf("Please Enter HostName:");

 (void) scanf("%s", hostname);//get hostname form user

 game = ClientLogin(hostname, PortNo);

 strcpy(connection.hostname, hostname);
 connection.logined = game.logined;
 connection.playerNum = game.playerNum;
 connection.PortNo = PortNo;
 connection.DataFD = game.DataFD;

 return connection; //Call Client Login
}

void RegisterAccount()
{
 int PortNo;
 char hostname[30] = "";

 printf("Please Enter Port Number:");

 (void) scanf("%d", &PortNo);//Get PortNo from user

 printf("Please Enter HostName:");

 (void) scanf("%s", hostname); //Get hostname from user

 ClientRegister(hostname,PortNo);  //Call Client Register
}


/*!
 * \brief Prints settings menu
 */
void PrintSettingsMenu()
{
    puts("Ultimate Chess: Settings\n");
    puts("\t1. Board color");
    puts("\t2. Piece color");
    puts("\t3. Back to main menu\n");
    printf("Please select an option: ");
}

/*!
 * \brief The settings menu
 * \param settings: The settings for the game
 */
void SettingsMenu(Settings* const settings)
{

    STATIC_ASSERT(settings, "Invalid Settings Pointer");

    while(1) // Loops indefinitely
    {

        ClearScreen(); // Clears screen
        
        PrintSettingsMenu(); // Prints the settings menu for the game

        uint8_t option = getchar(); // Gets the input to decide what to do next

        switch(option)
        {

            case '1':
                BoardColorMenu(settings); // Goes to the board color menu
                break;
            case '2': 
                PieceColorMenu(settings); // Goes to the piece color menu
                break;
            case '3': return;
            default: break;

        }
    }
}

/*!
 * \brief Prints the settings of the board color and sets the new color
 * \param settings: The game settings struct
 */
void BoardColorMenu(Settings* const settings)
{
    
    STATIC_ASSERT(settings, "Invalid Settings Pointer");
    
    while(1) // Loops indefinitely
    {

        ClearScreen(); // Clears screen
        
        puts("Ultimate Chess: Background Color Settings\n");
        puts("\t1. Set white background");
        puts("\t2. Set black background");
        puts("\t3. Exit board color menu\n");
        printf("Please select an option: ");
        
        uint8_t selection = getchar(); // Players selection

        switch(selection)
        {

            case '1': 
                settings->whitebg = GetColor(WHITE, false); // Sets the white background
                break;
            case '2': 
                settings->blackbg = GetColor(BLACK, false); // Sets the black background
                break;
            case '3': return;
            default: break;

        }
    }
}

/*!
 * \brief Prints the settings of the player color and sets the new color
 * \param settings: The game settings struct
 */
void PieceColorMenu(Settings* const settings)
{
    
    STATIC_ASSERT(settings, "Invalid Settings Pointer");
    
    while(1) // Loops indefinitely
    {

        ClearScreen(); // Clears screen
        
        puts("Ultimate Chess: Player Color Settings\n");
        puts("\t1. Set white color");
        puts("\t2. Set black color");
        puts("\t3. Exit color selection menu\n");
        printf("Please select an option: ");
        
        uint8_t selection = getchar(); // Players selection
        
        switch(selection)
        {

            case '1': 
                settings->whitecolor = GetColor(WHITE, true); // Sets the white piece
                break;
            case '2': 
                settings->blackcolor = GetColor(BLACK, true); // Sets the black piece
                break;
            case '3': return;
            default: break;

        }
    }
}

/*!
 * \brief Takes in an int from the player and returns a color based off of the int
 * \returns Color: The selected color
 */
Color GetColor(bool isWhite, bool piece)
{

    while(1)
    {
        
        (piece)? PromptPieceColor(isWhite): PromptBGColor(isWhite); // Prompts the piece to be changed

        int selection = GetInt(); // The players selection

        switch(selection)
        {

            case 1: return Black;
            case 2: return Red;
            case 3: return Green;
            case 4: return Yellow;
            case 5: return Blue;
            case 6: return Magenta;
            case 7: return Cyan;
            case 8: return LightGray;
            case 9: return Reset;
            case 10: return DarkGray;
            case 11: return LightRed;
            case 12: return LightGreen;
            case 13: return LightYellow;
            case 14: return LightBlue;
            case 15: return Pink;
            case 16: return LightCyan;
            case 17: return White;
            default: break;
        
        }
    }
}

/*!
 * \brief Prints the menu and options for the pieces
 * \param isWhite: Bool to say whether a player is white or not
 */
void PromptPieceColor(bool isWhite)
{

    ClearScreen(); // Clears the screen

    puts("Ultimate Chess: Player Color Settings\n");
    printf("What color would you like the %s pieces to be?\n\n", (isWhite?"white": "black"));
    puts("\t1. Black");
    puts("\t2. Red");
    puts("\t3. Green");
    puts("\t4. Yellow");
    puts("\t5. Blue");
    puts("\t6. Magenta");
    puts("\t7. Cyan");
    puts("\t8. Light Gray");
    puts("\t9. Terminal Default");
    puts("\t10. Dark Gray");
    puts("\t11. Light Red");
    puts("\t12. Light Green");
    puts("\t13. Light Yellow");
    puts("\t14. Light Blue");
    puts("\t15. Pink");
    puts("\t16. Light Cyan");
    puts("\t17. White\n");
    printf("Please enter an option: ");

}

/*!
 * \brief Prints the menu and options for the board
 * \param isWhite: Bool to say whether a player is white or not
 */
void PromptBGColor(bool isWhite)
{

    ClearScreen(); // Clears the screen
    
    puts("Ultimate Chess: Background Color Settings\n");
    printf("What color would you like the %s background to be?\n\n", (isWhite? "white": "black"));
    puts("\t1. Black");
    puts("\t2. Red");
    puts("\t3. Green");
    puts("\t4. Yellow");
    puts("\t5. Blue");
    puts("\t6. Magenta");
    puts("\t7. Cyan");
    puts("\t8. Light Gray");
    puts("\t9. Terminal Default");
    puts("\t10. Dark Gray");
    puts("\t11. Light Red");
    puts("\t12. Light Green");
    puts("\t13. Light Yellow");
    puts("\t14. Light Blue");
    puts("\t15. Pink");
    puts("\t16. Light Cyan");
    puts("\t17. White\n");
    printf("Please enter an option: ");

}

/*!
 * \brief Gets an int as input from the keyboard
 * \returns uint32_t: Returns an int from the keyboard
 */
inline uint32_t fgetint(FILE* const file)
{

    uint32_t temp; // Variable to hold the int

    if(fscanf(file, " %u", &temp)) return temp; // Gets the input from the keyboard
    
    return UINT32_MAX;

}

// EOF //