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
      }
      
      virtual ~Number()
      {
         cerr << *this << endl;
      }
      
   public:
     
      MatchPointer<
         bee::fish::parser::Character
      > _plus = bee::fish::parser
            ::Character('+');
         
      MatchPointer<
         bee::fish::parser::Character
      > _minus = bee::fish::parser
            ::Character('-');
         
      MatchPointer<Or> _sign =
         _plus or _minus;
      
      const Range IntegerChar =
         Range('0', '9');

      MatchPointer<Capture> _integer =
         Capture(
            new Repeat(IntegerChar.copy(), 1)
         );
      
      MatchPointer<And> _number =
         (~_sign and _integer);
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         if (result() == false)
         {
            Match::write(out, tabIndex);
            return;
         }

        // out << *_sign << endl;
         
         if (_plus->matched())
            out << "+";
         else if (_minus->matched())
            out << "-";
         else
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
     
      Parser parser(number);
      
      parser.read(line);
   
      if (parser.result() == false)
         cout << "Invalid number" << endl;
         
      cout << number << endl;
      
   }
  
   cout << "Bye" << endl;
   
   
   return 0;
}