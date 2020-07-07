#include <iostream>
#include "parser.h"

using namespace std;
using namespace Bee::Fish::Parser;

int main(int argc, char* argv[]) {
   cout << "Hello World" << endl;
 
   // Character
   Character character('A');
   character.read("A");
   cout << "Character:" << character << endl;
   
   // Range
   Range range('a', 'z');
   range.read("abc");
   cout << "Range:" << range << endl;
   
   // Word
   Word word("Brett");
   word.read("Bre", false);
   word.read("tt");
   cout << "Word:" << word << endl;
   
   // And
   And _and(
      new Word("Brett"),
      new Character('.'),
      new Word("Silverman")
   );
   _and.read("Brett.Silverman");
   //cout << "And:" << _and.items() << endl;
   _and.output(cout);
   
   /*
   // Or
   Or _or(
      new Word("Brett"),
      new Word("David")
   );
   _or.read("David");
   cout << "Or:" << _or << endl;
   
   // Not
   Not _not1(
      new Range('a', 'z')
   );
   
   _not1.read("A");
   cout << "Not1:" << _not1 << endl;
   
   Not _not2(
      new Range('a', 'z')
   );
   
   _not2.read("a");
   cout << "Not2:" << _not2 << endl;
   
   // Optional
   And optional(
      new Word("Abc"),
      new Optional(
         new Word("123")
      )
   );
   optional.read("Abc12a");
   cout << "Optional:" << optional << endl;
   
   // Repeat
   class CharA : public Character {
   public:
      CharA() : Character('A')
      {
      }
   };
   Repeat<CharA> repeat;
   repeat.read("AAA");
   cout << "Repeat: " << repeat << endl;
   */
   return 0;
}