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

   if (!bee::fish::json::test())
      return 1;

   cerr << "Reading from stdin." << endl;
   Match parser = JSON;
   //optional<bool> ok = parser.read("{\"a\":{\"hello\":true}}", true);
   optional<bool> ok = parser.read(cin);

   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
           << endl;
      //Label* label = &parser
   }
   else
      cerr << endl << "Fail" << endl;
  
}