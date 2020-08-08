#include <iostream>
#include "parser.h"

using namespace std;

namespace bee::fish::parser {

bool test() {
   
   bool ok = true;
   
   // Character
   Character character('A');
   ok &= character.read("Ab");
   cerr << "Character:" 
        << character
        << endl;
   cerr << ok << endl;
   
   // Range
   Range range('a', 'z');
   ok &= range.read("abc");
   cerr << "Range:" << range << endl;
   cerr << ok << endl;

   
   // Word
   Word word("Brett");
   word.read("Bre", false);
   ok &= word.read("tt");
   cerr << "Word:" << word << endl;
   cerr << ok << endl;

   
   And sentence = 
      Word("Hello")
      and Character(' ')
      and Word("World");
   
   ok &= sentence.read("Hello World");
  
   cerr << "Sentence: " 
        << sentence << endl;
   cerr << ok << endl;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ok &= ciword.read("breTT");
   cerr << "Case Insensitive Word: " << ciword << endl;
   cerr << ok << endl;
 
   // Or
   Or _or = Word("Brett") or Word("sad");
   
   ok &= _or.read("sad");
   
   cerr << "Or:" << _or << endl;
   cerr << ok << endl;

   // Not
   Not _not1 =
      not Range('a', 'z');
   
   ok &=_not1.read("A");
   cerr << "Not1:" << _not1 << endl;
   cerr << ok << endl;

   Not _not2 =
      not Range('a', 'z');
      
   _not2.read("a");
   if (_not2.success() == true)
      ok = false;
   cerr << "Not2:" << _not2 << endl;
   cerr << ok << endl;

   // Repeat
   
   Repeat repeat(new Character('A'));
   ok &= repeat.read("AAA");
   cerr << "Repeat:" << repeat << endl;
   cerr << ok << endl;

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
      and Repeat(new BlankChar())
      and Character('*');
      
   ok &= blanks.read("*   *");
   cerr << "Blanks:" << blanks << endl;
   cerr << ok << endl;

   // Optional

   And optional(
      new Word("Brett"),
      new Optional(
         new Repeat(new BlankChar())
      ),
      new Word("ABC")
   );
   
   ok &= optional.read("Brett   ABC");
   cerr << "Optional:" << optional << endl;
   cerr << "optional:" << 
      optional[1].value() << endl;
   cerr << ok << endl;
   
   class CharacterDot : public Character {
   public:
      CharacterDot() : Character('.')
      {}
   };
   
   And opt = Optional(
      new Repeat(new CharacterDot())
   ) and Word("Brett");
 
   ok &= opt.read("...Brett");
   
   cerr << opt << endl;
   cerr << ok << endl;
   
   if (ok)
      cerr << endl << "SUCCESS" << endl;
   else
      cerr << endl << "FAIL" << endl;
      
   return ok;
   
}

}