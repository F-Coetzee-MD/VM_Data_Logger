#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
 
using namespace std;

class tcpServer
{

    private:

    int             listening;
    int             clientSocket;
    int             bytesReceived;
    char            host[NI_MAXHOST];   
    char            service[NI_MAXSERV];
    char            buf[4096];
    sockaddr_in     hint;
    sockaddr_in     client;
    socklen_t       clientSize;

    public:

    void InitiateAndListen();
};
