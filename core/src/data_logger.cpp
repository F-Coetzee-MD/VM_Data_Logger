#include "../inc/data_logger.hpp"

#define DB    ((thread_variables*)variables)->database
#define TCP   ((thread_variables*)variables)->plcClient
#define MDM   ((thread_variables*)variables)->mdmClient
#define TF    ((thread_variables*)variables)->threadFailed

struct thread_variables{
  sqliteAPI* database;
  tcpClient* plcClient;
  tcpClient* mdmClient;
  bool* threadFailed;
};

void InitiateDataStructure();

vector<string> colNames;
vector<string> colTypes;

string pollData = "0x0 0x1 0x0 0x0 0x0 0x6 0x0 0x3 0x0 0x0 0x0 0x58";
string POLL_DATA = FormatHexString(&pollData);

void* InitiateDatabase(void* variables){
  INITIATE:
  InitiateDataStructure();
  delay           wait;
  systemDateTime  dateTime;
  fileManager     fileSystem;
  string currentDate = dateTime.GetDate();
  vector<string> fileNames = fileSystem.ListAllFiles((string)"../databases/data/");
  /* filter for files that end on .db and that are the right length */
  string tempFileName;
  for(int x = 0; x < (int)fileNames.size(); x++){
    if(x < 0) x = 0; 
    /* if current file's name is not the right lenght*/
    if(fileNames.at(x).size() != 26){
      fileNames.erase(fileNames.begin() + x);
      x--;
      continue;
    }
    /* get the file extension of the file */
    tempFileName = fileNames.at(x);
    string extension = tempFileName.erase(0, tempFileName.size()-3);
    /* get the date value in the file */
    tempFileName = fileNames.at(x);
    string dateCreated = tempFileName.erase(10);
    /* if current file is not the right file type */
    if(extension != ".db"){
      fileNames.erase(fileNames.begin() + x);
      x--;
      continue;
    }
    /* remove file if it's date value is not the current date */
    if(dateCreated != currentDate){
      fileNames.erase(fileNames.begin() + x);
      x--;
      continue;
    }
  }
  /* if there is more than one db created today */
  string dbName, newest;
  if(fileNames.size() > 1){
    /* find the newest db created today */
    newest = fileNames.at(0);
    for(int x = 1; x < (int)fileNames.size(); x++){
      if(newest < fileNames.at(x)) 
        newest = fileNames.at(x);
    }
    /* check if the columns of the db is correct */
    DB->OpenAndConnect((string)"../databases/data/", newest);
    vector<string> fileColumns = DB->ReadTableColumns(currentDate);
    if((fileColumns != colNames)||(DB->CheckForTables().size() != 1)){
      /* create new database and table*/
      DB->Disconnect();
      dbName = currentDate + "_" + dateTime.GetTime();
      (void)DB->OpenAndConnect((string)"../databases/data/", dbName + ".db");
      (void)DB->CreateNewTable(currentDate, &colNames, &colTypes);
    }
  }
  /* if there is only one db for today that exists */
  else if(fileNames.size() == 1){
    newest = fileNames.at(0);
    /* check if the columns of the db is correct */
    (void)DB->OpenAndConnect((string)"../databases/data/", newest);
    vector<string> fileColumns = DB->ReadTableColumns(currentDate);
    if((fileColumns != colNames)||(DB->CheckForTables().size() != 1)){
      /* create new database and table*/
      DB->Disconnect();
      dbName = currentDate + "_" + dateTime.GetTime();
      (void)DB->OpenAndConnect((string)"../databases/data/", dbName + ".db");
      (void)DB->CreateNewTable(currentDate, &colNames, &colTypes);
    }
  }
  /* if there are no db created for today yet */
  else{
    /* create new database and table*/
    dbName = currentDate + "_" + dateTime.GetTime();
    (void)DB->OpenAndConnect((string)"../databases/data/", dbName + ".db");
    (void)DB->CreateNewTable(currentDate, &colNames, &colTypes);
  }
  if(!DB->connected){
    wait.milliSeconds(1000);
    goto INITIATE;
  }
  TCP->ConnectToServer(
    "192.168.1.10",       /* server IP address */ 
    420,                  /* server port number */
    3000                  /* connectio time allowed */
  );

  while(true){    
    int badPollCount = 0;
    POLL:
    if(currentDate != dateTime.GetDate())
      goto INITIATE;
    if(DB->blockWriting){
      wait.milliSeconds(500);
      goto POLL;
    } 
    /* poll data from plc */
    (void)TCP->SendRequest(
      &POLL_DATA,           /* frame to send */   
      3000,                 /* timeout time in ms */
      true                  /* enable timeout */
    );
    /* capture data received from plc */
    string* strValues = &TCP->resReceived;
    deque<uint8_t> uintValues = ByteStreamStrToUintDeque(strValues);
    /* format data captured from plc */
    vector<string> wordValues; 
    vector<string> formattedData;
    formattedData.push_back(currentDate);
    formattedData.push_back(dateTime.GetTime());
    for(int x = 0; x < 9; x++) uintValues.pop_front();
    wordValues = UintDequeToWordStrVector(&uintValues);
    AppendStrVectorToStrVector(&wordValues, &formattedData);
    /* check if data from plc is valid */
    if(formattedData.size() != colNames.size()){ 
      badPollCount++;
      if(badPollCount < 10) goto POLL;
      else break;
    };
    /* write data to databse */
    DB->WriteLine(
      &currentDate,
      &colNames,
      &formattedData
    ); 
    /* reset badPollCount and add a delay */
    badPollCount = 0;
    wait.milliSeconds(500);
  }
  *TF = true;
  pthread_exit(NULL);
}

void InitiateDataStructure()
{
  colNames.push_back("Date");                                       colTypes.push_back("TEXT");
  colNames.push_back("Time");                                       colTypes.push_back("TEXT");
  colNames.push_back("Cutter Head 1 Direction");                    colTypes.push_back("FLOAT");
  colNames.push_back("Cutter Head 2 Direction");                    colTypes.push_back("FLOAT");
  colNames.push_back("Cutter Head 3 Direction");                    colTypes.push_back("FLOAT");
  colNames.push_back("Cutter Head 4 Direction");                    colTypes.push_back("FLOAT");
  colNames.push_back("Plunger IN");                                 colTypes.push_back("FLOAT");
  colNames.push_back("Thrust Cylinder 1 Direction");                colTypes.push_back("FLOAT");                                                             
  colNames.push_back("Thrust Cylinder 2 Direction");                colTypes.push_back("FLOAT");                                                            
  colNames.push_back("Thrust Cylinder 3 Direction");                colTypes.push_back("FLOAT");                                                             
  colNames.push_back("Thrust Cylinder 4 Direction");                colTypes.push_back("FLOAT");                                                                                     
  colNames.push_back("Thrust Cylinder Retracted");                  colTypes.push_back("FLOAT");                                                                              
  colNames.push_back("Thrust Cylinder Extended");                   colTypes.push_back("FLOAT");                                 
  colNames.push_back("Cutters Stalled");                            colTypes.push_back("FLOAT");                                                                                              
  colNames.push_back("Cutters Fail To Start");                      colTypes.push_back("FLOAT");                                                                                         
  colNames.push_back("Cutting Head Temp Warning");                  colTypes.push_back("FLOAT");                                                                              
  colNames.push_back("Cutting Head Temp Alarm");                    colTypes.push_back("FLOAT");                                                                     
  colNames.push_back("Filter 75");                                  colTypes.push_back("FLOAT");                                                                                                                               
  colNames.push_back("Filter 100");                                 colTypes.push_back("FLOAT");                                                                                                               
  colNames.push_back("Motor Temp Left");                            colTypes.push_back("FLOAT");                                                                                                                    
  colNames.push_back("Motor Temp Right");                           colTypes.push_back("FLOAT");                                                                                                                 
  colNames.push_back("Stabilizers Pressure Bottom Left");           colTypes.push_back("FLOAT");                                                                               
  colNames.push_back("Stabilizers Pressure Bottom Right");          colTypes.push_back("FLOAT");                                                                            
  colNames.push_back("Stabilizers Pressure Top Left");              colTypes.push_back("FLOAT");                                                                             
  colNames.push_back("Stabilizers Pressure Top Right");             colTypes.push_back("FLOAT");                                                                                                       
  colNames.push_back("Thrust Position Bottom Left");                colTypes.push_back("FLOAT");                                                                                                     
  colNames.push_back("Thrust Position Top Left");                   colTypes.push_back("FLOAT");                                                                                                      
  colNames.push_back("Thrust Position Bottom Right");               colTypes.push_back("FLOAT");                                    
  colNames.push_back("Thrust Position Top Right");                  colTypes.push_back("FLOAT");                                                                                                                           
  colNames.push_back("Thrust Pressure Bottom Left");                colTypes.push_back("FLOAT");                                                                                             
  colNames.push_back("Thrust Pressure Top Left");                   colTypes.push_back("FLOAT");                                                                                             
  colNames.push_back("Thrust Pressure Bottom Right");               colTypes.push_back("FLOAT");                                                                                            
  colNames.push_back("Thrust Pressure Top Right");                  colTypes.push_back("FLOAT");                           
  colNames.push_back("Gripper Cylinder Pressure Bottom Left");      colTypes.push_back("FLOAT");                              
  colNames.push_back("Gripper Cylinder Pressure Top Left");         colTypes.push_back("FLOAT");                              
  colNames.push_back("Gripper Cylinder Pressure Bottom Right");     colTypes.push_back("FLOAT");                             
  colNames.push_back("Gripper Cylinder Pressure Top Right");        colTypes.push_back("FLOAT");   
  colNames.push_back("Thrustbody X");                               colTypes.push_back("FLOAT");                                                                                        
  colNames.push_back("Thrustbody Y");                               colTypes.push_back("FLOAT");                                                                                        
  colNames.push_back("Cuttingbody X");                              colTypes.push_back("FLOAT");                                                                                      
  colNames.push_back("Cuttingbody Y");                              colTypes.push_back("FLOAT");                                                                                                                
  colNames.push_back("Motor RPM Bottom Right");                     colTypes.push_back("FLOAT");                                                                                                                
  colNames.push_back("Motor RPM Bottom Left");                      colTypes.push_back("FLOAT");                                                             
  colNames.push_back("Motor RPM Top Right");                        colTypes.push_back("FLOAT");                                                             
  colNames.push_back("Motor RPM Top Left");                         colTypes.push_back("FLOAT");                                                                                      
  colNames.push_back("Thrust Cylinder 1 Flow Valve Position");      colTypes.push_back("FLOAT");                                                                       
  colNames.push_back("Thrust Cylinder 2 Flow Valve Position");      colTypes.push_back("FLOAT");                                                                       
  colNames.push_back("Thrust Cylinder 3 Flow Valve Position");      colTypes.push_back("FLOAT");                                                                       
  colNames.push_back("Thrust Cylinder 4 Flow Valve Position");      colTypes.push_back("FLOAT");                                 
  colNames.push_back("Plunger Pressure");                           colTypes.push_back("FLOAT");                                                                                            
  colNames.push_back("Cooling Temp After");                         colTypes.push_back("FLOAT");                                                                                 
  colNames.push_back("Cooling Temp Before");                        colTypes.push_back("FLOAT");                                                                              
  colNames.push_back("Cutter Supply Line Pressure");                colTypes.push_back("FLOAT");                                                                                                        
  colNames.push_back("Valve Pressure S");                           colTypes.push_back("FLOAT");                                                                                                         
  colNames.push_back("Valve Pressure T");                           colTypes.push_back("FLOAT");                                                                                                                              
  colNames.push_back("Filter Press Before");                        colTypes.push_back("FLOAT");                                                                                   
  colNames.push_back("Filter Press After");                         colTypes.push_back("FLOAT");                                                                                      
  colNames.push_back("Vacuum Blower 1 Temp Warning");               colTypes.push_back("FLOAT");                                                                   
  colNames.push_back("Vacuum Blower 2 Temp Warning");               colTypes.push_back("FLOAT");                                     
  colNames.push_back("Vacuum Blower 1 Temperature");                colTypes.push_back("FLOAT");                                     
  colNames.push_back("Vacuum Blower 2 Temperature");                colTypes.push_back("FLOAT");                                      
  colNames.push_back("Vacuum DeltaP Warning");                      colTypes.push_back("FLOAT");                                     
  colNames.push_back("Vacuum DeltaP Trip");                         colTypes.push_back("FLOAT");                                     
  colNames.push_back("Vacuum Compressed Air Error");                colTypes.push_back("FLOAT");                                    
  colNames.push_back("Vacuum Main Motor 1 Fail To Start");          colTypes.push_back("FLOAT");                                  
  colNames.push_back("Vacuum Main Motor 2 Fail To Start ");         colTypes.push_back("FLOAT");                                   
  colNames.push_back("Vacuum Main Motor 1 Healthy");                colTypes.push_back("FLOAT");                                  
  colNames.push_back("Vacuum Main Motor 2 Healthy");                colTypes.push_back("FLOAT");                                 
  colNames.push_back("Vacuum System Running");                      colTypes.push_back("FLOAT");                                       
  colNames.push_back("Vacuum Collector Tank Level");                colTypes.push_back("FLOAT");                                  
  colNames.push_back("Vacuum Start Limmeting Counter");             colTypes.push_back("FLOAT");                                                                 
  colNames.push_back("Vacuum DeltaP");                              colTypes.push_back("FLOAT");                                                                                                                         
  colNames.push_back("Vacuum Exhaust Temp 1");                      colTypes.push_back("FLOAT");                                                                                                         
  colNames.push_back("Vacuum Exhaust Temp 2");                      colTypes.push_back("FLOAT");                                                                                           
  colNames.push_back("Vacuum VacuumPressSensor");                   colTypes.push_back("FLOAT");                                                                                    
  colNames.push_back("Vacuum Suction Pressure Sensor");             colTypes.push_back("FLOAT");                                                                      
  colNames.push_back("Vacuum Pulsing Valve Pressure");              colTypes.push_back("FLOAT");                                                                       
  colNames.push_back("Cutting Head Cooling Motor Pressure");        colTypes.push_back("FLOAT");                                                                         
  colNames.push_back("Cutting Head Pressure");                      colTypes.push_back("FLOAT");
  colNames.push_back("Feed Rate Average");                          colTypes.push_back("FLOAT");
  colNames.push_back("Thrust Cylinder Tons Top Right");             colTypes.push_back("FLOAT");
  colNames.push_back("Thrust Cylinder Tons Top Left");              colTypes.push_back("FLOAT");
  colNames.push_back("Thrust Cylinder Tons Bottom Right");          colTypes.push_back("FLOAT");
  colNames.push_back("Thrust Cylinder Tons Bottom Left");           colTypes.push_back("FLOAT");
  colNames.push_back("Stabilizer Cylinder Tons Top Right");         colTypes.push_back("FLOAT");
  colNames.push_back("Stabilizer Cylinder Tons Top Left");          colTypes.push_back("FLOAT");
  colNames.push_back("Stabilizer Cylinder Tons Bottom Right");      colTypes.push_back("FLOAT");
  colNames.push_back("Stabilizer Cylinder Tons Bottom Left");       colTypes.push_back("FLOAT");
  colNames.push_back("Average Thrust Cylinder Tons");               colTypes.push_back("FLOAT");
  colNames.push_back("Cutting Head Torque");                        colTypes.push_back("FLOAT");
}

