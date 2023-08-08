/* IMPORT BUILT IN LIBRARIES */
#include <cstdint>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>

#include "../inc/data_logger.hpp"
#include "../libs/delays/delay.hpp"
#include "../inc/user_interface.hpp"
#include "../libs/tcp_ip/tcp_ip_client.hpp"
#include "../libs/sqlite_API/sqlite_API.hpp"

using namespace std;

struct thread_variables{
  sqliteAPI* database;
  tcpClient* plcClient;
  tcpClient* mdmClient;
  bool* threadFailed;
}TV;

void CheckThreadFailures(bool* threadFailed){
  delay wait;
  while(true){
    if(*threadFailed == true)
      break;
    wait.milliSeconds(500);
  }
}

int main(void){       
  thread_variables* pTV = &TV;
  pTV->database = new sqliteAPI;
  pTV->plcClient = new tcpClient;
  pTV->threadFailed = new bool;
  pthread_t databaseThread;
  pthread_t userInterfaceThread;

  (void)pthread_create(
    &databaseThread, 
    NULL, 
    InitiateDatabase, 
    (void*)pTV
  );

  (void)pthread_create(
    &userInterfaceThread, 
    NULL, 
    StartUI, 
    (void*)pTV
  );

  (void)pthread_detach(databaseThread);
  (void)pthread_detach(userInterfaceThread);
  CheckThreadFailures(pTV->threadFailed);

  return 0;
}


