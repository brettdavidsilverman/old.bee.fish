#include <iostream>
#include <clocale>
#include <locale>
#include "json.h"
#include "../parser/test.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_JSON_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (!bee::fish::json::test())
      return 1;
      
   std::locale::global(std::locale("C.UTF-8"));
   std::wcerr << "User-preferred locale setting is " << std::locale("").name().c_str() << endl;
 
   // replace the C++ global locale as well as the C locale with the user-preferred locale
   
   //std::wcerr << "User-preferred locale setting is " << std::locale("").name().c_str() << endl;

   // use the new global locale for future wide character output
   //std::wcerr.imbue(std::locale());
   
   wcerr << "Reading from stdin." << endl;
   Match parser = UTF8String and Character(Match::EndOfFile);
   //optional<bool> ok = parser.read("{\"a\":{\"hello\":true}}", true);
   optional<bool> ok = parser.read(cin);

   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
           << endl;
      //Label* label = &parser
   }
   else
      cerr << endl << "Fail" << endl;
  
}