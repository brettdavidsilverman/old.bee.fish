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

   std::locale::global(std::locale("C.UTF-8"));
   

   if (!bee::fish::json::test())
      return 1;
   
   /*
   std::wcerr << "User-preferred locale setting is " << std::locale("").name().c_str() << endl;

   wcerr << "Reading from stdin." << endl;
   _StringCharacter parser = _StringCharacter();
   
   //optional<bool> ok = parser.read("{\"a\":{\"hello\":true}}", true);
   //optional<bool> ok = parser.read(cin);
   cerr << endl << parser << endl;
   cerr << endl << (int)parser.character() << endl;
   
   if (ok == true)
   {
      cerr << endl
           << "ok joe" << endl
           << endl;
      //Label* label = &parser
   }
   else
   
      cerr << endl << parser << "Fail" << endl;
  */
  return 0;
}