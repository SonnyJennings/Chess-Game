/*!
 * \file Settings.h
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the prototypes and custom types for the Settings Module of Ultimate Chess
 * \version 1.0
 * \date 2021-04-16
 * \copyright Copyright (c) 2021
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

// ------------------------- Dependencies ------------------------- //
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#include "Moves.h"
#include "Board.h"

// ------------------------- Types ------------------------- //
typedef enum
{
    Login,
    Register,
    PlayerNum,
    AddMove,
    GetMove
}ClientMode;

typedef struct 
{
 int logined;
 int playerNum;
 int DataFD;

}GameInfo;


// ------------------------- Functions ------------------------- //

//function retrieve player number from server
int ClientPlayerNum(char *hostname,int PortNo, int SocketFD);

//function start client for login protocol
GameInfo ClientLogin(char *hostname,int PortNo);

//function start client for register protocol
void ClientRegister(char *hostname,int PortNo);

//function start client for send move protocol
void MakeMoveClient (char *hostname,int PortNo, int DataFD, Move move);

//function start client for receive move protocol
Move ReceiveMoveClient (char *hostname,int PortNo, int DataFD);

//function start client to check if waiting
int IsWaiting(char *hostname,int PortNo, int DataFD);

//function start client for reseting number of players
void GameResetClient(char *hostname,int PortNo, int DataFD);

//Start a new connection
int StartConnection(char *hostname,int PortNo);

//End connection
void EndConnection(int SocketFD);





#endif

// EOF //
