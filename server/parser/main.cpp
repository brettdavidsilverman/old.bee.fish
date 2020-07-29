
#include <iostream>
#include <parser.h>
#include <../request.h>

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::server;

int main(int argc, char* argv[]) {
   
   // Set standard input and output
   // to unbufferd
   
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
           
   if (!test())
      return 1;
 
   //Request req;
   JSON req;
   
   Match& match = req;

   cerr << "Reading from stdin." << endl;
   bool success = match.read(cin);
   
   if (success) {
      cerr << endl
           << "ok joe" << endl
           << req.value()
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
   
}