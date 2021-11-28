#include <iostream>
#include <filesystem>

#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;

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
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }
   
/*
   JSON json;

   Parser parser(json);
   
   parser.read(cin);

   if (json->matched() || (json->result() == BeeFishMisc::nullopt))
   {
      cout << "Valid JSON" << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
   }

  */
   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      JSON json;
      
      Parser parser(json);
      
      parser.read(line);

      if (json.matched() || (json.result() == BeeFishMisc::nullopt))
      {
         cout << "Valid JSON" << endl;
      }
      else
      {
         cout << "Invalid JSON" << endl;
      }
      
   }

   cout << "Bye" << endl;
   
   return 0;
}
