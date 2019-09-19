//**************HEADER**************************************************************
//Programmer name: Melissa Barr
//Program name: ftserver.c
//Program description: Server side of file transfer program that sets up server and waits
//to be contacted by a client, once contacted, accepts and sends and recieves
//directories/files requested
//Course name: CS372
//Last modified: March 7th, 2019
//**********************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <dirent.h>

//CITATIONS:
//#1: From my Operating Systems assignment #5 in CS344

// #****************Tokenize Port function************
// #Description: function to tokenize and return the client file
// #Pre-conditions: readbuffer that holds message sent
// #Post-conditions: client file tokenized and returned
char *tokenizePort(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    //tokenizes the message sent in readBuffer
    while ((tok = strtok(tok, " ")) != NULL){
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[1];
}

// #****************Tokenize Port2 function************
// #Description: function to tokenize and return the portnumber
// #Pre-conditions: readbuffer that holds message sent
// #Post-conditions: port number tokenized and returned
char *tokenizePort2(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    //tokenizes the message sent in readBuffer
    while ((tok = strtok(tok, " ")) != NULL){
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[2];
}

// #****************Tokenize Port3 function************
// #Description: function to tokenize and return the clientConnection
// #Pre-conditions: readbuffer that holds message sent
// #Post-conditions: clientConnection tokenized and returned
char *tokenizePort3(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    //tokenizes the message sent in readBuffer
    while ((tok = strtok(tok, " ")) != NULL){
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[3];
}

// #****************Get Directory function************
// #Description: function to return all files in the current directory
// #Pre-conditions: dirBuffer to hold the directory and hostname to append
// #Post-conditions: dirBuffer filled with all files in the current directory
int *getDirectory(char dirBuffer[], char hostname1[]){

    struct dirent *dp;
    DIR *dir;
    char tempBuffer[1000];
    size_t bytes_read;

	memset(dirBuffer, '\0', sizeof(dirBuffer)); // Clear the buffer

    //opens the directory
    if((dir = opendir(".")) == NULL){
        printf("Error can't open");
    }
    strcat(dirBuffer, hostname1);
    strcat(dirBuffer, " ");
    
    //reads in all files in the directory
    do{
        if((dp = readdir(dir)) != NULL){

            strcat(dirBuffer, dp->d_name);
            strcat(dirBuffer, " ");             
        } 
    }while (dp != NULL); 
    
    closedir(dir);
    return 0;
}

// #****************Check Directory function************
// #Description: function to return yes or no if the file asked is in the current directory
// #Pre-conditions: dirBuffer to hold the directory, host name to append, and clientFile to look for
// #Post-conditions: return 0 or 1 if the file is in the current directory
int checkDirectory(char dirBuffer[], char hostname1[], char* clientFile){

    struct dirent *dp;
    DIR *dir;
    char tempBuffer[1000];
    size_t bytes_read;

    //opens the directory
    if((dir = opendir(".")) == NULL){
        printf("Error can't open");
    }

    //reads in all files in the directory
    do{
        if((dp = readdir(dir)) != NULL){

            //looking if the fileName requested is in the current directory
            if(strcmp(dp->d_name, clientFile) == 0){
                    return 1;
            }          
        } 
    }while (dp != NULL); 

    closedir(dir);
    return 0;
}

// #****************Process Connection function************
// #Description: this function does the bulk of the work, creates memory to hold the files sent, recieves the message
// from the client, sets up data connection, sends file or directory, closes data connection
// #Pre-conditions: first p connection established
// #Post-conditions: q connection started and closed, data sent via q connection
int process_connection(char buffer[], int charsRead, int establishedConnectionFD, int portNumber, char hostname1[1024]){

    //*****************CITATION #1 START***************
    //setting memory
    char *sendBuffer;
    sendBuffer = malloc(sizeof(char) * 10010000);
	char readBuffer[100000];
    char *tempBuffer;
    tempBuffer = malloc(sizeof(char) * 10010000);

    memset(tempBuffer, '\0', 10010000); // Clear the buffer
	memset(sendBuffer, '\0', 10010000); // Clear the buffer
    memset(readBuffer, '\0', sizeof(readBuffer)); // Clear the buffer
    charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
    //*****************CITATION #1 END***************

    char* clientPortNum;
    char* clientConnection;
    char sendDirectory[1000];
    char dirBuffer[1000];
    clientConnection = tokenizePort2(readBuffer);
    clientPortNum = tokenizePort(readBuffer);

    char* clientPort2;
    char* clientFile;
    char* clientConnection1;

    if(readBuffer[1] == 'g'){
        clientConnection1 = tokenizePort3(readBuffer);
        clientFile = tokenizePort(readBuffer);
        clientPort2 = tokenizePort2(readBuffer);
    }

    //*****************CITATION #1 START***************
    //setting variables
	int listenSocketQ, establishedConnectionQ, portNumber2;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress2, clientAddress2;

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress2, '\0', sizeof(serverAddress2)); // Clear out the address struct
	if(readBuffer[1] == 'g'){
        portNumber = atoi(clientPort2); // Get the port number, convert to an integer from a string
    }
    else{
        portNumber = atoi(clientPortNum); // Get the port number, convert to an integer from a string

    }
    serverAddress2.sin_family = AF_INET; // Create a network-capable socket
	serverAddress2.sin_port = htons(portNumber); // Store the port number
	serverAddress2.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketQ = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketQ < 0) error("ERROR opening socket");

    if (setsockopt(listenSocketQ, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
        error("setsockopt(SO_REUSEADDR) failed");

	// Enable the socket to begin listening
	if (bind(listenSocketQ, (struct sockaddr *)&serverAddress2, sizeof(serverAddress2)) < 0) // Connect socket to port
		printf("ERROR on binding");

	listen(listenSocketQ, 5); // Flip the socket on - it can now receive up to 5 connections

    sizeOfClientInfo = sizeof(clientAddress2); // Get the size of the address for the client that will connect
    establishedConnectionQ = accept(listenSocketQ, (struct sockaddr *)&clientAddress2, &sizeOfClientInfo); // Accept
    
    if (establishedConnectionQ < 0){
        printf("ERROR1 on accept\n");
    }        
    //*****************CITATION #1 END***************

    //we know we want to list
    if(readBuffer[1] == 'l'){

        printf("Connection from %s.\n\n", clientConnection);
        printf("List directory requested on port %s.\n", clientPortNum);
        printf("Sending directory contents to %s:%s\n", clientConnection, clientPortNum);

        //getDirectory function
        getDirectory(dirBuffer, hostname1);
        //send the directory
        charsRead = send(establishedConnectionQ, dirBuffer, strlen(dirBuffer), 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

        //close connection
        close(establishedConnectionQ);
        close(listenSocketQ);

        return 0;
    }
    else if(readBuffer[1] == 'g'){      //we know we want to get
        
        printf("Connection from %s.\n", clientConnection1);
        printf("File \"%s\" requested on port %s.\n", clientFile, clientPort2);

        //checking if file is in directory
        int check;
        int size = 0;
        check = checkDirectory(dirBuffer, hostname1, clientFile);
        if(check == 1){

            int bytecount;
            char c;
            size_t bytes_read;

            //open and read the file
            FILE* fp = fopen(clientFile, "r");
            bytes_read = fread(tempBuffer, 1, 10010000, fp);

            //adding the hostname to the front of the file
            strcat(sendBuffer, hostname1);
            strcat(sendBuffer, tempBuffer);
            strcat(sendBuffer, "@!");

            int total = 0;
            int len = strlen(sendBuffer);
            int bytesleft = len;
            int n;

            //sending the file in chunks
            while(total < len){

                n = send(establishedConnectionQ, sendBuffer+total, bytesleft, 0);

                if(n == -1){
                    break;
                }
                total += n;
                bytesleft -= n;

            }

            len = total;

            printf("Sending \"%s\" to %s:%s\n", clientFile, clientConnection1, clientPort2);
            printf("Total sent: %d bytes\n", total);

            fclose(fp);

            //close connection
            close(establishedConnectionQ);
            close(listenSocketQ);

        }
        else{
            //send back error if file is not in directory
            printf("File not found. Sending error message to %s:%s\n", clientConnection1, clientPort2);
            char totalMessage[2024];
            //adding the hostname ot the front of the message
            strcat(totalMessage, hostname1);
            strcat(totalMessage, "FILE NOT FOUND@!");
            charsRead = send(establishedConnectionQ, totalMessage, strlen(totalMessage), 0); // Send success back
            if (charsRead < 0) error("ERROR writing to socket");
            memset(totalMessage, '\0', 2024); // Clear the buffer

            //close connection
            close(establishedConnectionQ);
            close(listenSocketQ);
        }

        return 0;
    }
    else{
        //if it is neither of the above, then invalid input
        printf("Invalid input\n");
        char *message;
        message = "ERROR - Invalid input";
        charsRead = send(establishedConnectionQ, message, strlen(message), 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

        close(establishedConnectionQ);
        close(listenSocketQ);

    }
	
}

// #****************Main function************
// #Description: function to take in and validate the command line arguments, set up first p connection
// #Pre-conditions: command line arguments
// #Post-conditions: p connection set up and listening until ended via a SIGINT
int main(int argc, char *argv[])
{

    printf("Hello, you need to login in order to use the server\n");
    printf("Username: \n");
    char userName[1024];
    scanf("%s", userName);
    printf("Password(case sensitive): \n");
    char userPass[1024];
    scanf("%s", userPass);
    if(strcmp(userPass, "computerNetworksYay") != 0){
        printf("Incorrect password, please look at the README\n");
        exit(0);
    }
    else{
        printf("Great, lets get started!\n\n");
    }


    //*****************CITATION #1 START***************
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[180000];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc != 2 ) { fprintf(stderr,"USAGE: %s <SERVER_PORT>\n", argv[0]); exit(1); } // Check usage & args
	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
    //*****************CITATION #1 END***************

    //to get the hostname
    char hostname[1024];
    char hostname1[1024];
    hostname[1024] = '\0';
    gethostname(hostname, 1023);
    int i = 0;
    for(i = 0; i<5; i++){
        hostname1[i] = hostname[i];
    }

    //*****************CITATION #1 START***************
	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		printf("ERROR on binding");

	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
    printf("Server open on %d \n\n", portNumber);

	//while loop that accepts the message
	while(1){

		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0){
			printf("ERROR1 on accept\n");
		}
        //*****************CITATION #1 END***************

        
        process_connection(buffer, charsRead, establishedConnectionFD, portNumber, hostname1);
    

        //closes the connection
        close(establishedConnectionFD);

	}

		close(listenSocketFD); // Close the listening socket
	
		return 0; 

}