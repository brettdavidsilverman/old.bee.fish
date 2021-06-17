#include <iostream>
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

using namespace std;
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
   
   
   cerr << "Reading from stdin" << endl;
   Database db("test.data");
   Path start(db);
   cerr << sizeof(start) * 8 / 64 << endl;
   return 0;
   _JSON json(&start);
   Parser parser(json);
   parser.read("{\"Brett\":{ \"Shane\":2 }}");
   
   if (parser.result() == true)
   {
    
      BString first;
      start >> first;
      cerr << first << endl;
      BString second;
      start >> second;
      cerr << second << endl;
      start = Branch::Root;
      cerr << start;
   }
   
   db.close();
   
   remove("test.data");
   
   cerr << parser.result() << endl;
  
   return 0;
   
   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      _JSON json;
      
      Parser parser(json);
   
      if (json.matched())
      {
         cout << "Valid JSON" << endl;
      }
      else
      {
         cout << "Invalid JSON" << endl;
         cout << json << endl;
      }
      
   }
  
   cout << "Bye" << endl;
   
   return 0;
}
