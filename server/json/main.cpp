#include <iostream>
#include <clocale>
#include <locale>
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
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

   if (!bee::fish::parser::test())
      return 1;
      
   if (!bee::fish::json::test())
      return 2;

   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
         
      _JSON parser;
      
      parser.read(line);
   
      if (parser.matched())
      {
         cout << parser << endl;
      }
      else
         cout << "Invalid JSON" << endl;
   }
  
   cout << "Bye" << endl;
   
   return 0;
}