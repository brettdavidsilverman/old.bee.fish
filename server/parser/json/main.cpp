#include <iostream>
#include <parser.h>
#include "json.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::parser::json;


int main(int argc, char* argv[]) {
   
   // Set standard input and output
   // to unbufferd
   
   cerr << "bee.fish.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_BUILD_DATE
           << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_BUILD_NUMBER
           << endl;
           
  // Request req;
   JSON* parser = new JSON();

   cerr << "Reading from stdin." << endl;
   bool ok = parser->read(cin, true);
   
   if (ok) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
   delete parser;
}