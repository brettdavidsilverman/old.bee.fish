
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
            cout << "Parser" << endl;
            
            And parser =
            Match(
               "email",
               Repeat(not Character('@')) and
               Character('@') and
               Repeat(not (
                  Character(':') or
                  Character('@') )
               ) 
            )
            and
            Character(':')
            and
            Match(
               "password",
               Repeat(not
                  Character(Match::eof)
               )
            ) and
            Character(Match::eof);
            
            cout << "Credentials: ";
            
            parser.read(cin, true);
            
            if (parser.success() == true) {
              /* cout << "Size: " << parser.inputs().size() << endl;
               cout << "Email: " << parser[0].value() << endl;
               cout << "Password: " << parser[1].value() << endl;
              */
              cout << parser.value();
            }
            else
               cout << "Fail" << endl;
               
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