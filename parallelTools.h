#define SERVER_PORT 53801
#define SERVER_NAME "127.0.0.1"
#define SIZE 1024
#define MAX_LINE 80
#define HEADER_SIZE 4

struct Header {
    short count;
    short sequence;
};

int InitializeClient(char* host, unsigned short port);
void Write(int sock, void* buffer, int length);
void WritePacket(int sock, struct Header* header, char* data);
void Read(int sock, void* buffer, int length);
void ReadPacket(int sock, struct Header* header, char* data);