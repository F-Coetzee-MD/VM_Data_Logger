#include "./json_API.hpp"

Document json::ReadJSON(string fileLocation)
{
    Document doc;

    /* open the file */
    ifstream file(fileLocation);
  
    /* read the entire file into a string */
    string json((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
  
    /* parse the json data */
    doc.Parse(json.c_str());
  
    /* check for parse errors */
    if (doc.HasParseError()) 
    {
        cerr << "Error parsing JSON: " << doc.GetParseError() << endl;
    }
    file.close();
    return doc;
}

void json::WriteJSON(string fileLocation, string jsonString)
{
    // NOT YET NEEDED
}
