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

#define SERVER_PORT 53801
#define SERVER_NAME "127.0.0.1"
#define SIZE 1024
#define MAX_LINE 80

struct Header {
    short count;
    short sequence;
};

int sendFilename(int socket, char filename[SIZE]);
void gatherText(char line[MAX_LINE+1],char** set);
int countSent, countReceived, byteTotal;
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

//adds line of text to output file at the current line number
void gatherText(char line[MAX_LINE+1], char** set){
    set[lineNum++] = &line;
}

//sets each string in the array to its own line in the output file
void finalizeFile(){
    
}

int main(){
    outputFile = fopen("out.txt", "w");
    //initialize cleint data
    int port = SERVER_PORT;
    int sequence;
    
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct hostent * serverIP;

    char serverHostname[SIZE];
    unsigned short serverPort;

    char filename[SIZE];
    printf("Enter name of file to be transferred:\n");
    scanf("%s",filename);

    //Initializing TCP Socket

    if ((clientSocket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP)) < 0){
        perror("Client: Error opening socket");
        exit(1);
    }
    printf("Client: Server socket created successfully.\n");
    
    serverIP = gethostbyname(SERVER_NAME);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    memcpy( (char *)&serverAddress.sin_addr, serverIP->h_addr, serverIP->h_length);
    serverAddress.sin_port = htons(SERVER_PORT);

    if (connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))<0){
        perror("Client: Can't connect to server");
        close(clientSocket);
        exit(1);
    }
    printf("Client: Connected to server\n");

    if ((sendFilename(filename, clientSocket))<0)
    {
        printf("Client: Error sending filename.\n");
        close(clientSocket);
        exit(1);
    }
    printf("Client: Filename sent successfully\n");

    //variables for incoming data
    char *line[MAX_LINE + 1];
    struct Header headerBuff;
    struct Header *ptr = &headerBuff;
    //waits for server response
    countReceived = recv(clientSocket,ptr,sizeof(headerBuff),0);

    //loops until the 0 count is sent from server
    do{
        sequence = headerBuff.sequence;
        byteTotal = byteTotal + sizeof(headerBuff);

        countReceived = recv(clientSocket,line,sizeof(line),0);
        gatherText(line, dataSet);
        byteTotal = byteTotal + sizeof(line);
        countReceived = recv(clientSocket,ptr,sizeof(headerBuff),0);
        struct Header *ptr = &headerBuff;
    }
    while(countReceived > 0);

    printf("Client: Total number of packets recieved = %d\n", sequence);
    printf("Client: Total number of bytes recieved = %d\n", byteTotal);
    
    finalizeFile();
    printf("Client: Closing connection...\n");
    close(clientSocket);
    
    return 0;
}