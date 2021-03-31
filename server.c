/* server.c */ 
/* Programmed by Paul Traumiller and Sohan G */
/* Due March 30, 2021 */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <netinet/in.h>
#include <arpa/inet.h>
#include "parallelTools.h"


void sendFile(int socket, char* filename);
void sendPacket(int socket, int sN, char* buffer);

void sendFile(int socket, char* filename)
{
    FILE* theFile = fopen(filename, "r");
    char buffer[MAX_LINE +1];
    short int sN = 0;
    int bytes = 0;

    while(fgets(buffer,MAX_LINE+1,theFile)){
        sendPacket(socket, sN++, buffer);
        bytes = bytes + strlen(buffer) + HEADER_SIZE;
    }
    sendPacket(socket,sN,"");
    fclose(theFile);
    printf("Server: Total number of packets transmitted = %hd\n",sN+1);
    printf("Server: Total number of data bytes = %d\n",bytes);
}

void sendPacket(int socket, int sN, char* buffer){
    struct Header header;
    header.count = htons(strlen(buffer));
    header.sequence = htons(sN);
    send(socket, &header, sizeof(header),0);
    send(socket, buffer, strlen(buffer),0);
    if (strlen(buffer)>0){
        printf("Server: Packet %d transmitted with %lu data bytes \n",sN, strlen(buffer)+HEADER_SIZE);

    }
    else{
        printf("Server: End of transmission packet with sequence number %d transmitted with %lu data bytes\n",sN,strlen(buffer)+HEADER_SIZE);
    }
    // int* x;
    // int waitMachine;
    // printf("Server: Waiting for confirmation...\n");
    // waitMachine = recv(socket,x,sizeof(int),0);
    // printf("Server: Confirmation received!\n");
}

int main(){
    int serverSocket;
    int newSocket;

    struct sockaddr_in serverAddress;
    unsigned short portNumber;
    
    struct sockaddr_in incomingAddress;
    unsigned int addressSize;

    char incomingData[SIZE];
    char modData[SIZE];
    unsigned int dataLength;

    int sentBytes, recievedBytes;

    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Server: Socket initialzied.\n");

    // set up server information
    memset(&serverAddress,0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    portNumber = SERVER_PORT;
    serverAddress.sin_port = htons(portNumber);

    // bind
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Server: Error in bind");
        close(serverSocket);
        exit(1);
    }
    printf("Server: Binding successfull.\n");

    // listen
    if (listen(serverSocket, 50)<0){
        perror("Server: Error Listening");
        close(serverSocket);
        exit(1);
    }
    printf("Server: Listening...\n");
    
    addressSize = sizeof(incomingAddress);
    //server will wait unitl a client request comes in 
    newSocket = accept(serverSocket,(struct sockaddr*)&incomingAddress,&addressSize);
    if (newSocket < 0){
        perror("Server: Error accepting data");
        close(newSocket);
        exit(1);
    }
    printf("Server: Data accepted!\n");
    
    //recieve filename
    recievedBytes = recv(newSocket,incomingData, SIZE,0);
    FILE* file = fopen(incomingData, "r");

	struct Header header;
	char buffer[MAX_LINE + 1];
	int sequence = 0;
	int packets = 0;
	int totalBytes = 0;


    while (fgets(buffer, sizeof(buffer), file))
        {
            header.count = strlen(buffer) + 1;
            header.sequence = sequence++;
            WritePacket(newSocket, &header, buffer);
            printf("Server: Packet %d transmitted with %d data bytes\n", header.sequence, header.count);
            totalBytes += header.count;
            packets++;
        }

    header.count = 0;
	header.sequence = sequence++;

	WritePacket(newSocket, &header, "");
	printf("Server: End of Transmission Packet with sequence number %d transmitted with %d data bytes\n", header.sequence, strlen(""));
	printf("Server: %d data packets transmitted\n", packets);
	printf("Server: %d data bytes transmitted\n", totalBytes);
	fclose(file);
    
    printf("Server: Tranfer complete, closing sockets...\n");
    close(newSocket);
    close(serverSocket);
    

    return 0;
}