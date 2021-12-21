#include <iostream>
#include <fstream>
#include "parser.h"
#include "test.h"

using namespace std;
using namespace BeeFishParser;

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
      if (!BeeFishParser::test())
         return 1;
            
      return 0;
   }
   
   class Number : public Match
   {
   
   public:
      Number() : Match()
      {
         _match = _number;
      }
      
      virtual ~Number()
      {
         cerr << *this << endl;
      }
      
   public:
     
      BeeFishParser::Character* _plus =
         new BeeFishParser::Character('+');
         
      BeeFishParser::Character* _minus = 
         new BeeFishParser::Character('-');
         
      Or* _sign = new Or(
         _plus,
         _minus
      );
      
      Range* _integerChar =
         new Range('0', '9');

      Capture* _integer =
         new Capture(
            new Repeat(_integerChar, 1)
         );
      
      And* _number = new And(
         new Optional(_sign),
         _integer
      );
      
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