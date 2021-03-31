/* client.c */ 
/* Programmed by Paul Traumiller and Sohan G */
/* Due March 30, 2021 */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <arpa/inet.h>
#include "parallelTools.h"


int sendFilename(int socket, char filename[SIZE]);
int countSent, countHeader, countData, byteTotal;
FILE* outputFile;

int lineNum;
char dataSet[SIZE][MAX_LINE+1];

int sendFilename(int socket, char filename[SIZE]){
    unsigned int dataLength;
    dataLength = strlen(filename)+1;
    countSent = send(socket, filename, dataLength,0);
    printf("Client: Filename sent = %s\n", filename);
    return countSent;
}



int findServer()
{
    int openSocket;
    struct sockaddr_in serverAddress;
    struct hostent * serverIP;

    char serverHostname[SIZE];
    unsigned short serverPort;

    //Initializing TCP Socket

    if ((openSocket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP)) < 0){
        perror("Client: Error opening socket");
        exit(1);
    }
    printf("Client: Server socket created successfully.\n");
    
    serverIP = gethostbyname(SERVER_NAME);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    memcpy( (char *)&serverAddress.sin_addr, serverIP->h_addr, serverIP->h_length);
    serverAddress.sin_port = htons(SERVER_PORT);

    //connecting to server

    if (connect(openSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))<0){
        perror("Client: Can't connect to server");
        close(openSocket);
        exit(1);
    }
    printf("Client: Connected to server\n");
    return openSocket;
}

int main(){
    outputFile = fopen("out.txt", "w");
    //initialize cleint data
    int port = SERVER_PORT;
    int sequence;
    int clientSocket;
    char filename[SIZE];
    printf("Enter name of file to be transferred:\n");
    scanf("%s",filename);

    clientSocket = findServer();

    if ((sendFilename(clientSocket,filename))<0)
    {
        printf("Client: Error sending filename.\n");
        close(clientSocket);
        exit(1);
    }
    printf("Client: Filename sent successfully\n");


    //variables for incoming data
    char buffer[MAX_LINE + 1];
    struct Header headerBuff;
    struct Header *ptr = &headerBuff;
    short int sequenceBuff, localSequence;
    ReadPacket(clientSocket, &headerBuff, buffer);
    while (headerBuff.count != 0)
	{
		sequence++;
		byteTotal += headerBuff.count;
		printf("Client: Packet %d received with %d data bytes\n", headerBuff.sequence, headerBuff.count);
		if (!fputs(buffer, outputFile))
			perror("fputs() failed");
		
		ReadPacket(clientSocket, &headerBuff, buffer);
	}  
    printf("Client: End of transmission packet with sequence number %d received with %lu data bytes\n",headerBuff.sequence, strlen(buffer)+HEADER_SIZE);

    printf("Client: Total number of packets received = %d\n", sequence);
    printf("Client: Total number of bytes received = %d\n", byteTotal);
    
    printf("Client: Closing connection...\n");


    close(clientSocket);
    
    return 0;
}