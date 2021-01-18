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
      
   JSON request;
   //bool success = true;
   //bool success = request.read("\"蓮书厙蹦㦕乥厙哦哦哦哦厙乥㦓餹鎙㥔锹厕㤹㣕㕍協퍍㓌쪐\\u0000\"");
  // success = request.read("\"\n\"");
 
   cerr << endl << "Reading from stdin." << endl;
   bool result = request.read(cin);
 
   if (result && request.result())
   {
      cerr << endl
           << request.value()
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