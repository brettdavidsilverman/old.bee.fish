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
   class Test : public Match
   {
   public:
      string _value;
      
      Test() : Match(
         Capture(
            Word("test"),
            [this](Capture& item)
            {
               _value = item.value();
            }
         )
      )
      {
      }
   };
      
   Test test;
   test.read("test");
   cerr << test._value << endl;
      
   return 0;
   
   if (!bee::fish::json::test())
      return 1;

   Match parser = JSON;
   
   cerr << "Reading from stdin." << endl;
   bool ok = parser.read(cin, true);
   
   if (ok && (parser.result() == true)) {
      cerr << endl
           << "ok joe" << endl
           //<< parser["field"]
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
}