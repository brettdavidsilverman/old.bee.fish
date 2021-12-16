#include <iostream>
#include <filesystem>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

#include "output.h"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishJSONOutput;

int main(int argc, const char* argv[]) {

   
   BeeFishJSONOutput::Object object(
      "key", {
         "key", "value"
      }
   );

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
   
   JSON json;

   JSONParser parser(json);
  /*
   
   JSONParser::OnValue onvalue =
      [](const BString& key, JSON& json) {
         cerr << json.value() << endl;
      };

   parser.invokeValue("fieldName", onvalue);
   
*/
   parser.read(cin);

   if (json.matched())// || (json->result() == BeeFishMisc::nullopt))
   {
      cout << "Valid JSON: " << json.value() << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
   }

   return 0;

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
