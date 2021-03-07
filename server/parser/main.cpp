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
     
   //JSON = _JSON;
   
   if (!bee::fish::parser::test())
      return 1;
   
   class Number : public Match
   {
   
   public:
      Number() : Match()
      {
         setMatch(
            ~Sign and
             Integer
         );
      }
     
   public:
      MatchPtr Sign =
         Character('+') or
         Character('-');
      
      MatchPtr IntegerChar =
         Range('0', '9');
      
      MatchPtr Integer =
         Repeat(IntegerChar, 1);
      
     virtual void write(ostream& out) const
     {
         if (Sign->matched())
         {
            if (Sign->value() == "+")
               out << "Plus";
            else if (Sign->value() == "-")
               out << "Minus";
         }
         else
            out << "Plus";
         out << " ";
         out << Integer->value();
     }
     
   };
   
   
   string line;
   while (!cin.eof())
   {
      cout << "Number: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
         
      MatchPtrBase number =
         make_shared<Number>();
      
      number->read(line);
   
      if (number->matched())
      {
         cout << *number << endl;
      }
      else
         cout << "Invalid number" << endl;
   }
  
   cout << "Bye" << endl;
   
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