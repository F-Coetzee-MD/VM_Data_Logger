#include "../inc/data_logger.hpp"

#define DB ((sqliteAPI*)database)

void* StartLoggingData(void* database)
{
  delay             basicDelay;
  systemDateTime    dateTime;

  cout << dateTime.GetDate() << endl;
  cout << dateTime.GetTime() << endl;

  // check if new database needs to be made
  //  check if headers match for the current data
  //  check if the date is correct

  (void)DB->OpenAndConnect
  (
    "../databases/data/",   /* directory to database */
    "test.db"               /* name of database */
  );
 
  vector<string> fuck = DB->CheckForTable();

  for(int x = 0; x < (int)fuck.size(); x++)
  {
    cout << fuck.at(x) << endl;
  }

  vector<string> shit = DB->ReadTableColumns((string)"basic_table2");

  for(int x = 0; x < (int)shit.size(); x++)
  {
    cout << shit.at(x) << endl;
  }

  vector<string> column_test;
  column_test.push_back("col1");
  column_test.push_back("col2");
  column_test.push_back("col3");

  vector<string> type_test;
  type_test.push_back("TEXT");
  type_test.push_back("TEXT");
  type_test.push_back("TEXT");

  (void)DB->CreateNewTable((string)"basic_table2", &column_test, &type_test);
  
  vector<string> test;
  for(int x = 0; x < 3; x++)
  {
    test.push_back("20");
  }
  string com = "basic_table2";
  vector<string> col; 

  (void)DB->WriteLine(&com, &col, &test);
  cout << "All functions passed" << endl;

  //while(true)
  //{
    // check if new db should be made
    // poll data from plc
    // format the received data
    // write data to newest database
    // wait a short wile before starting again
    //cout << dateTime.GetTime() << endl;
    //basicDelay.milliSeconds(2);
  //}
  //return NULL;
  pthread_exit(NULL);
}
