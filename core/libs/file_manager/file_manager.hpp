#pragma once

#include <filesystem>
#include <iostream> 
#include <vector>
#include <string>

using namespace std;
using namespace std::filesystem;

class fileManager
{
  public:
    vector<string> ListAllFiles(string);
};
