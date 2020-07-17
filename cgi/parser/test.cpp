#include <iostream>
#include "parser.h"

using namespace std;

namespace bee::fish::parser {

void test() {
   
   // Character
   Character character('A');
   character.read("Ab");
   cerr << "Character:" 
        << character
        << endl;
   cerr << endl;
  
   // Range
   Range range('a', 'z');
   range.read("abc");
   cerr << "Range:" << range << endl;
   cerr << endl;

   
   // Word
   Word word("Brett");
   word.read("Bre", false);
   word.read("tt");
   cerr << "Word:" << word << endl;
   cerr << endl;

   
   And sentence = 
      Word("Hello")
      and Character(' ')
      and Word("World");
   
   sentence.read("Hello World");
  
   cerr << "Sentence: " << sentence << endl;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ciword.read("breTT");
   cerr << "Case Insensitive Word: " << ciword << endl;
   cerr << endl;

   // Or
   Or _or = Word("Brett") or Word("sad");
   
   _or.read("sad");
   
   cerr << "Or:" << _or << endl;
   cerr << endl;

   // Not
   Not _not1 =
      not Range('a', 'z');
   
   _not1.read("A");
   cerr << "Not1:" << _not1 << endl;
   cerr << endl;

   Not _not2 =
      not Range('a', 'z');
      
   _not2.read("a");
   cerr << "Not2:" << _not2 << endl;
   cerr << endl;

   // Repeat
   class CharA : public Character {
   public:
      CharA() : Character('A')
      {
      }
   };
   
   Repeat<CharA> repeat;
   repeat.read("AAA");
   cerr << "Repeat:" << repeat << endl;
   cerr << endl;

   class BlankChar : public Or {
   public:
      BlankChar() : Or(
         new Character(' '),
         new Character('\t')
      )
      {
      }
   };


   And blanks =
      Character('*')
      and Repeat<BlankChar>()
      and Character('*');
      
   blanks.read("*   *");
   cerr << "Blanks:" << blanks << endl;
   cerr << endl;

   // Optional

   And optional(
      {
         new Word("Brett"),
         new Optional(
            new Repeat<BlankChar>()
         ),
         new Word("ABC")
      }
   );
   
   optional.read("Brett   ABC");
   cerr << "Optional:" << optional << endl;
   cerr << "optional:" << 
      optional[1].value() << endl;
   
   class CharacterDot : public Character {
   public:
      CharacterDot() : Character('.')
      {}
   };
   
   And opt = Optional(
      new Repeat<CharacterDot>()
   ) and Word("Brett");
 
   opt.read("...Brett");
   
   cerr << opt << endl;

   
}

}