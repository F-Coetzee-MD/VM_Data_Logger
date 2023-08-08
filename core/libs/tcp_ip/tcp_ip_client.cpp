#include "./tcp_ip_client.hpp"

void tcpClient::KillConnection()
{
  this->connected = false;
  shutdown(this->sock, SHUT_RDWR);
}

void tcpClient::Connect(string* IP, int PORT){
  CONNECT:
  this->sock              = 0;
  this->connectRes        = 0;
  this->hint.sin_family   = AF_INET;
  this->hint.sin_port     = htons(PORT);
  /*create a socket*/
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock == -1) goto CONNECT;
  inet_pton(AF_INET, IP->c_str(), &this->hint.sin_addr);
  this->connectRes = connect(this->sock, (sockaddr*)&this->hint, sizeof(this->hint));
  if (this->connectRes == -1) {
    if (this->connectionFailed) return;
    this->connectionWait.milliSeconds(10);
    shutdown(this->sock, SHUT_RDWR);
    goto CONNECT;
  }
}

void tcpClient::ConnectToServer(string IP, int PORT, int timeoutTime){
  TEST:
  this->connectionDone    = false;
  this->connectionFailed  = false;
  this->currentTime       = 0;
  this->timeoutCounter    = 0;
  this->timeoutThread = thread([&timeoutTime, this]{
    while (this->timeoutCounter < timeoutTime){
      this->timeoutChecker.milliSeconds(1);
      this->timeoutCounter++;
    }
    if (!this->connectionDone){
      shutdown(this->sock, SHUT_RDWR);
      this->connectionFailed = true;
    } 
  });
  this->connectThread = thread([&IP, &PORT, &timeoutTime, this] {       
    /*create a socket*/
    this->Connect(&IP, PORT);
    this->connectionDone = true;
    this->timeoutCounter = timeoutTime;
  });
  timeoutThread.join();
  connectThread.join();
  if(connectionFailed)
    goto TEST;  
  this->connected = true;
}

bool tcpClient::SendRequest(string* msg, int timeoutTime, bool response){ 
  (void)send(this->sock, msg->c_str(), msg->size(), 0);  
  if (!response) return true; 
  this->receiveFailed = false;
  this->doneRecieving = false;
  this->timeoutCounter = 0;
  this->timeoutThread = thread([&timeoutTime, &response, this]{
    while (this->timeoutCounter < timeoutTime){
      this->timeoutChecker.milliSeconds(1);
      this->timeoutCounter++;
    }
    if (!this->doneRecieving){
      if (timeoutTime != 0){
        shutdown(this->sock, SHUT_RDWR);
        this->receiveFailed = true;
      }
    } 
  });
  this->receiveThread = thread([&timeoutTime, this]{
    /*wait for response*/ 
    memset(this->buff, 0, 65535); 
    this->bytesReceived = recv(this->sock, this->buff, 65535, 0);
    this->resReceived = string(this->buff, bytesReceived);
    this->doneRecieving = true;
    this->timeoutCounter = timeoutTime;
  });
  this->timeoutThread.join();
  this->receiveThread.join(); 
  if (receiveFailed) return false;
  return true;
}

