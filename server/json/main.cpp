#include <iostream>
#include "json.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.server.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_JSON_VERSION
        << endl;
   _Object::Field field;
   field.read("\"a\":1");
   cerr << field;
   
   return 0;
   
   if (!bee::fish::json::test())
      return 1;

   cerr << "Reading from stdin." << endl;
   Match parser = _Object();
   optional<bool> ok = parser.read(cin, true);
   
   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
          // << parser["\"field\""]
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
}