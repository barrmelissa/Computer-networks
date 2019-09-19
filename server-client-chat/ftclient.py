#CLIENT

#**************HEADER**************************************************************
#Programmer name: Melissa Barr
#Program name: client.py
#Program description: Client side of chat program that connects to server and
#asks either to list the servers directory or get a file
#Course name: CS372
#Last modified: March 7th, 2019
#**********************************************************************************

# from socket import *
import socket
import platform
import sys
import time
import os

#CITATIONS:
##1 Socket programming lecture from class, slides on TCP python sockets
##2 https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data

#****************Get Host Name function************
#Description: function to get the host name from the server
#Pre-conditions: data sent from server
#Post-conditions: host name taken out from data and returned
def gethostName(data):
    hostname1 = []
    for x in range(0, 5):
        hostname1.append(data[x])
    #join the chars together
    hostname2 = ''.join([str(x) for x in hostname1])
    return hostname2

#****************Recieve Data function************
#Description: function to start the data connection, recieve the data, close the data connection
#Pre-conditions: p connection set up
#Post-conditions: q connection set up and closed, data recieved
def recieveData(secondPort, fileName, serverHost, command):
    print 'Getting data... sleeping for 3 seconds'

    time.sleep(3)                                  

    #*****************CITATION #1 START***************
    #setting up socket to connect to second Q connection
    clientSocketQ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientSocketQ.connect((serverHost, secondPort))
    #*****************CITATION #1 END***************

    #if '-l' then we know we need to list directory
    if command == '-l':
        #getting the hostname
        modifiedSentence = clientSocketQ.recv(1024)
        newsentence = modifiedSentence.split(' ')
        hostname = newsentence[0] 
        newsentence.pop(0)
        print 'Reciving directory structure from', hostname, ':', secondPort, '\n'
        print 'NOTE: Please only request a .txt file.\n'
        for x in range(0, len(newsentence)):
            print newsentence[x]
        
    #if '-g' then we know we need to get file
    if command == '-g':

        #*****************CITATION #2 START***************
        #reciving the data in a loop for each chunk
        data = b''
        while True:
            part = clientSocketQ.recv(1024)
            data += part
            if part.endswith('@!'):
                break
        #*****************CITATION #2 END***************

        hostname2 = gethostName(data)
        data2 = hostname2 + 'FILE NOT FOUND@!'

        #checking if the file was found
        if data == data2:
            print '\n', hostname2, ':', secondPort, 'says FILE NOT FOUND'
        else:

            print 'Receiving "', fileName, '" from', hostname2, ':', secondPort, '\n'

            #looking through the current directory to see if we already have a file with the same name
            #if so, rename it with 'dup' as the prefix
            dirFiles = os.listdir('.')
            for x in range(0, len(dirFiles)):
                if fileName == dirFiles[x]:
                    fileName = 'dup' + fileName
            
            #open a new file with the new name, and write the data to it
            f = open(fileName, 'wb')   
            f.write(data[5:-2])
            f.close()
            print 'File transfer complete.\n'
    
    #close the connection
    clientSocketQ.close()

#****************Make request function************
#Description: function to set up and connect the initial connection
#Pre-conditions: server listening for connections
#Post-conditions: client connected, returned clientSocket
def makeRequest(combinedMessage, serverHost, serverPort):
    
    #*****************CITATION #1 START***************
    #setting up the socket to connect to the first p connection
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientSocket.connect((serverHost, serverPort))
    #*****************CITATION #1 END***************

    #sending the message
    clientSocket.send(combinedMessage)

    return clientSocket

#****************Initiate Contact function************
#Description: function to take in and validate command line arguments, initiate contact with the server
#Pre-conditions: command line arguments entered
#Post-conditions: client connected to server, close connection
def initiateContact():
    #taking in and validating the command line arguments
    if((len(sys.argv) < 5 or len(sys.argv) > 6)):
        print "Error, invalid command line arguments. Please enter python ftclient <hostname> <server_port> -l or -g <data_port>\n"
        exit()
    if sys.argv[3] != '-l' and sys.argv[3] != '-g':
        print "Error, please enter -l or -g.\n"
        exit()
    # if(len(sys.argv) == 6):
    #     textFile = sys.argv[4] 
    #     if textFile[-4:] != '.txt':
    #         print "Error, please enter a txt file.\n"
    #         exit()

    #setting all the variables
    serverHost = sys.argv[1]
    serverPort = int(sys.argv[2])
    command = sys.argv[3]
    clientHost = platform.node()
    clientHost1 = clientHost.split('.')
    clientHost1 = clientHost1[0]

    #putting together the message depending if we want to list or get
    if command == '-g':
        fileName = (sys.argv[4])
        secondPort = (sys.argv[5])
        combinedMessage = command + ' ' + fileName + ' ' + secondPort + ' ' + clientHost1
        secondPort = int(sys.argv[5])
    else:
        fileName = 'temp'
        secondPort = (sys.argv[4])
        combinedMessage = command + ' ' + secondPort + ' ' + clientHost1
        secondPort = int(sys.argv[4])
    
    clientSocket = makeRequest(combinedMessage, serverHost, serverPort)
    recieveData(secondPort, fileName, serverHost, command)
    clientSocket.close()

#****************Main function************
#Description: function to call the first function to initiate contact
#Pre-conditions: command line arguments
#Post-conditions: program is finished and quits
if  __name__ =='__main__':
    initiateContact()
    quit()