#include <iostream>
#include "json.h"
#include "../parser/test.h"
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

   bee::fish::json::initialize();
 
   if (!bee::fish::json::test())
      return 1;

   cerr << "Reading from stdin." << endl;
   _Object parser = _Object();
   optional<bool> ok = parser.read("{\"a\":\"hello\"}", true);
   
   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
           << parser["\"a\""]
           << endl;
      //Label* label = &parser
   }
   else
      cerr << endl << "Fail" << endl;
  
}