#include "./data_parsers.hpp"

vector<string> StringToStrVector(string* inputStr)
{
  string currentStr;
  vector<string> out;
  for(int x = 0; x < (int)inputStr->size(); x++){
    if(inputStr->at(x) != *" ")
      currentStr += inputStr->at(x);
    else{
      out.push_back(currentStr);
      currentStr = "";
    }
  }
  if (currentStr != "")
    out.push_back(currentStr);
  return out;
}

vector<string> UintDequeToWordStrVector(deque<uint8_t>* byteDeque)
{
  vector<string> wordVector;
  if(byteDeque->size()%2 != 0)
    return wordVector;
  for(int x = 0; x < (int)byteDeque->size(); x+=2)
  {
    uint8_t msB = byteDeque->at(x);
    uint8_t lsB = byteDeque->at(x+1);
    uint16_t word = (msB << 8) + lsB;
    wordVector.push_back(to_string((float)word/50));
  }
  return wordVector;
}

string FormatHexString(string* stringFrame)
{
  string word, frame;
  stringstream ss(*stringFrame); 
  while (ss >> word) 
    frame += (uint8_t)stoi(word, 0, 16);
  return frame;
}

vector<string> UintVectorToStrVector(vector<uint8_t>* inputVector)
{
  vector<string> stringVector;
  for(int x = 0; x < (int)inputVector->size(); x++)
    stringVector.push_back(to_string(inputVector->at(x)));
  return stringVector;
}

deque<string> UintVectorToStrDeque(vector<uint8_t>* inputVector)
{
  deque<string> dequeVector;
  for(int x = 0; x < (int)inputVector->size(); x++)
    dequeVector.push_back(to_string(inputVector->at(x)));
  return dequeVector;
}

vector<float> StrVectorToFltVector(vector<string>* inputVector)
{
  char* p;
  vector<float> floatVector;
  for(int x = 0; x < (int)inputVector->size(); x++)
    floatVector.push_back((float)strtoul(inputVector->at(x).c_str(), &p, 16)/1000);
  return floatVector;
}

vector<float> ByteStreamStrToFltVector(string* inputString)
{
  vector<float> floatVector;
  for(int x = 0; x < (int)inputString->size(); x++)
    floatVector.push_back((float)inputString->at(x));
  return floatVector;
}

deque<uint8_t> ByteStreamStrToUintDeque(string* inputString)
{
  deque<uint8_t> uintVector;
  for(int x = 0; x < (int)inputString->size(); x++)
    uintVector.push_back((uint8_t)inputString->at(x));
  return uintVector;
}

void AppendStrVectorToStrVector(vector<string>* uintVector, vector<string>* newStrVector)
{
  for(int x = 0; x < (int)uintVector->size(); x++)
    newStrVector->push_back(uintVector->at(x));
}
