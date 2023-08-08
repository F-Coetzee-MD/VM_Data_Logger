#pragma once

#include <sqlite3.h>
#include <iostream> 
#include <string>
#include <vector>

using namespace std;

class sqliteAPI
{
  private:
    sqlite3       *myDB;
    sqlite3_stmt  *stmt;

  public:
    string dbName;
    string directory;
    bool connected;
    bool blockWriting;
    void Disconnect();
    vector<string> CheckForTables();
    long long int GetNumberOfRows(string);
    void ConvertTableToCSV(string*, string*, string*, string*, bool);
    void ConvertTableToJSON(string*, string*, string*, string*, bool);
    bool CreateNewTable(string, vector<string>*, vector<string>*);
    int OpenAndConnect(string, string);
    vector<string> ReadTableColumns(string);
    bool WriteLine(string*, vector<string>*, vector<string>*);
    sqliteAPI();
};
