#**************HEADER**************************************************************
#Programmer name: Melissa Barr
#Program name: chatserve.py
#Program description: Server side of chat program that sets up server and waits
#to be contacted by a client, once contacted, accepts and sends and recieves
#messages
#Course name: CS372
#Last modified: Febuary 12th, 2019
#**********************************************************************************


#CITATIONS
# 1 - lecture 15 slide 9

from socket import *
import sys

#****************Start up function************
#Description: function to start up the port and host and listen until a 
#client requests, then accepts
#Pre-conditions: Host name and port number provided as appropriate arguments
#Post-conditions: Connection socket returned, server set up
def startUp(m):
    if(len(sys.argv) !=2):
        print "Enter: chatserve, portNumber\n"
    #CITATION 1
    serverPort = int(sys.argv[1])
    serverSocket = socket(AF_INET,SOCK_STREAM)
    serverSocket.bind(("", serverPort + m))
    serverSocket.listen(1)
    #----------
    print "The server is ready to receive, lets start chatting! (Let Client start with handle)"
    connectionSocket, addr = serverSocket.accept()

    return connectionSocket

#****************Receieve Message function************
#Description: function takes in the user defined handle for the client and the 
#predesignated handle for the server and recieves the message sent by the client
#Pre-conditions: userHandleB and connectionSocket defined, i set to 0
#Post-conditions: i set to 1 if user wants to quit, prints message
def recievemessage(userHandleB, connectionSocket, i):

    result = connectionSocket.recv(1024)
    if (result == "\quit\n"):
        i = 1
    else:
        sys.stdout.write('\x1b[6;30;42m' + userHandleB + "> " + result + '\x1b[0m')

    return i

#****************Send Message function************
#Description: function to send the message to the client, appends the userHandleA onto
#the beginning of it and sends the message, if the user wants to quit, sets i to 1
#Pre-conditions: userHandleA and connectionSocket defined, i set to 0
#Post-conditions: Connection socket returned, server set up
def sendmessage(userHandleA, connectionSocket, i):

    sys.stdout.write(userHandleA + "> ")
    text = raw_input("")
    if(text == "\quit"):
        i = 1
    else:
        output = userHandleA + "> " + text
        connectionSocket.send(output)

    return i

#****************Main function************
#Description: function that calls all the functions to set up, send and receive, loops
#and keeps the connection open after the client has closed it
#Pre-conditions: host name and port number provided as appropriate arguments
#Post-conditions: while loop executed, closed
if __name__ == "__main__":
    
    m = 0
    while 1:
        if m != 0:
            print "\nEnter a port number 1 more than you had originally."
        connectionSocket = startUp(m)
        userHandleA = "HostA"
        userHandleB = connectionSocket.recv(11)

        print "Initial message recieved from " + userHandleB + ". Please send first message from client."
        while 1:
            i = 0
            i = recievemessage(userHandleB, connectionSocket, i)
            if i == 1:
                connectionSocket.close()
                break
            i = sendmessage(userHandleA, connectionSocket, i)
            if i == 1:
                connectionSocket.close()
                break
        m = m + 1

       








