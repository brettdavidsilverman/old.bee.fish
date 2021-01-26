#include <iostream>
#include "json.h"

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
           
   And parser = JSON;

   cerr << "Reading from stdin." << endl;
   bool ok = parser.read(cin, true);
   
   if (ok) {
      cerr << endl
           << "ok joe: " << json_count << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
}