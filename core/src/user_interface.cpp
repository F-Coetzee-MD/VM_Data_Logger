#include "../inc/user_interface.hpp"

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

void* variables;
bool uiActive = true;

void StopUI(){
  uiActive = false;
  *TF = true;
}

void Status(){
  string dbConnection;
  string plcConnection;
  string dbWritingStatus;
  if(DB->connected)
    dbConnection = "Connected";
  else
    dbConnection = "Not Connected";
  if(TCP->connected)
    plcConnection = "Connected";
  else
    plcConnection = "Not Connected";
  if(!DB->connected || !TCP->connected)
    dbWritingStatus = "NO";
  else{
    delay wait;
    long long int count1, count2;
    count1 = DB->GetNumberOfRows("");
    wait.seconds(2);
    count2 = DB->GetNumberOfRows("");
    if (count2 > count1)
      dbWritingStatus = "YES";
    else dbWritingStatus = "NO";
  }
  cout << "\n================================================" << endl;
  cout << "Database Connection Status:      " + dbConnection << endl; 
  cout << "Controller Connection Status:    " + plcConnection << endl;
  cout << "Is Data Being Logged:            " + dbWritingStatus << endl;
  cout << "================================================\n" << endl;
  cout << "\nInsert any key to continue: " << endl;
  string waiter;
  cin >> waiter;
}

void ShowErrorMessage(){
  string errDone;
  system("clear");
  cout << "Unknown Command" << endl;
  cout << "Insert any key to continue..." << endl;
  cin >> errDone;
}

void ConvertToFile(string fileType){
  if(!DB->connected) return;
  string spec;
  string outDir = "../databases/" + fileType + "/"; 
  vector <string> tableNames = DB->CheckForTables();
  if(fileType == "json")
    DB->ConvertTableToJSON(&DB->directory, &tableNames.at(0), &outDir, &spec, true);
  else if(fileType == "csv")
    DB->ConvertTableToCSV(&DB->directory, &tableNames.at(0), &outDir, &spec, true);
}

void SelectConvertToFile(string fileType){
  system("clear");
  string fileName, fileDate;
  cout << "File to convert to " + fileType + ": ";
  cin >> fileName;
  fileDate = fileName;
  if(fileName.size() == 26)
    fileDate.erase(fileDate.begin() + 10, fileDate.end());                   
  else return;
  string command = "sqlite3 -header -csv ../databases/data/" + fileName;
  if(fileType == "json")
    command += " \"SELECT * FROM '" + fileDate + "';\" > " + "../databases/json/" + fileName + ".json"; 
  else if(fileType == "csv")
  command += " \"SELECT * FROM '" + fileDate + "';\" > " + "../databases/csv/" + fileName + ".csv"; 
  system(command.c_str());
}

void UploadToMdMobile(){

}

void TakeInput(string* command){
  if(*command == (string)"") {/* NOP */}
  else if(*command == (string)"exit")     StopUI();                               /* stops ui and exits program */
  else if(*command == (string)"quit")     StopUI();                               /* stops ui and exits program */
  else if(*command == (string)"json")     ConvertToFile((string)"json");          /* convert current connected DB to json */
  else if(*command == (string)"csv")      ConvertToFile((string)"csv");           /* convert current connected DB to csv */
  else if(*command == (string)"sjson")    SelectConvertToFile((string)"json");    /* select any DB file to convert to json */
  else if(*command == (string)"scsv")     SelectConvertToFile((string)"csv");     /* select any DB file to convert to csv */
  else if(*command == (string)"status")   Status();                               /* check status of current running application */
  else ShowErrorMessage();
}

void* StartUI(void* pVariables){
  variables = pVariables;
  while(uiActive){
    system("clear");
    string command;
    cout << "==> ";
    cin >> command;
    TakeInput(&command);
  }
  pthread_exit(NULL);
}

