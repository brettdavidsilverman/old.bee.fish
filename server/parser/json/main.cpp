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
   JSON parser;

   cerr << "Reading from stdin." << endl;
   bool success = parser.read(cin);
   
   if (success) {
         
      cerr << endl
           << "ok joe" << endl
           << parser
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
   
}