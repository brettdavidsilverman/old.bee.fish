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
           
  // if (!test())
  //    return 1;
   
  Request request;
  bool success = true;
      cerr << endl << "Reading from stdin." << endl;
  // bool success = request.read(cin);
 
   if (success) {
         
      cerr << endl
           << "ok joe"
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
          /* << req.method() << " "
           << req.path() << " "
           << req.version() << endl;
           
      Headers& headers = req.headers();
  
      for (Header* header : headers.items())
      {
         //Header* header = it->second;
         cerr << header->name()
              << "\t"
              << header->value()
              << endl;
           
      }
   }
   */
   
   return 0;
   
}