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
     
   if (!bee::fish::parser::test())
      return 1;
   
   class Number : public Match
   {
   
   public:
      Number() : Match()
      {
         _match = new And(
            new Optional(Sign),
            Integer
         );
      }
     
   public:
      Match* Sign = new Or(
         new Character('+'),
         new Character('-')
      );
      
      Match* IntegerChar =
         new Range('0', '9');
      
      Match* Integer =
         new Repeat(IntegerChar, 1);
      
     virtual void write(
        ostream& out,
        size_t tabIndex = 0
     ) const
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
         
      Number number;
      
      number.read(line);
   
      if (number.matched())
      {
         cout << number << endl;
      }
      else
         cout << "Invalid number" << endl;
   }
  
   cout << "Bye" << endl;
   

   return 0;
}