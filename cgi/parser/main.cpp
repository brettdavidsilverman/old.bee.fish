#include <iostream>
#include "parser.h"
#include <mcheck.h>

using namespace std;
using namespace Bee::Fish::Parser;
/*
Match operator and (const Match& and1, const Match& and2)
{
   return And(and1, and2);
}
*/

int main(int argc, char* argv[]) {
   
   cout << "Hello World" << endl;
   
   // Character
   Character character('A');
   character.read("Ab");
   cout << "Character:" 
        << character
        << endl;
   
  
   // Range
   Range range('a', 'z');
   range.read("abc");
   cout << "Range:" << range << endl;
   
   
   // Word
   Word word("Brett");
   word.read("Bre", false);
   word.read("tt");
   cout << "Word:" << word << endl;
   
   
   Match* sentence = new And(
      new Word("Hello"),
      new Character(' '),
      new Word("World")
   );
   
   sentence->read("Hello ");
  
   cout << "Sentence: " << *sentence << endl;
   delete sentence;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ciword.read("breTT");
   cout << "Case Insensitive Word: " << ciword << endl;
   
   // And
   And _and(
      new Word("Brett"),
      new Character('.'),
      new Word("Silverman")
   );
   _and.read("Brett.Silverman");
   
   cout << "And:" << _and << endl;
   
   
   // Or
   Or _or(
      new Word("Brett")
   );
   
   _or.read("Brett");
   
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
   
   // Repeat whitespace
   class WhitespaceChar : public Or {
   public:
      WhitespaceChar() :
         Or(
            new Character(' '),
            new Character('\t')
         )
      {
      }
   };

   class Whitespace :
      public Repeat<WhitespaceChar> 
   {
   public:
      Whitespace()
      {}
   };
   
   Whitespace whitespace;
   whitespace.read("\t a");
   cout << "Whitespace: " << whitespace << endl;

   
   // Optional using And
   And andOpt(
      new Word("Abc"),
      new Or(
         new And(
            new Word("Optional"),
            new Word("OPSM")
         ),
         new Word("OPSM")
      ),
      new Character(Match::eof)
   );
   andOpt.read("AbcOPSM");
   cout << "AndOpt:" << andOpt << endl;
   
   class _Optional : public Or {
   public:
      _Optional(
         Match* optional,
         Match* next
      ) :
         Or (
            new And(
               optional,
               next
            ),
            next->copy()
         )
      {
      }
      
      virtual Match* optional() {
         Match* _and = (*this)[0];
         Match* optional = (*_and)[0];
         return optional;
      }
      
      virtual Match* next() {
         Match* _and = (*this)[0];
         Match* next;
         if (_and->success() == true)
            next = (*_and)[1];
         else
            next = (*this)[1];
         return next;
      }
      
   };

 
   
   And optional(
      new Word("Abc"),
      new _Optional(
         new Word("Optional"),
         new Word("OPSM")
      )
      
   );
   
   optional.read("AbcOPSM");
   cout << "_Optional:" << optional << endl;
   _Optional* _optional = (_Optional*)(optional[1]);
   cout << "Next:" << 
      *(_optional->next()) << endl;
   cout << "optional:" << *(_optional->optional()) << endl;
   return 0;
}