// #**************HEADER**************************************************************
// #Programmer name: Melissa Barr
// #Program name: chatclient.py
// #Program description: Client side of chat program that sets up client and contacts
// a server. Once accpeted, sends and recieves messages
// #Course name: CS372
// #Last modified: Febuary 12th, 2019
// #**********************************************************************************

//CITATIONS
// 1 - my OS server program
// 2 - beej -> https://beej.us/guide/bgnet/html/single/bgnet.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

// #****************Send Message function************
// #Description: function to send the message to the server, takes in the message, and sends it
// #Pre-conditions: server set up
// #Post-conditions: message sent, buffer4 cleared for next time
void sendMessage(int socketfd, char *buffer1){

    //initalization
	char buffer4[500];	
	int charsWritten = 0;

    //getting the users message        
    fgets(buffer4, 500, stdin);
    fflush(stdin);
    
    //CITATION 1
    if ((charsWritten = send(socketfd, buffer4, strlen(buffer4), 0)) < 0) {
            printf("CLIENT: Error sending data to chatserve host.\n");
            exit(1);
    }

    //clearning the buffer for next time
    memset(buffer4, 0, sizeof(buffer4));
}

// #****************Receive Message function************
// #Description: function that recieves message from the server, determines if there is an error, or if
// the server closed the connection. Otherwise, prints the message
// #Pre-conditions: server set up, conncetionOpen set to 1
// #Post-conditions: connectionOpen set to 2 if the server closed connection, message printed
void receiveMessage(int socketfd, char *buffer1, int *connectionOpen){

    //initalization
	char buffer3[500];
	int charsRead = NULL;
    
    //if there was an error recieving
    if ((charsRead = recv(socketfd, buffer3, 500, 0)) == 0) {
        printf("Server closed connection.\n");  
        *connectionOpen = 2;      
    } else {
        printf("%s\n", buffer3);
    }

    //clearing the buffer for next time
    memset(buffer3, 0, sizeof(buffer3));

}

// #****************Initiate Contact function************
// #Description: function that gets the clients handle and stores it
// #Pre-conditions: buffer1 allocated, server created
// #Post-conditions: socketfd returned, buffer1 holds the clients handle
int initiateContact(char *buffer1, int socketfd){

    //taking in the userHandle
    printf("Enter handle (one word, up to 10 characters): ");
    fgets(buffer1, 10, stdin);
    fflush(stdin);
    buffer1[strcspn(buffer1, "\n")] = 0;

    //sending the userHandle to the server
    send(socketfd, buffer1, strlen(buffer1), 0);

    return socketfd;

}

// #****************Main function************
// #Description: initalizes variables, does inital server setup, calls functions, and while loop
// chat functionality
// #Pre-conditions: hostname and port number entered correctly
// #Post-conditions: chat completed, client closed
int main(int argc, char **argv) {
	//checking for the correct number of arguments
    if (argc != 3) { 
       printf("Enter: chatclient executable, host, portNumber\n");
    }
    //defining variables
	int socketfd;
    int true = 1;
    char buffer1[500];
	char buffer2[500];
    int addressInfo;
    struct addrinfo hints, *servinfo, *p;

    //CITATION 2
    //making sure the struct is empty, TCP stream sockets
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    //CITATION 2
    //checking for errors
    if ((addressInfo = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(addressInfo));
        exit(1);
    }
    //CITATION 2
    //looping to connect
    for(p = servinfo; p != NULL; p = p->ai_next) {
        //making a socket
        if ((socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("CLIENT: socket error");
            }
        //connects to the server
        if (connect(socketfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socketfd);
            perror("CLIENT: connect error");
        }
        break;

    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }
    //-------------

    //adding the ">" to the userHandle
    socketfd = initiateContact(buffer1, socketfd);
    strcat(buffer1, "> ");

    //while loop for sending and recieving messages
    while(true){
        int connectionOpen = 1;
        printf("%s", buffer1);
        sendMessage(socketfd, buffer1);
        receiveMessage(socketfd, buffer1, &connectionOpen);
        //if connectionsOpen is not equal to 1 then it has been changed in either sending
        //or reciving and we want to close the socket
        if(connectionOpen != 1){
            close(socketfd);
            break;
        }
        
    }

    //CITATION 2
	freeaddrinfo(servinfo);

	return 0;
}



