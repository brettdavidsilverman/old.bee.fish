#include <iostream>
#include <clocale>
#include <locale>
#include "parser.h"
#include "test.h"

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
         _match = _number.get();
      }
      
   public:
      MatchPointer<Or> _sign =
         Character('+') or
         Character('-');
      
      const Range IntegerChar =
         Range('0', '9');

      MatchPointer<Repeat> _integer =
         Repeat(IntegerChar.copy(), 1);
      
      MatchPointer<And> _number =
         ~_sign and
          _integer;
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         
        // out << ( _sign._pointer == (_number->_inputs[0]->_match) ) << endl;
         cerr << "Here" << endl;
         if (_sign->matched())
         {
            if (_sign->value() == "+")
               out << "Plus";
            else if (_sign->value() == "-")
               out << "Minus";
         }
         else
            out << "Plus";
            
         out << " ";
         out << _integer->value();
         
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