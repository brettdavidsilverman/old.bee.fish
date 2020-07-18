
#include <iostream>
#include "parser.h"
#include "../socket/secure/asio/request.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::server;

int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_DATE
           << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_NUMBER
           << endl;
   test();
   
   Headers headers;
   headers.read("hello:world\r\nbee:bee@bee.fish\r\n");
   
   if (headers.success() == true) {
      cerr << headers << endl;
      /*
      for (auto pair : headers) {
         cerr << pair.first
              << '\t'
              << pair.second->value()
              << endl;
      }
      */
   }
   else
      cerr << "Fail" << endl;
      
   return 0;
   
   
   request req;
   
   Match& match = req;

   cerr << "Reading from stdin." << endl;
   bool success = match.read(cin);
   
   if (success == true) {
      cerr << "ok joe" << endl
           << req.method() << " "
           << req.path() << " "
           << req.version() << endl;
           
      Headers& headers = req.headers();
  
      for (auto it = headers.cbegin();
             it != headers.cend();
             ++it)
      {
         Header* header = it->second;
         cerr << header->name()
              << "\t"
              << header->value()
              << endl;
           
      }
   }
   
}