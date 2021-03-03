#include <iostream>
#include <clocale>
#include <locale>
#include "parser.h"
#include "test.h"
//#include "json.h"

using namespace std;
using namespace bee::fish::parser;

int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
           
  // std::locale::global(std::locale("C.UTF-8"));
   
   //JSON = _JSON;
   
   if (!bee::fish::parser::test())
      return 1;
   
   return 0;
      
   class Number : public Match
   {
   public:
      Capture Sign =
         Capture(
            Character('+') or
            Character('-')
         );
      
      Range IntegerChar =
         Range('0', '9');
      
      Capture Integer = 
         Capture(
            Repeat(IntegerChar, 1)
         );
      
   public:
      Number() : Match()
      {
         setMatch(
            Optional2(
               Sign,
               Integer
            )
         );
      }
   };
   
   Number number;
   
   cout << number << ":";
   
   number.read(cin);
   if (number.matched())
   {
      if (number.Sign.matched())
      {
         if (number.Sign.value() == "+")
            cout << "Plus";
         else if (number.Sign.value() == "+")
            cout << "Minus";
         else
            cout << "?";
      }
      cout << number.Integer.value();
   }
   else
      cout << "Invalid number: " << number;
   cout << endl;
   
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