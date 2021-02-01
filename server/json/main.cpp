#include <iostream>
#include "json.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   // Set standard input and output
   // to unbufferd
   
   cerr << "bee.fish.server.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_JSON_VERSION
        << endl;
           
   if (!bee::fish::json::test())
      return 1;

   Match parser = JSON;
   
   cerr << "Reading from stdin." << endl;
   bool ok = parser.read(cin);
   
   if (ok && (parser.result() == true)) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else {
      cerr << endl << "Fail" << endl;
      cerr << parser << endl;
   }
 
}