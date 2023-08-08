#pragma once

#include <bits/stdc++.h>
#include <iostream> 
#include <string.h>
#include <string>
#include <vector>

using namespace std;

string FormatHexString(string*);
vector<string> StringToStrVector(string*);
vector<float> StrVectorToFltVector(vector<string>*);
vector<float> ByteStreamStrToFltVector(string*);
deque<uint8_t> ByteStreamStrToUintDeque(string*);
vector<string> UintVectorToStrVector(vector<uint8_t>*);
deque<string> UintVectorToStrDeque(vector<uint8_t>*);
void AppendStrVectorToStrVector(vector<string>*, vector<string>*);
vector<string> UintDequeToWordStrVector(deque<uint8_t>*);

