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

//function to tokenize and return the portnumber
//clean this up by just setting return result[i] and pass in i instead!!!
char *tokenizePort(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    while ((tok = strtok(tok, " ")) != NULL){
        // printf("%s\n", tok);
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[1];
}
//clean up and take out
char *tokenizePort2(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    while ((tok = strtok(tok, " ")) != NULL){
        // printf("%s\n", tok);
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[2];
}
//clean up and take out
char *tokenizePort3(char readbuffer[]){

    char *tok = readbuffer;
    char *result[100];
    int i = 0;

    while ((tok = strtok(tok, " ")) != NULL){
        // printf("%s\n", tok);
        result[i] = tok;
        i++;
        tok = NULL;
    }
    return result[3];
}
//getDirectory function, returns all files in current directory
int *getDirectory(char dirBuffer[], char hostname1[]){

    struct dirent *dp;
    DIR *dir;
    char tempBuffer[1000];
    size_t bytes_read;

	memset(dirBuffer, '\0', sizeof(dirBuffer)); // Clear the buffer

    if((dir = opendir(".")) == NULL){
        printf("Error can't open");
    }
    strcat(dirBuffer, hostname1);
    strcat(dirBuffer, " ");
    do{
        if((dp = readdir(dir)) != NULL){

            strcat(dirBuffer, dp->d_name);
            strcat(dirBuffer, " ");             
        } 
    }while (dp != NULL); 
    
    // printf("We got: %s\n", dirBuffer);

    closedir(dir);
    return 0;
}
//checkDirectory function, returns yes or no if the directory asked for is in current directory
int checkDirectory(char dirBuffer[], char hostname1[], char* clientFile){

    struct dirent *dp;
    DIR *dir;
    char tempBuffer[1000];
    size_t bytes_read;

    if((dir = opendir(".")) == NULL){
        printf("Error can't open");
    }

    do{
        if((dp = readdir(dir)) != NULL){

            //look for a certain one, use this when validating that the file we are looking for is in the directory
            if(strcmp(dp->d_name, clientFile) == 0){
                    return 1;
            }          
        } 
    }while (dp != NULL); 

    // printf("We got: %s\n", dirBuffer);

    closedir(dir);
    return 0;
}


int process_child(char buffer[], int charsRead, int establishedConnectionFD, char encryptedString[180000], int portNumber, char hostname1[1024]){

    //CITE: From my OS5 assignment
	//making a buffer size of 180000 to have enough space to hold the large files + the large keys that are strcat together
	// char sendBuffer[1000000];

    // memset(sendBuffer, '\0', 1000000);	
    char *sendBuffer;
    sendBuffer = malloc(sizeof(char) * 10010000);
	char readBuffer[100000];
    // char sendBuffer[1000000];
    char *tempBuffer;
    tempBuffer = malloc(sizeof(char) * 10010000);
    // tempBuffer = (char *) malloc(10000000);

    // printf("Size of temp buffer: %d\n", 10010000);

    // char tempBuffer[1000005];
 
    memset(tempBuffer, '\0', 10010000); // Clear the buffer
	

	memset(sendBuffer, '\0', 10010000); // Clear the buffer

    memset(readBuffer, '\0', sizeof(readBuffer)); // Clear the buffer
    charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk

    // printf("WE RECIEVED: %s\n", readBuffer);

    char* clientPortNum;
    char* clientConnection;
    char sendDirectory[1000];
    char dirBuffer[1000];
    clientConnection = tokenizePort2(readBuffer);
    clientPortNum = tokenizePort(readBuffer);

    //we know we want to list
    if(readBuffer[1] == 'l'){

        printf("Connection from %s.\n\n", clientConnection);
        printf("List directory requested on port %s.\n", clientPortNum);
        printf("Sending directory contents to %s:%s\n", clientConnection, clientPortNum);

        //getDirectory function
        getDirectory(dirBuffer, hostname1);
        charsRead = send(establishedConnectionFD, dirBuffer, strlen(dirBuffer), 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

        return 0;
    }
    else if(readBuffer[1] == 'g'){      //we know we want to get
        sleep(5);
        //clean up and make one
        char* clientConnection1;
        clientConnection1 = tokenizePort3(readBuffer);
        char* clientFile;
        clientFile = tokenizePort(readBuffer);
        char* clientPort2;
        clientPort2 = tokenizePort2(readBuffer);

        printf("Connection from %s.\n", clientConnection1);
        printf("File \"%s\" requested on port %s.\n", clientFile, clientPort2);

        printf("Here8\n");

        int socketQ, portNumber1;
                printf("Here9\n");

	    struct sockaddr_in serverAddress2;
                printf("Here10\n");

	    struct hostent* serverHostInfo;
        printf("Here4\n");

        portNumber1 = atoi(clientPort2); 
        printf("Here5\n");

        memset((char*)&serverAddress2, '\0', sizeof(serverAddress2)); // Clear out the address struct
                printf("Here6\n");

        serverAddress2.sin_family = AF_INET; // Create a network-capable socket
                printf("Here7\n");

        serverAddress2.sin_port = htons(10294); // Store the port number
        // serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
        char* hostBuffer[1024];
        // int serverHostInfo;     

        printf("Here1\n");
        serverHostInfo = gethostbyname("flip3"); // Convert the machine name into a special form of address

        if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
        memcpy((char*)&serverAddress2.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
        printf("Here1\n");

        // Set up the socket
	    socketQ = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	    if (socketQ < 0) error("CLIENT: ERROR opening socket");
	        printf("Here1\n");

	    // Connect to server
	    if (connect(socketQ, (struct sockaddr*)&serverAddress2, sizeof(serverAddress2)) < 0) // Connect socket to address
		    error("CLIENT: ERROR connecting");
        

        //checking if file is in directory
        int check;
        int size = 0;
        check = checkDirectory(dirBuffer, hostname1, clientFile);
        if(check == 1){
            printf("YEP, IT'S THERE\n");

            int bytecount;
            char c;
            size_t bytes_read;


            FILE* fp = fopen(clientFile, "r");
    
            //read the contents of the file into sendBuffer
            // bytes_read = fread(sendBuffer, 1, sizeof(sendBuffer), fp);
            
            // fseek(fp, 0, SEEK_END);
            // size = ftell(fp);   
            // fseek(fp, 0, SEEK_SET);

            // printf("Size starts as: %d\n", size);

            bytes_read = fread(tempBuffer, 1, 10010000, fp);
            // printf("We read %s\n", tempBuffer);
            // size = size - sizeof(tempBuffer);
            // printf("Size is now: %d\n", size);

            // printf("What is in tempBuffer %s\n", tempBuffer);

            strcat(sendBuffer, hostname1);
            strcat(sendBuffer, tempBuffer);
            strcat(sendBuffer, "@!");


            int total = 0;
            int len = strlen(sendBuffer);
            int bytesleft = len;
            int n;

            while(total < len){

                n = send(socketQ, sendBuffer+total, bytesleft, 0);
                if(n == -1){
                    break;
                }
                total += n;
                bytesleft -= n;

            }

            len = total;

            printf("Total send: %d", total);



            //send buffer with contents of file
            // charsRead = send(establishedConnectionFD, sendBuffer, bytes_read, 0);

            printf("Sending \"%s\" to %s:%s\n", clientFile, clientConnection1, clientPort2);
            fclose(fp);

            close(socketQ);

        }
        else{
            //send back error if file is not in directory
            printf("File not found. Sending error message to %s:%s\n", clientConnection1, clientPort2);
            char *message;
            message = "FILE NOT FOUND@!";
            charsRead = send(socketQ, message, strlen(message), 0); // Send success back
            if (charsRead < 0) error("ERROR writing to socket");

            close(socketQ);
        }

        return 0;
    }
    else{
        printf("Invalid input\n");
        char *message;
        message = "ERROR - Invalid input";
        charsRead = send(establishedConnectionFD, message, strlen(message), 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

    }
	
    // close(establishedConnectionFD); // Close the existing socket which is connected to the client

}


//taking command line arguments
int main(int argc, char *argv[])
{
    //CITE from my OS5 assignment
	//creating a size of 180000 to hold large files
	char encryptedString[180000];

	//code from lecture
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

    //to get the hostname
    char hostname[1024];
    char hostname1[1024];
    hostname[1024] = '\0';
    gethostname(hostname, 1023);
    int i = 0;
    for(i = 0; i<5; i++){
        hostname1[i] = hostname[i];
    }

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
        
        //calls the function to process the child
        while(1){

            //put new connection here?
            //maybe inside process_child is better?

            process_child(buffer, charsRead, establishedConnectionFD, encryptedString, portNumber, hostname1);
            printf("Out of process_child\n");
        }

        //closes the connection
        // close(establishedConnectionFD);
        // exit(0);

	}
        close(establishedConnectionFD);
        exit(0);

		close(listenSocketFD); // Close the listening socket
	
		return 0; 

}