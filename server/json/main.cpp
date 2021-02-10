#include <iostream>
//#include "json.h"
#include "../parser/test.h"
//#include "test.h"

using namespace std;
using namespace bee::fish::parser;
//using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
         //  << BEE_FISH_JSON_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
   
      class _Capture2 : public Match
      {
      
      public:
         string _name;
         string _value;
         
         
         _Capture2() : Match(
            Capture(
               Word("name"),
               [this](Capture& capture)
               {
                  _name = capture.value();
               }
            ) and
            Character(' ') and
            Capture(
               Word("value"),
               _value
            )
         )
         {
         }
         
         
      };
      bool _ok = true;
      
      _Capture2 capture2;
      capture2.read("name value");
     // _ok &= (capture2.result() == true);
      
      //_ok &= test("Capture class value 2", _Capture2(), true, "name value");

      Match _capture2;
      Match loadOnDemand2 = LoadOnDemand(_capture2);
      _capture2 = _Capture2();
      loadOnDemand2.read("name value");
      _ok = (loadOnDemand2.result() == true);
      
      if (_ok)
         cerr << "ok" << endl;
      else
         cerr << "FAIL" << endl;
         
      return 0;
      /*
   if (!bee::fish::parser::test())
      return 1;
      
   bee::fish::json::initialize();
 
   if (!bee::fish::json::test())
      return 1;

   cerr << "Reading from stdin." << endl;
   Match parser = JSON;
   optional<bool> ok = parser.read("{\"a\":1}", true);
   
   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
           << parser
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
  */
}