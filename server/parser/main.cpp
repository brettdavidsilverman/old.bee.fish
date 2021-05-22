#include <iostream>
#include <fstream>
#include "parser.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;

int main(int argc, const char* argv[]) {
         
   cerr << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
           
   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing parser..." << endl << endl;
      if (!bee::fish::parser::test())
         return 1;
            
      return 0;
   }
   
   class Number : public Match
   {
   
   public:
      Number() : Match()
      {
         _match = _number.get();
         _integer->_capture = true;
         _sign->_capture = true;
      }
      
   public:
      MatchPointer<Or> _sign =
         bee::fish::parser::Character('+') or
         bee::fish::parser::Character('-');
      
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
         if (_result == false)
         {
            Match::write(out, tabIndex);
            return;
         }
         
         if (_sign->matched())
         {
            if (_sign->value() == "+")
               out << "++";
            else if (_sign->value() == "-")
               out << "--";
            else
               out << "??" << _sign->value();
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
   
      if (number._result == false)
         cout << "Invalid number" << endl;
         
      cout << number << endl;
      
   }
  
   cout << "Bye" << endl;
   
   
   return 0;
}