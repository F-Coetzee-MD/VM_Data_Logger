#include "./sqlite_API.hpp"

sqliteAPI::sqliteAPI(){
  this->connected = false;
  this->blockWriting = false;
}

void sqliteAPI::Disconnect(){
  this->connected = false;
  (void)sqlite3_close(this->myDB);
}

bool sqliteAPI::CreateNewTable(string tableName, vector<string>* colNames, vector<string>* colTypes){
  if(!this->connected) 
    return false;

  if(colTypes->size() == 0) 
    goto SKIP;
  
  if(colNames->size() != colTypes->size()) 
    return false;

  SKIP:

  string sqlStatement = "CREATE TABLE '" + tableName + "' (";

  if(colTypes->size() > 0){
    for(int x = 0; x < (int)colNames->size()-1; x++)
      sqlStatement += "'" + colNames->at(x) + "' " + colTypes->at(x) + ",";
    sqlStatement += "'" + colNames->at(colNames->size()-1) + "' " + colTypes->at(colTypes->size()-1) + ");";
  }
  else{
    for(int x = 0; x < (int)colNames->size()-1; x++)
      sqlStatement += "'" + colNames->at(x) + "',";
    sqlStatement += "'" + colNames->at(colNames->size()-1) + "');";
  }

  if(sqlite3_prepare_v2(this->myDB, sqlStatement.c_str(), -1, &this->stmt, NULL) != 0)
    return false;

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return true;
}

vector<string> sqliteAPI::CheckForTables(){ 
  vector<string> tableNames;
  if(!this->connected) 
    return tableNames; 

  string sqlStatement = "SELECT tbl_name FROM sqlite_master WHERE type = 'table'";
  if(sqlite3_prepare_v2(this->myDB, sqlStatement.c_str(), -1, &this->stmt, NULL))
    return tableNames;

  while(sqlite3_step(stmt) == SQLITE_ROW)
    tableNames.push_back((string)(const char*)sqlite3_column_text(this->stmt, 0));

  sqlite3_finalize(stmt);
  return tableNames;
}

int sqliteAPI::OpenAndConnect(string directory, string name){  
  this->dbName = name; 
  this->directory = directory;
  int res = sqlite3_open(
    (directory + name).c_str(),       /* Database filename (UTF-8) */
    &this->myDB
  ); 

  if (res != 0) return res;

  this->connected = true;
  return res;
}

vector<string> sqliteAPI::ReadTableColumns(string tableName){
  vector<string> column_names;
  if(!this->connected) 
    return column_names;
  string sqlStatement = "PRAGMA table_info ('" + tableName + "')";
  if(sqlite3_prepare_v2(this->myDB, sqlStatement.c_str(), -1, &this->stmt, NULL))
    return column_names;
  while(sqlite3_step(stmt) == SQLITE_ROW) 
    column_names.push_back((string)(const char*)sqlite3_column_text(this->stmt, 1));
  sqlite3_finalize(stmt);
  return column_names;
}

long long int sqliteAPI::GetNumberOfRows(string tableName){
  sqlite3_int64 changeCount;
  if(!this->connected) return 0;
  changeCount = sqlite3_last_insert_rowid(this->myDB);
  return (long long int)changeCount;
}

bool sqliteAPI::WriteLine(string* tableName, vector<string>* colNames, vector<string>* colValues){
  if(!this->connected) 
    return false;
  if(this->blockWriting)
    return false;
  if(colNames->size() == 0) 
    goto SKIP;
  if(colNames->size() != colValues->size()) 
    return false;

  SKIP:

  string sqlStatement = "INSERT INTO '" + *tableName + "'";
  if(colNames->size() > 0){
    sqlStatement += " (";
    for(int x = 0; x < (int)colNames->size()-1; x++)
      sqlStatement += '"' + colNames->at(x) + '"' +",";
    sqlStatement += '"' + colNames->at(colNames->size()-1) + '"' +")";
  }
  sqlStatement += " VALUES(";
  for(int x = 0; x < (int)colValues->size()-1; x++)
    sqlStatement += '"' + colValues->at(x) + '"' +  ",";
  sqlStatement += '"' + colValues->at(colValues->size()-1) + '"' +  ");";
  if(sqlite3_prepare_v2(this->myDB, sqlStatement.c_str(), -1, &this->stmt, NULL) != 0)
    return false;
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return true;
}

void sqliteAPI::ConvertTableToJSON(string* dbDir, string* tableName, string* outDirectory, string* spec, bool entireTable){ 
  if(this->connected){
    this->blockWriting = true;
    string command = "sqlite3 -json " + *dbDir + this->dbName;

    if(entireTable)
      command += " \"SELECT * FROM '" + *tableName + "'\" > " + *outDirectory + this->dbName + ".json"; 
    else command += " \"" + *spec + "\" > " + *outDirectory + this->dbName + ".json"; 
    
    system(command.c_str());
    this->blockWriting = false;
  }
}

void sqliteAPI::ConvertTableToCSV(string* dbDir, string* tableName, string* outDirectory, string* spec, bool entireTable){
  if(this->connected){
    this->blockWriting = true;
    string command = "sqlite3 -header -csv " + *dbDir + this->dbName;

    if(entireTable)
      command += " \"SELECT * FROM '" + *tableName + "';\" > " + *outDirectory + this->dbName + ".csv"; 
    else command += " \"" + *spec + "\" > " + *outDirectory + this->dbName + ".csv"; 
    
    system(command.c_str());
    this->blockWriting = false;
  }
}

//sqlite3 -header -csv c:/sqlite/chinook.db "select * from tracks;" > tracks.csv
