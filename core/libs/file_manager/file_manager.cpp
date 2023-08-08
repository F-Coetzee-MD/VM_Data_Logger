#include "./file_manager.hpp"

vector<string> fileManager::ListAllFiles(string directory)
{
  vector<string> fileList;
  string currentFile;

  for (const auto & entry : directory_iterator(directory))
  {
    currentFile = (string)(entry.path());
    fileList.push_back(currentFile.erase(0, directory.size()));
  }
  return fileList;
}

