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

   wcerr << "bee.fish.json"
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
      wcout << "Testing json..." << endl << endl;
      if (!bee::fish::json::test())
         return 1;
            
      return 0;
   }
   
   /*
   if (exists("test.data"))
      remove("test.data");
   */
   wcerr << "Reading from stdin" << endl;
   Database db("test.data");
   Path start(db);
   _JSON json;
   Parser parser(json);
   //parser.read("[{\"Brett\":1},2]");
   parser.read(cin);
   
   if (parser.result() == true)
   {
    
      BString first;
     // start >> first;
     // wcerr << first << endl;
   }
   
   db.close();
   
   
   wcerr << parser.result() << endl;
  
   return 0;
   
   string line;
   while (!cin.eof())
   {
      wcout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      _JSON json;
      
      Parser parser(json);
   
      if (json.matched())
      {
         wcout << "Valid JSON" << endl;
      }
      else
      {
         wcout << "Invalid JSON" << endl;
         wcout << json << endl;
      }
      
   }
  
   wcout << "Bye" << endl;
   
   return 0;
}
