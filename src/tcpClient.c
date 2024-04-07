#include "tcpClient.h"

static void QuitConnection(int sig);

static int FD;

const char *Program	/* program name for descriptive diagnostics */
	= "Client";

void QuitConnection(int sig)
{
    
    if(FD < 0)
    {
        printf("\nConnect End unsuccessful\n");
    }
    else
    {
	 EndConnection(FD);
	 printf("\nEnded Connection\n");
     exit(10);
    }
}

void FatalError(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */


int StartConnection(char *hostname,int PortNo)
{
	int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */

	 //Start Connnection
	Server = gethostbyname(hostname);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program, hostname);
        exit(10);
    }
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }

	SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	FD = SocketFD;
	memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    memcpy(&ServerAddress.sin_addr.s_addr,
		Server->h_addr_list[0], Server->h_length);
	
	connect(SocketFD, (struct sockaddr*)&ServerAddress,
			sizeof(ServerAddress));

	return SocketFD;

    
}

void EndConnection(int SocketFD)
{
	int l, n;
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

	//Send bye to server close connection
    strcpy(SendBuf, "bye");
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;
	
	close(SocketFD);
}

GameInfo ClientLogin(char *hostname,int PortNo)
{
	//int logined = 0;
	int l, n;
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

	GameInfo game = {.logined = 0, .playerNum = 0, .DataFD = -1};

    int SocketFD = StartConnection(hostname,PortNo);

	signal(SIGINT, QuitConnection);

    //Send LOGIN command
    strcpy(SendBuf, "LOGIN");
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   
		SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{   
		FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;


	printf("%s", RecvBuf); //print prompt for username:

	for(int i =0; i < 2; i++)//loop through 2 more time for login process
	{
	 scanf("%s",SendBuf); //prompt for user login info username or password
	 l = strlen(SendBuf);
	 if (SendBuf[l-1] == '\n')
	 {   
		 SendBuf[--l] = 0;
	 }

	 fflush(stdin);

     write(SocketFD, SendBuf, strlen(SendBuf));
	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	 if (n < 0) 
	 {   
		FatalError("reading from socket failed");
	 }
	 RecvBuf[n] = 0;
	 printf("%s", RecvBuf);
	 
	 fflush(stdin);
	}

	printf("\n"); //print new line

    //Check if Port is available for a new game
    //else check if login was successful
	if(strcmp(RecvBuf, "Game in Progress at Port") == 0)
	{
	 strcpy(SendBuf, "bye"); //Send bye to stop login process
	 l = strlen(SendBuf);
	 if (SendBuf[l-1] == '\n')
	 {   
		 SendBuf[--l] = 0;
	 }
	 write(SocketFD, SendBuf, strlen(SendBuf));
	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	 if (n < 0) 
	 {  
		  FatalError("reading from socket failed");
	 }
	 RecvBuf[n] = 0;
	 close(SocketFD); //close connection
	 printf("Port busy with another game\n");
	 sleep(5);
	 return game; //login failed
	}
	else if(strcmp(RecvBuf,"Login Successful") == 0)
	{
		game.logined = 1; //login success
	}
	else 
	{
	    game.logined = 0; //login failed
		
	}

    game.playerNum = ClientPlayerNum(hostname, PortNo, SocketFD);
	game.DataFD = SocketFD;

    //EndConnection(SocketFD);
	
	return game; //return if login success
}

int ClientPlayerNum(char *hostname,int PortNo, int SocketFD)
{
	int l, n;
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

    //Prompt Server for Player number Ex.Player 1
	strcpy(SendBuf, "PLAYERNUM");
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   
		SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    if (n < 0) 
	    {   FatalError("reading from socket failed");
	    }
	    RecvBuf[n] = 0;
	printf("Player %s\n", RecvBuf);

	return (int) (RecvBuf[0] - 48);
}

void ClientRegister(char *hostname,int PortNo)
{
	int l, n;
    int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

	signal(SIGINT, QuitConnection);


    SocketFD = StartConnection(hostname,PortNo);

	//Send REG command for registering account
    strcpy(SendBuf, "REG");
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   
		SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{   
		FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;

	printf("%s", RecvBuf); //Print input prompt
 
    //Loop 2 times for username and also password
	for(int i =0; i < 2; i++)
	{
	 scanf("%s",SendBuf);
	 l = strlen(SendBuf);
	 if (SendBuf[l-1] == '\n')
	 {   
		 SendBuf[--l] = 0;
	 }
	 fflush(stdin);
     write(SocketFD, SendBuf, strlen(SendBuf));//send username or password
	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	 if (n < 0) 
	 {   
		 FatalError("reading from socket failed");
	 }
	 RecvBuf[n] = 0;
	 printf("%s", RecvBuf);
	 fflush(stdin);
	}

	printf("\n");

	EndConnection(SocketFD);

}


void MakeMoveClient (char *hostname,int PortNo,int DataFD, Move move)
{
	int l, n;
    int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

    SocketFD = DataFD;

    //Send 'Add' server command for adding move
    strcpy(SendBuf, "ADD"); 
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   
		SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{   
		FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;
	fflush(stdin);

	//Send move from
    char str[3] = ""; //temp string to store move from and to
	IndexToString(move.start, str);//convert move.start index to move from string
	str[3] = '\n';
	strcpy(SendBuf,str);
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{
	   SendBuf[--l] = 0;
	}
	fflush(stdin);
    write(SocketFD, SendBuf, strlen(SendBuf));  //Send from char
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;

    //send move to
	IndexToString(move.end, str); //Convert move.end index to string
	str[3] = '\n';
	strcpy(SendBuf,str);
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	fflush(stdin);
    write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;

    //EndConnection(SocketFD);

}

Move ReceiveMoveClient (char *hostname,int PortNo, int DataFD)
{
	int l, n;
    int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

	Move move;

    SocketFD = DataFD;

    //Send Get command to get move from server
    strcpy(SendBuf, "GET"); //Send 'Get' server command
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    if (n < 0) 
	    {   FatalError("reading from socket failed");
	    }
	    RecvBuf[n] = 0;

    //Send FROM command to get move from
	strcpy(SendBuf, "FROM"); //Send 'From' server command
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;


    //Set move.start
	move.start = CharToIndex(RecvBuf[1],RecvBuf[0]); //convert from string to index

    //Send TO command to get move to
	strcpy(SendBuf, "TO"); //Send 'TO' server command
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;
	
	//Set move.end
	move.end = CharToIndex(RecvBuf[1],RecvBuf[0]); //convert to string to index

    //EndConnection(SocketFD);

	return move;
}

int IsWaiting(char *hostname,int PortNo, int DataFD)
{
	int l, n;
    int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */
	int wait;

	Move move;

    SocketFD = DataFD;

    //Send WAIT command to check if player still waiting on move from the other player
    strcpy(SendBuf, "WAIT"); //Send 'Get' server command
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{
	   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;
	
	//If received response is Waiting print Waiting with rotating wheel
	if(strcmp(RecvBuf,"Waiting")==0)
	{
	 printf("\r%s|", RecvBuf);
	 fflush(stdout);
	 sleep(1);
	 printf("\b/");
	 fflush(stdout);
	 sleep(1);
	 printf("\b-");
	 fflush(stdout);
	}

    //Set wait if response is Waiting
	if(strcmp(RecvBuf, "Waiting") == 0)
	{
		wait = 1; //waiting
	}
	else
	{
		wait = 0; //not waiting
	}

	return wait;//Return 1 or 0 is waiting or not

}


void GameResetClient(char *hostname,int PortNo, int DataFD)
{
	int l, n;
    int SocketFD;	/* socket file descriptor */
    struct sockaddr_in ServerAddress;	/* server address we connect with */
    struct hostent *Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

	SocketFD = DataFD;

    //Send Reset command to reset number of players
    strcpy(SendBuf, "RESET"); //Send 'Reset' server command
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	write(SocketFD, SendBuf, strlen(SendBuf));
	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	if (n < 0) 
	{
	   FatalError("reading from socket failed");
	}
	RecvBuf[n] = 0;

    EndConnection(SocketFD);

}