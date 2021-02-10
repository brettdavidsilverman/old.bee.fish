#include <iostream>
#include "parser.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;


int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
     
      class _Capture2 : public Match
      {
      
      public:
         string _name;
         string _value;
         
         
         _Capture2() : Match(
            Capture(
               Word("name")
               ,_name
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
      
      Match capture2 = _Capture2();
     // capture2.read("name value");
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
 
   if (!bee::fish::parser::test())
      return 1;
    
   //bool success = true;
   //bool success = request.read("\"蓮书厙蹦㦕乥厙哦哦哦哦厙乥㦓餹鎙㥔锹厕㤹㣕㕍協퍍㓌쪐\\u0000\"");
  // success = request.read("\"\n\"");
 /*
   cerr << endl << "Reading from stdin." << endl;
   bool result = request.read(cin);
 
   if (result && request.result())
   {
      cerr << endl
         //  << request.value()
         //  << endl
           << "ok joe"
           << endl;
   }
   else
   {
   
      cerr << endl << "Fail" << endl;

   }
   
 */
   return 0;
   
}