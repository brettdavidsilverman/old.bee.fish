#include <iostream>
#include <filesystem>

#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

using namespace std;
using namespace std::filesystem;
using namespace bee::fish::parser;
using namespace bee::fish::json;
using namespace bee::fish::database;

int main(int argc, const char* argv[]) {

 
   cerr << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_JSON_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!bee::fish::json::test())
         return 1;
            
      return 0;
   }
   
   /*
   if (exists("test.data"))
      remove("test.data");
   */
   cerr << "Reading from stdin" << endl;
   
   _JSON json;
   
   Parser parser(json);
   
   parser.read(cin);

   if (json.matched() || (json.result() == nullopt))
   {
      cout << "Valid JSON" << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
      cout << json << endl;
   }
/*
   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      _JSON json;
      
      Parser parser(json);
      
      parser.read(line);

      if (json.matched() || (json.result() == nullopt))
      {
         cout << "Valid JSON" << endl;
      }
      else
      {
         cout << "Invalid JSON" << endl;
         cout << json << endl;
      }
      
   }
*/  
   cout << "Bye" << endl;
   
   return 0;
}
