#include <iostream>
#include "parser.h"
//#include "request.h"

using namespace std;
using namespace bee::fish::parser;
//using namespace bee::fish::server;

int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.parser "
        << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_DATE
        << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_NUMBER
        << endl;
        
   test_basics();
   
/*
   bee::fish::server::GenericHeader request;
   
   Match& match = request;

   cerr << "Reading from stdin." << endl;
   bool success = match.read("hello: joe\r\n");
   
   cerr << endl;
   if (success)
      cerr << "ok joe" << endl
           << request.name() << ":\t"
           << request.value() << " "
           << endl;
    
   
   
   if (success == true)
      cerr << "ok joe" << endl
           << request.method() << " "
           << request.path() << " "
           << request.version() << endl;
   
   
        
   cerr << "Hit enter:";
   cin.get();
   
   */
}