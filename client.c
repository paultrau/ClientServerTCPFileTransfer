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
#define HEADER_SIZE 4

struct Header {
    short count;
    short sequence;
};

int sendFilename(int socket, char filename[SIZE]);
void gatherText(char buffer[MAX_LINE+1],char** theData);
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

//adds line of text to output file at the current line number
void gatherText(char buffer[MAX_LINE+1], char** theData){
    theData[lineNum] = buffer;
    lineNum++;
}

//sets each string in the array to its own line in the output file
void finalizeFile(){
    printf("Client: Finalizing File...\n");
    char* test = dataSet[0];
    printf("Data:%s\n",test);

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
    
    //loops until the 0 count is sent from server
    int flag=1;
    do{
        printf("Client: Receiving...\n");
        //waits for server response
        countHeader = recv(clientSocket,ptr,sizeof(headerBuff),0);
        countData = recv(clientSocket, &buffer,sizeof(buffer),0);
        printf("Client: Data incoming: %s\n",buffer);

        sequenceBuff = ptr->sequence;
        localSequence = ntohs(sequenceBuff);
        sequence = localSequence;
        gatherText(buffer, dataSet);
        byteTotal = byteTotal + sizeof(buffer) + HEADER_SIZE;

        if (strlen(buffer)>0)
            printf("Client: Packet %d received with %lu data bytes\n",localSequence, strlen(buffer));
        else{
            flag=0;
            printf("Client: End of transmission packet with sequence number %d received with %lu data bytes\n",headerBuff.sequence, strlen(buffer));
        }
       
        printf("Client: Sending back confirmation...\n");
        int* merc = &flag;
        send(clientSocket,merc,sizeof(int*),0);
        printf("Client: Confirmation sent!\n");

    }while(flag==1);


    printf("Client: Total number of packets received = %d\n", sequence);
    printf("Client: Total number of bytes received = %d\n", byteTotal);
    
    finalizeFile();
    printf("Client: Closing connection...\n");


    close(clientSocket);
    
    return 0;
}