#pragma once

#include <iostream> 
#include <fstream>
#include <string>

#include "./rapidJSON/document.h"
#include "./rapidJSON/filereadstream.h"

using namespace std;
using namespace rapidjson;

class json
{
    public:

    Document ReadJSON(string);
    void WriteJSON(string, string);
};