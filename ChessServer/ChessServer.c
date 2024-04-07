#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include "login.h"

void *ServerHandle(void *ClientFD);

void ServerQuitHandler(int sig);

int NumOfPlayer //store number of players
    = 0;

int Waiting //value whether player is waiting for move
    = 1;

int Shutdown = 0;

char from[256]; //store input move from temporary before passing move
char to[256];   //store input move from temporary before passing move

#define max_client 5

pthread_mutex_t lock;

int main(int argc, char *argv[])
{
    if (argc < 2) //if not port entered print usage of program
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(10);
    }

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "\nmutex init failed\n");
        exit(10);
    }

    int l, n;
    int ServSocketFD,           /* socket file descriptor for service */
        DataSocketFD;           /* socket file descriptor for data */
    int PortNo = atoi(argv[1]); /* port number */
    socklen_t ClientLen;
    struct sockaddr_in
        ServerAddress, /* server address (this host) */
        ClientAddress; /* client address we connect with */

    signal(SIGINT, ServerQuitHandler); //handle Ctrl c for server

    //Start Server
    printf("%d:Starting..\n", PortNo);
    ServSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (ServSocketFD < 0)
    {
        printf("service socket creation failed");
    }

    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;

    bind(ServSocketFD, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)); //bind socket

    listen(ServSocketFD, max_client); //listen up to 5 clients on socket

    pthread_t line;

    do
    {
        ClientLen = sizeof(ClientAddress);
        int DataSocketFD = accept(ServSocketFD, (struct sockaddr *)&ClientAddress, &ClientLen); //open new socket for connection
        int *socket = malloc(sizeof(int));
        *socket = DataSocketFD;
        pthread_create(&line, NULL, ServerHandle, socket);

    } while (!Shutdown);
    pthread_join(line, NULL);
    pthread_mutex_destroy(&lock);
    printf("%s: Shutting down.\n", argv[0]);
    close(ServSocketFD);
    return 0;
}

void ServerQuitHandler(int sig)
{
    Shutdown = 1;
    printf("\nShutdown Server\n");
}

void *ServerHandle(void *ClientFD)
{
    int DataSocketFD = *(int *)ClientFD;
    free(ClientFD);
    char RecvBuf[256];
    char SendBuf[256];
    char user[30]; //store user input username temporary
    char pass[30]; //store password input username temporary
    int n, l;
    int Login //incremental step for login protocol
        = 0;
    int Register //incremental step for register protocol
        = 0;
    int Add //incremental step for add move protocol
        = 0;
    int Get //incremental step for get move protocol
        = 0;

    int Bye = 0;

    do
    {

        //pthread_mutex_lock(&lock);
        n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf) - 1); //read from socket and copy to RecvBuf
        RecvBuf[n] = 0;
        printf("Server: Received message: %s\n", RecvBuf);

        if (0 == strcmp(RecvBuf, "bye")) //Bye Protocol
        {
            Bye = 1;

            if (NumOfPlayer != 0)
                NumOfPlayer--;

            strncpy(SendBuf, "server bye", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (0 == strcmp(RecvBuf, "shutdown")) //Shutdown server
        {
            strncpy(SendBuf, "server shutdown", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (0 == strcmp(RecvBuf, "LOGIN") && Login == 0) //Less than 2 player start login protocol
        {
            Login = 1;
            strncpy(SendBuf, "username:", sizeof(SendBuf) - 1); //prompt username
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if ((Login == 1) && (n > 0)) //login step 2
        {
            Login = 2;
            strncpy(user, RecvBuf, 30);                         //store username temporarily
            strncpy(SendBuf, "password:", sizeof(SendBuf) - 1); //prompt password
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if ((Login == 2) && (n > 0))
        {
            Login = 0;
            strncpy(pass, RecvBuf, 30); //store password temporarily

            if (NumOfPlayer >= 2)
            {
                strncpy(SendBuf, "Game in Progress at Port", sizeof(SendBuf) - 1);
                SendBuf[sizeof(SendBuf) - 1] = 0;
            }
            else if (signin(user, pass) == 1) //call sign in function with stored user, pass
            {
                strncpy(SendBuf, "Login Successful", sizeof(SendBuf) - 1);
                SendBuf[sizeof(SendBuf) - 1] = 0;
                NumOfPlayer += 1; //Login success add num of player
            }
            else //login unsuccessful
            {
                strncpy(SendBuf, "Login Unsuccessful", sizeof(SendBuf) - 1);
                SendBuf[sizeof(SendBuf) - 1] = 0;
            }
        }
        else if (0 == strcmp(RecvBuf, "REG") && Login == 0) //Start register protocol
        {
            Register = 1;
            strncpy(SendBuf, "username:", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Register == 1 && (n > 0))
        {
            Register = 2;
            strncpy(user, RecvBuf, 30); //get username
            strncpy(SendBuf, "password:", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Register == 2 && (n > 0))
        {
            Register = 0;
            strncpy(pass, RecvBuf, 30); //get password
            SendBuf[sizeof(SendBuf) - 1] = 0;
            account_reg(user, pass); //register account to text file
        }
        else if (0 == strcmp(RecvBuf, "PLAYERNUM") && Register == 0 && Login == 0) //start get player num protocol
        {
            sprintf(SendBuf, "%d", NumOfPlayer); //send number of player to client as string
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if ((0 == strcmp(RecvBuf, "ADD")) && Register == 0 && Login == 0) //start add move protocol
        {
            Add = 1;
            Waiting = 1;                                        //make sure waiting is true or 1
            strncpy(SendBuf, "Send From", sizeof(SendBuf) - 1); //Prompt for move from
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Add == 1 && (n > 0)) //add move step 2
        {
            Add = 2;
            strncpy(from, RecvBuf, 2); //store move from
            printf("%s\n", RecvBuf);
            strncpy(SendBuf, "Send To", sizeof(SendBuf) - 1); //Prompt for move to
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Add == 2 && (n > 0)) //add move step 3
        {
            Add = 0;
            strncpy(to, RecvBuf, 2); //store move to
            printf("%s\n", RecvBuf);
            strncpy(SendBuf, "Added Move", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
            Waiting = 0; //Set waiting to false or 0
        }
        else if (0 == strcmp(RecvBuf, "GET") && Register == 0 && Login == 0 && Waiting == 0) //Start Get move protocol when waiting is 0
        {
            Get = 1;
            strncpy(SendBuf, "Getting Move", sizeof(SendBuf) - 1);
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Get == 1 && (0 == strcmp(RecvBuf, "FROM"))) //Get move step 2
        {
            Get = 2;
            printf("%s\n", from);
            strncpy(SendBuf, from, sizeof(SendBuf) - 1); //send move from
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else if (Get == 2 && (0 == strcmp(RecvBuf, "TO"))) //Get move step 2
        {
            Get = 0;
            printf("%s\n", to);
            strncpy(SendBuf, to, sizeof(SendBuf) - 1); //send move to
            SendBuf[sizeof(SendBuf) - 1] = 0;
            Waiting = 1; //set waiting back to 1
        }
        else if (0 == strcmp(RecvBuf, "WAIT") && Register == 0 && Login == 0) //Start Is Waiting protocol
        {
            if (Waiting == 1) //Send "Waiting" if waiting = 1
            {
                strncpy(SendBuf, "Waiting", sizeof(SendBuf) - 1);
                SendBuf[sizeof(SendBuf) - 1] = 0;
            }
            else //Send "Not Waiting" if waiting = 0
            {
                strncpy(SendBuf, "Not Waiting", sizeof(SendBuf) - 1);
                SendBuf[sizeof(SendBuf) - 1] = 0;
            }
        }
        else if (0 == strcmp(RecvBuf, "RESET") && Register == 0 && Login == 0) //Start Reset Protocol
        {
            NumOfPlayer = 0;                                     //Reset number of players
            strncpy(SendBuf, "Game Ended", sizeof(SendBuf) - 1); //Game end at port
            SendBuf[sizeof(SendBuf) - 1] = 0;
        }
        else
        {
            strncpy(SendBuf, "server echo ", sizeof(SendBuf) - 1); //else Echo input from client
            SendBuf[sizeof(SendBuf) - 1] = 0;
            strncat(SendBuf, RecvBuf, sizeof(SendBuf) - 1 - strlen(SendBuf));
        }

        l = strlen(SendBuf);
        printf("Server: Sending response: %s.\n", SendBuf);
        n = write(DataSocketFD, SendBuf, l); //write SendBuf to socket
        //pthread_mutex_unlock(&lock);
    } while (!Bye && !Shutdown);
    printf("Server: Received last message from client, closing data connection.\n");
    close(DataSocketFD);
    return NULL;
}
