# pragma once

#include <chrono>
#include <format>
#include <string>
#include <string.h>
#include <iostream>
#include <ctime>  
#include <sstream> 
#include <iomanip> 

using namespace std;
using namespace std::chrono;

class systemDateTime
{
  private:
    char buffer[23];

  public:
    string GetDate();
    string GetTime();
};


