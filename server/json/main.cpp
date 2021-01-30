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
           
   
/*
      const Match Test = Set(
         Character('{'),
         LoadOnDemand(JSON),
         Character(','),
         Character('}')
      );
     */
  // const Match JSON; =// ~BlankSpace and Number;
    //   Repeat(~BlankSpace and Number);
   //     Repeat(JSON and Character('\n'));
    //  Character(Match::EndOfFile);
      
   Match parser = JSON;
   
   cerr << "Reading from stdin." << endl;
   bool ok = parser.read(cin, true);
   
   if (ok && (parser.result() == true)) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
}