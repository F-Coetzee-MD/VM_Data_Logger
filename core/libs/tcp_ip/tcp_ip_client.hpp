#pragma once

#include "../delays/delay.hpp"
#include "../json_API/json_API.hpp"

#include <iostream>
#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>

using namespace std;

class tcpClient
{
  private:
    int             sock;
    int             connectRes;
    int             sendRes;
    int             buffSize;
    int             currentTime;
    int             requestMade;
    int             timeoutCounter;
    char            buff[65535];   
    bool            bufferLocked;
    bool            doneRecieving;
    bool            receiveFailed;
    bool            connectionDone;
    bool            connectionFailed;
    delay           myDelay;
    delay           connectionWait;
    delay           timeoutChecker;
    string          toSend;
    thread          connectThread;
    thread          receiveThread;
    thread          timeoutThread;
    sockaddr_in     hint;
    void Connect(string*, int);
    
  public:
    int     bytesReceived;
    string  resReceived;
    bool    connected;
    /* close the socket */
    void KillConnection(); 
    /* ip, port, timeout duration */
    void ConnectToServer(string, int, int);
    /* tcp/ip frame to send, reponse timeout, wait for response */
    bool SendRequest(string*, int, bool);  
};
