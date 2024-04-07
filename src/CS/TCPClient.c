/*!
 * \file TCPClient.c
 * \author Orion Serup (oserup@uci.edu)
 * \author Ian Poremba (iporemba@uci.edu)
 * \author Raymond Duong (duongr2@uci.edu)
 * \author Chris Rodriguez (chriar10@uci.edu)
 * \author Sunshine Jennings (smjennin@uci.edu)
 * \brief Contains the implementation of the TCPClient Module of Ultimate Chess
 * \version 1.1
 * \date 2021-05-23
 * \copyright Copyright (c) 2021
 */

// ------------------------- Dependencies ------------------------- //

#include "TCPClient.h"

// ------------------------- Declarations ------------------------- //

static uint8_t ClientRecv(const ChessClient* const client, Message* const message);

// static void QuitConnection(int sig);

static pthread_t CreateMessageHandler(const ChessClient* const client, ChatBuffer* const buffer, Move* const move);

static void* MessageThread(void* data);

static void DeleteChatBuffer(ChatBuffer* const buffer);

static void AddChat(const ChessClient* const client, ChatBuffer* const buffer, const char* chatMessage);

static void CreateChatMessage(Message* const message, const char* chatMessage);

// ------------------------- Definintions ------------------------- //

/*!
 * \brief 
 * 
 */
typedef struct
{ 

	ChessClient* client; 
	ChatBuffer* buffer;
	Move* move;

} MessageThreadData; 

/*!
 * \brief Create a Message Handler object
 * 
 * \return pthread_t 
 */
pthread_t CreateMessageHandler(const ChessClient* const client, ChatBuffer* const buffer, Move* const move)
{

	pthread_t thread;

	MessageThreadData td = { (ChessClient*)client, buffer, move };

	pthread_create(&thread, NULL, MessageThread, &td);

	return thread;

}

/*!
 * \brief 
 * 
 * \param data 
 * \return void* 
 */
void* MessageThread(void* data)
{

	MessageThreadData* td = (MessageThreadData*)data;

	Message message;

	while(ClientRecv(td->client, &message))
	{

		if(message.header.type == MOVE_HEADER)
			memcpy(td->move, (Move*)message.body, sizeof(Move));

		else if(message.header.type == CHAT_HEADER)
		{

			pthread_mutex_lock((pthread_mutex_t*)&td->client->server->lock);

			AddChat(td->client, td->buffer, (char*)message.body);

			pthread_mutex_unlock((pthread_mutex_t*)&td->client->server->lock);

		}
	}

	pthread_exit(NULL);

}

/*!
 * \brief 
 * 
 * \param client 
 * \param port 
 * \param hostname 
 * \return true 
 * \return false 
 */
bool ConnectClient(ChessClient* const client, const uint16_t port, const char* hostname)
{

	if(port <= 2000)
	{

		fprintf(stderr, "Invalid Port Number, %d, Should Be > 2000", port);
		return false;

	}

	if(client->server == NULL)
	{

		pthread_mutex_lock(&client->server->lock); // lock the server editting to this thread

		client->server = CreateServer(hostname, port);

		pthread_mutex_unlock(&client->server->lock); // unlock the server lock after modifying
	
		if(!connect(client->server->SocketFD, (const struct sockaddr*)&client->server->socket, sizeof(client->server->socket)))
		{

			puts("Cannot Connect to Server, either is busy, or bad host/port");
			return false;

		}

		return true;

	}

	puts("Cannot Connect To A Server While Already Connected to One");

	return false;

}

/*!
 * \brief 
 * 
 * \param client 
 * \return true 
 * \return false 
 */
bool DisconnectClient(ChessClient* const client)
{
	
	if(client->server) // if we are connected to anything
	{

		close(client->server->SocketFD); // disconnect from the socket
		DeleteServer(client->server); // delete the server

		client->server = NULL; // set the server address to null

		puts("Disconnecting From Server... Disconnect Complete"); // send a status message

		return true; // return disconnect success

	}

	puts("Could Not Quit Connection, No Current Connection");

	return false; // if we arent connected to anything then we cant close

}

// void QuitConnection(int sig)
// {
//    
//     if(FD < 0)
//     {
//
//         printf("\nConnect End unsuccessful\n");
//
//     }
//     else
//     {
//
// 	 		EndConnection(FD);
// 	 		printf("\nEnded Connection\n");
//      	exit(10);
//
//     }
// }

/*!
 * \brief 
 * 
 * \param message 
 * \return uint8_t 
 */
uint8_t ClientSend(const ChessClient* const client, const Message* const message)
{

	if(client->server) // if we are connected to a server
	{

		Message response; // have a response to write into

		// write the header of the message so that the server can prepare for the body
		if(!write(client->server->SocketFD, &(message->header), sizeof(message->header)))
			puts("Can't Send Message Header To Server");

		// if the server is ok with the header, it will send it back
		if(!read(client->server->SocketFD, &response, sizeof(message->header)))
			puts("Cannot Read Data From Server");

		// if the header and the header from the server are the same then its ready for the body
		if(response.header.type == message->header.type)
			if(!write(client->server->SocketFD, &message->body, message->header.length))
				puts("Cannot Write Message Body to the Server");

		return message->header.length;

	}

	return 0;

}

/*!
 * \brief 
 * 
 * \param client 
 * \param message 
 * \return uint8_t 
 */
uint8_t ClientRecv(const ChessClient* const client, Message* const message)
{

	if(client->server)
	{

		if(read(client->server->SocketFD, &message->header, sizeof(message->header)) == -1) // get the message header
		{

			puts("Cannot Receieve Header From Server");
			return 0;

		}

		if(send(client->server->SocketFD, &message->header, sizeof(message->header), 0) == -1)
		{

			puts("Couldn't Send Header Acknowledgement");
			return 0;

		}
		
		if(read(client->server->SocketFD, &message->body, message->header.length) == -1) // read the amount of data that was supposed to be sent
		{

			puts("Cannot Send Header Acknowledgement");
			return 0;

		}

		return message->header.length;

	}

	puts("Cannot Read From Server");

	return 0;

}

/*!
 * \brief 
 * 
 * \param client 
 * \param user 
 * \param pass 
 * \return true 
 * \return false 
 */
bool ClientLogin(const ChessClient* const client, const char* const user, const char* const pass)
{
	
	if(client->server) // if we are connected to a server
	{

		Message loginmessage; // message to send to login

		memset(&loginmessage.body, 0, 60); // zero the array up until the send of the data

		strcpy((char*)loginmessage.body, user); // set the beginning of the info to the user
		strcpy((char*)loginmessage.body + 30, pass); // set the midpoint on to pass

		loginmessage.header.type = LOGIN_HEADER;
		loginmessage.header.length = 60; // 30 characters for user and pass max

		if(!ClientSend(client, &loginmessage))
			puts("Could Send Login Header To the Server");

		Message response;
		
		ClientRecv(client, &response);

		if(response.header.type == STATUS_HEADER)
		{
			
			if(response.body[0] != USER_OK)
			{

				puts("Invalid User Name, Please Register");
				return false;

			}

			if(response.body[1] != PASS_OK)
			{

				puts("Invalid Password");
				return false;

			}

			strcpy((char*)client->username, (char*)user);

			return 0;

		}
	}

	return false;

}

/*!
 * \brief Create a Move Message object
 * 
 * \param move 
 * \return Move 
 */
void CreateMoveMessage(Message* const message, const Move move)
{

	message->header.type = MOVE_HEADER;
	message->header.length = sizeof(Move);
	memcpy(message->body, &move, sizeof(Move));

}

/*!
 * \brief 
 * 
 * \param msg 
 */
void ErrorHandler(const char* msg)
{

	fputs("Client Error: ", stderr);
    perror(msg);
    fputs(": Exiting!\n", stderr);
    exit(20);

}

/*!
 * \brief 
 * 
 * \param client 
 * \param user 
 * \param pass 
 * \return true 
 * \return false 
 */
bool RegisterClient(const ChessClient* const client, const char* const user, const char* const pass)
{

	Message registermessage;

	registermessage.header.type = REGISTRATION_HEADER;
	registermessage.header.length = 60; // length of user and pass string

	strcpy((char*)registermessage.body, user);
	strcpy((char*)registermessage.body + 30, pass);

	if(!ClientSend(client, &registermessage))
		puts("Couldn't Send Registration Message");

	Message response;

	if(!ClientRecv(client, &response))
		puts("Couldn't Get Server Response");

	if(response.header.type == STATUS_HEADER)
	{

		if(response.body[0] != USER_OK)
		{

			puts("User Name Already Exists, Please Login");
			return false;

		}

		return true;
	}

	return false;

}

void DeleteChatBuffer(ChatBuffer* const buffer)
{
	
	STATIC_ASSERT(buffer, "Bad buffer Pointer");

    while(buffer->head) // Checks for a head of the chatbuffer
	{

		STATIC_ASSERT(buffer->head, "Invalid Chat Pointer");
		STATIC_ASSERT(buffer->head->text, "Invalid Chat Message Pointer");

		Chat* temp = buffer->head; // Sets a temporary to hold the top chat in the stack
		
		if(!buffer->head) return; // If the chat stack is empty then return

		else if(buffer->size == 1) // If theres only one element in the stack
		{

			free(temp->text); // Frees the chat text
			free(temp); // Frees the memory for the head

		}

		else // If theres more than one element in the stack
		{ 

			buffer->head = temp->next; // Sets the new stacks head to the second in the stack
			
			free(temp->text); // Frees the chat text
			free(temp); // Frees the top of the stack

		}

		buffer->size--; // Reduces the size of the stack

	}

	buffer->head = (Chat*)NULL; // Resets the head
	buffer->tail = (Chat*)NULL; // Resets the tail
	buffer->size = 0;

}

void AddChat(const ChessClient* const client, ChatBuffer* const buffer, const char* chatMessage)
{
	
	STATIC_ASSERT(buffer, "Bad buffer Pointer");
	
	STATIC_ASSERT(chatMessage, "Bad chatMessage Pointer");

	Message message; 
	
	CreateChatMessage(&message, chatMessage);

	if(!ClientSend(client, &message))
		puts("Couldn't Send Chat Message");

	else
	{
			
		Chat* chat = malloc(sizeof(Chat)); // Allocates memory for the size of the chat struct

		chat->text = malloc(strlen(chatMessage) + 1); // Allocates memory for the chat message
		
		strcpy(chat->text, chatMessage); // Sets the chat text to the chat message

		if(buffer->size) // Checks if there is anything in the chat buffer
		{

			buffer->head->prev = chat; // Sets the old heads previous chat to the chat
			chat->next = buffer->head; // Sets the chats next to be the previous chat head
			buffer->head = chat; // Sets a new head to the chatbuffer

		}
		else
		{

			chat->next = (Chat*)NULL; // Sets the chats next to be null
			buffer->head = chat; // Sets a new head to the chatbuffer
			buffer->tail = chat; // Sets a new tail to the chatbuffer

		}

		chat->prev = (Chat*)NULL; // Sets the chats prev to be null
		buffer->size++; // Increases the size of the chat buffer

	}
}

void CreateChatMessage(Message* const message, const char* chatMessage)
{

	message->header.type = CHAT_HEADER;
	message->header.length = (strlen(chatMessage) + 1);
	memcpy(message->body, chatMessage, (strlen(chatMessage) + 1));

}

void PrintMessages(ChatBuffer* const buffer)
{

	Chat* temp = buffer->tail; // Sets the selected chat message to the bottom of the chat stack 

    if(temp)
    {

        for(size_t i = 0; i < buffer->size; i++) // For all the chat messages in the stack
        { 

            printf("%s\n\n", temp->text); // Prints the list of messages
            temp = temp->prev;

        }
    }
}

void ChatDisplay(const ChessClient* const client, ChatBuffer* const buffer)
{

	bool go = true;
	char input = 'R';

	do
	{

		ClearScreen();

		PrintMessages(buffer);

		puts("\nWelcome to the chat menu\n");
		puts("Press R to refresh the messages");
		puts("Press T to send a new message");
		puts("Press Q to leave the chat menu");
		printf("\nYour selection: ");

		input = toupper(getchar());

		if(input == 'R')
			continue;

		else if(input == 'T')
		{

			char message[100];
			
			uint8_t n = 0;

			ClearScreen();

			PrintMessages(buffer);
			printf("\n\nYour message: ");
			scanf("%s", &message);
			AddChat(client, buffer, message);

		}

		else if(input == 'Q')
		{

			go = false;

			ClearScreen();

			printf("You are leaving the chat press enter to return to the game");
			getchar();

			ClearScreen();

		}

	} while(go);

}

// int StartConnection(char *hostname,int PortNo)
// {
// 	int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */

// 	 //Start Connnection
// 	Server = gethostbyname(hostname);
//     if (Server == NULL)
//     {   fprintf(stderr, "%s: no such host named '%s'\n", Program, hostname);
//         exit(10);
//     }
//     if (PortNo <= 2000)
//     {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
// 		Program, PortNo);
//         exit(10);
//     }

// 	SocketFD = socket(AF_INET, SOCK_STREAM, 0);
// 	FD = SocketFD;
// 	memset(&ServerAddress, 0, sizeof(ServerAddress));
//     ServerAddress.sin_family = AF_INET;
//     ServerAddress.sin_port = htons(PortNo);
//     memcpy(&ServerAddress.sin_addr.s_addr,
// 		Server->h_addr_list[0], Server->h_length);
	
// 	connect(SocketFD, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));

// 	return SocketFD;

    
// }

// void EndConnection(int SocketFD)
// {
// 	int l, n;
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

// 	//Send bye to server close connection
//     strcpy(SendBuf, "bye");
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;
	
// 	close(SocketFD);
// }

// GameInfo ClientLogin(char *hostname,int PortNo)
// {
// 	//int logined = 0;
// 	int l, n;
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

// 	GameInfo game = {.logined = 0, .playerNum = 0, .DataFD = -1};

//     int SocketFD = StartConnection(hostname,PortNo);

// 	signal(SIGINT, QuitConnection);

//     //Send LOGIN command
//     strcpy(SendBuf, "LOGIN");
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   
// 		SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{   
// 		FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;


// 	printf("%s", RecvBuf); //print prompt for username:

// 	for(int i =0; i < 2; i++)//loop through 2 more time for login process
// 	{
// 	 scanf("%s",SendBuf); //prompt for user login info username or password
// 	 l = strlen(SendBuf);
// 	 if (SendBuf[l-1] == '\n')
// 	 {   
// 		 SendBuf[--l] = 0;
// 	 }

// 	 fflush(stdin);

//      write(SocketFD, SendBuf, strlen(SendBuf));
// 	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	 if (n < 0) 
// 	 {   
// 		FatalError("reading from socket failed");
// 	 }
// 	 RecvBuf[n] = 0;
// 	 printf("%s", RecvBuf);
	 
// 	 fflush(stdin);
// 	}

// 	printf("\n"); //print new line

//     //Check if Port is available for a new game
//     //else check if login was successful
// 	if(strcmp(RecvBuf, "Game in Progress at Port") == 0)
// 	{
// 	 strcpy(SendBuf, "bye"); //Send bye to stop login process
// 	 l = strlen(SendBuf);
// 	 if (SendBuf[l-1] == '\n')
// 	 {   
// 		 SendBuf[--l] = 0;
// 	 }
// 	 write(SocketFD, SendBuf, strlen(SendBuf));
// 	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	 if (n < 0) 
// 	 {  
// 		  FatalError("reading from socket failed");
// 	 }
// 	 RecvBuf[n] = 0;
// 	 close(SocketFD); //close connection
// 	 printf("Port busy with another game\n");
// 	 sleep(5);
// 	 return game; //login failed
// 	}
// 	else if(strcmp(RecvBuf,"Login Successful") == 0)
// 	{
// 		game.logined = 1; //login success
// 	}
// 	else 
// 	{
// 	    game.logined = 0; //login failed
		
// 	}

//     game.playerNum = ClientPlayerNum(hostname, PortNo, SocketFD);
// 	game.DataFD = SocketFD;

//     //EndConnection(SocketFD);
	
// 	return game; //return if login success
// }

// int ClientPlayerNum(char *hostname,int PortNo, int SocketFD)
// {
// 	int l, n;
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

//     //Prompt Server for Player number Ex.Player 1
// 	strcpy(SendBuf, "PLAYERNUM");
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   
// 		SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	    if (n < 0) 
// 	    {   FatalError("reading from socket failed");
// 	    }
// 	    RecvBuf[n] = 0;
// 	printf("Player %s\n", RecvBuf);

// 	return (int) (RecvBuf[0] - 48);
// }

// void ClientRegister(char *hostname,int PortNo)
// {
// 	int l, n;
//     int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

// 	signal(SIGINT, QuitConnection);

//     SocketFD = StartConnection(hostname,PortNo);

// 	//Send REG command for registering account
//     strcpy(SendBuf, "REG");
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   
// 		SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{   
// 		FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;

// 	printf("%s", RecvBuf); //Print input prompt
 
//     //Loop 2 times for username and also password
// 	for(int i =0; i < 2; i++)
// 	{
// 	 scanf("%s",SendBuf);
// 	 l = strlen(SendBuf);
// 	 if (SendBuf[l-1] == '\n')
// 	 {   
// 		 SendBuf[--l] = 0;
// 	 }
// 	 fflush(stdin);
//      write(SocketFD, SendBuf, strlen(SendBuf));//send username or password
// 	 n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	 if (n < 0) 
// 	 {   
// 		 FatalError("reading from socket failed");
// 	 }
// 	 RecvBuf[n] = 0;
// 	 printf("%s", RecvBuf);
// 	 fflush(stdin);
// 	}

// 	printf("\n");

// 	EndConnection(SocketFD);

// }


// void MakeMoveClient (char *hostname,int PortNo,int DataFD, Move move)
// {
// 	int l, n;
//     int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

//     SocketFD = DataFD;

//     //Send 'Add' server command for adding move
//     strcpy(SendBuf, "ADD"); 
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   
// 		SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{   
// 		FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;
// 	fflush(stdin);

// 	//Send move from
//     char str[3] = ""; //temp string to store move from and to
// 	IndexToString(move.start, str);//convert move.start index to move from string
// 	str[3] = '\n';
// 	strcpy(SendBuf,str);
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{
// 	   SendBuf[--l] = 0;
// 	}
// 	fflush(stdin);
//     write(SocketFD, SendBuf, strlen(SendBuf));  //Send from char
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;

//     //send move to
// 	IndexToString(move.end, str); //Convert move.end index to string
// 	str[3] = '\n';
// 	strcpy(SendBuf,str);
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	fflush(stdin);
//     write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;

//     //EndConnection(SocketFD);

// }

// Move ReceiveMoveClient (char *hostname,int PortNo, int DataFD)
// {
// 	int l, n;
//     int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

// 	Move move;

//     SocketFD = DataFD;

//     //Send Get command to get move from server
//     strcpy(SendBuf, "GET"); //Send 'Get' server command
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	    if (n < 0) 
// 	    {   FatalError("reading from socket failed");
// 	    }
// 	    RecvBuf[n] = 0;

//     //Send FROM command to get move from
// 	strcpy(SendBuf, "FROM"); //Send 'From' server command
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;


//     //Set move.start
// 	move.start = CharToIndex(RecvBuf[1],RecvBuf[0]); //convert from string to index

//     //Send TO command to get move to
// 	strcpy(SendBuf, "TO"); //Send 'TO' server command
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;
	
// 	//Set move.end
// 	move.end = CharToIndex(RecvBuf[1],RecvBuf[0]); //convert to string to index

//     //EndConnection(SocketFD);

// 	return move;
// }

// int IsWaiting(char *hostname,int PortNo, int DataFD)
// {
// 	int l, n;
//     int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */
// 	int wait;

// 	Move move;

//     SocketFD = DataFD;

//     //Send WAIT command to check if player still waiting on move from the other player
//     strcpy(SendBuf, "WAIT"); //Send 'Get' server command
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{
// 	   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;
	
// 	//If received response is Waiting print Waiting with rotating wheel
// 	if(strcmp(RecvBuf,"Waiting")==0)
// 	{
// 	 printf("\r%s|", RecvBuf);
// 	 fflush(stdout);
// 	 sleep(1);
// 	 printf("\b/");
// 	 fflush(stdout);
// 	 sleep(1);
// 	 printf("\b-");
// 	 fflush(stdout);
// 	}

//     //Set wait if response is Waiting
// 	if(strcmp(RecvBuf, "Waiting") == 0)
// 	{
// 		wait = 1; //waiting
// 	}
// 	else
// 	{
// 		wait = 0; //not waiting
// 	}

// 	return wait;//Return 1 or 0 is waiting or not

// }


// void GameResetClient(char *hostname,int PortNo, int DataFD)
// {
// 	int l, n;
//     int SocketFD;	/* socket file descriptor */
//     struct sockaddr_in ServerAddress;	/* server address we connect with */
//     struct hostent *Server;	/* server host */
//     char SendBuf[256];	/* message buffer for sending a message */
//     char RecvBuf[256];	/* message buffer for receiving a response */

// 	SocketFD = DataFD;

//     //Send Reset command to reset number of players
//     strcpy(SendBuf, "RESET"); //Send 'Reset' server command
// 	l = strlen(SendBuf);
// 	if (SendBuf[l-1] == '\n')
// 	{   SendBuf[--l] = 0;
// 	}
// 	write(SocketFD, SendBuf, strlen(SendBuf));
// 	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
// 	if (n < 0) 
// 	{
// 	   FatalError("reading from socket failed");
// 	}
// 	RecvBuf[n] = 0;

//     EndConnection(SocketFD);

// }