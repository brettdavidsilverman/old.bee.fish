#include <iostream>
#include <parser.h>
#include "version.h"
#include "../https/request.h"
#include "json/json.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::parser::json;
using namespace bee::fish::server;

int main(int argc, char* argv[]) {
   
   cerr 
        << hex
        << setw(2)
        << setfill('0')
        << right
        << 0x00
        << endl;
        
   return 0;
   
   // Set standard input and output
   // to unbufferd
   cerr << "bee.fish.server.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_PARSER_VERSION
           << endl;
   
   
   if (!test())
      return 1;
   
   String request;
   //bool success = true;
   bool success = request.read("\"蓮书厙蹦㦕乥厙哦哦哦哦厙乥㦓餹鎙㥔锹厕㤹㣕㕍協퍍㓌쪐\\u0000\"");
  // success = request.read("\"\n\"");
 
   cerr << endl << "Reading from stdin." << endl;
  // success = request.read(cin);
 
   if (success)
   {
         
      cerr << endl;
      String::write(cerr, request.value());
      cerr << endl;
      wcerr 
           << request.wvalue()
           << endl
           << "ok joe"
           << endl;
   }
   else
   {
   
      cerr << endl << "Fail" << endl;

   }
   
   
   return 0;
   
}