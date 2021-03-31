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

int InitializeClient(char* host, unsigned short port)
{
    /*Create a socket using TCP*/
    int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        perror("socket() failed");

    /*Construct the server address structure*/
    struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(host);
	servAddr.sin_port = htons(port);

    /*Establish connection to echo server*/
	if (connect(sockfd, (struct scokaddr*) &servAddr, sizeof (servAddr)) < 0)
		perror("connect() failed");

    return sockfd;
}

void Write(int sock, void* buffer, int length)
{
    int sBytes, sv; 
    for (sBytes=0; sBytes<length; sBytes += sv)
    {
        if  ((sv = send(sock, buffer+sBytes, length-sBytes, 0)) <  0)
            perror("send() failed");
    }
}

void WritePacket(int sock, struct Header* header, char* data)
{
    struct Header encodedHeader;
    encodedHeader.count = htons(header->count);
    encodedHeader.sequence = htons(header->sequence);
    Write(sock, (void*)&encodedHeader, sizeof(header));
    Write(sock, (void*)data, header->count);
}

void Read(int sock, void* buffer, int length)
{
    recv(sock, buffer, length, 0);
}

void ReadPacket(int sock, struct Header* header, char* data)
{
    Read(sock, (void*)header, sizeof(header));
    header->count = ntohs(header->count);
    header->sequence = ntohs(header->sequence);
    if (header->count)
        Read(sock, (void*)data, header->count);
}