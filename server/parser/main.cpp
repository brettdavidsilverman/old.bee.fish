
#include <iostream>
#include <parser.h>
#include <../request.h>

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
           
   if (!test())
      return 1;
     
   cout << "Parser" << endl;
            
   And parser(
      new And(
         new Repeat<_Not<Char<'@'>>>(),
         new Character('@'),
         new Repeat<_Not<Char<':'> >  >()
      ),
      new Character(':'),
      new Repeat<_Not<Char<Match::eof> > >,
      new Character(Match::eof)
   );
         
   cerr << "Credentials: ";
            
   parser.read("bee@fee.bee.fish:password", true);
   cerr << endl;
   
   if (parser.success() == true) {
   
      cerr << "Credentials:\t" << parser.value() << endl;
      cerr << "Email:\t" << parser[0].value() << endl;
      cerr << "Password:\t" << parser[2].value() << endl;
   }
   else
      cerr << "Fail" << endl;
               
   
   Request req;
   
   Match& match = req;

   cerr << "Reading from stdin." << endl;
   bool success = match.read(cin);
   
   if (success == true) {
      cerr << "ok joe" << endl
           << req.method() << " "
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
   
   
}