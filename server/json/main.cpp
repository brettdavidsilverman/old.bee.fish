#include <iostream>
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


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
   
   class JSON2 : public _JSON
   {
   public:
      JSON2()
      {
      }
      
      virtual bool match(
         const bee::fish::b_string::Character&
            character
      )
      {
         bool matched = _JSON::match(character);
         
         if (matched)
         {
            if (character == '{')
            {
               cerr << "Open" << endl;
            }
            else if (character == '}')
            {
               cerr << "Close" << endl;
            }
         }
         
         return matched;
      }
      
      virtual Match* copy() const
      {
         return new JSON2();
      }
   };
   
   
   cerr << "Reading from stdin" << endl;
   JSON2 json;
   Parser parser(json);
   parser.read(cin);
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
