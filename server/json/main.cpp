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
        
   class Child : public Match
   {
   public:
      Child(const Match& source) :
         Match(source)
      {
      }
   };
   
   //Match obj = Object;
  // cerr << obj;
  // obj.read("c");
   //return 0;
   /*
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
      
      Test(const Test& source) :
         Match(source._name)
      {
      }
      
      virtual Match* copy() const 
      {
         return new Test(*this);
      }
   };
      
   const Test test;
   Match reader = test;
   
   reader.read("test");
   cerr << test._value << endl;
   */
  // return 0;
  
   cerr << JSON << endl;
   
   if (!bee::fish::json::test())
      return 1;

   
   
   cerr << "Reading from stdin." << endl;
   Match parser = JSON;
   optional<bool> ok = parser.read(cin, true);
   
   if (ok == true) {
      cerr << endl
           << "ok joe" << endl
           //<< parser["field"]
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
}