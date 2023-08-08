#pragma once

#include <deque>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>

#include "../libs/tcp_ip/tcp_ip_client.hpp"
#include "../libs/sqlite_API/sqlite_API.hpp"
#include "../libs/data_parsers/data_parsers.hpp"
#include "../libs/file_manager/file_manager.hpp"
#include "../libs/date_and_time/date_and_time.hpp"

using namespace std;

void StopUI();
void SelectConvertToFile(string);
void ConvertToFile(string);
void UploadToMdMobile();
void TakeInput(string*);
void Status();
void* StartUI(void*);



