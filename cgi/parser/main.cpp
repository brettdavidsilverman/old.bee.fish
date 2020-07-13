#include <iostream>
#include "parser.h"
#include "request.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::server;

/*
Match operator and (const Match& and1, const Match& and2)
{
   return And(and1, and2);
}
*/

int main(int argc, char* argv[]) {
   
   cout << "bee.fish.parser "
        << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_DATE
        << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_NUMBER
        << endl;

   /*
   bee::fish::server::request request;
   
   Match& match = request;

   cout << "Reading from stdin." << endl;
   bool success = match.read(cin);
   
   cout << endl
        << match
        << endl
        <<  "Read " 
        << ( success ?
                "success" :
                "failure" )
        << "."
        << endl;
        
   return 0;
   */
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
   
   
   And sentence(
      {
         new Word("Hello"),
         new Character(' '),
         new Word("World")
      }
   );
   
   sentence.read("Hello World");
  
   cout << "Sentence: " << sentence << endl;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ciword.read("breTT");
   cout << "Case Insensitive Word: " << ciword << endl;
   
   // And
   And _and(
      {
         new Word("Brett"),
         new Character('.'),
         new Word("Silverman")
      }
   );
   _and.read("Brett.Silverman");
   
   cout << "And:" << _and << endl;
   
   
   // Or
   Or _or(
      {
         new Word("Brett")
      }
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
   Repeat repeat(new CharA());
   repeat.read("AAA");
   cout << "Repeat: " << repeat << endl;
   
   class BlankChar : public Or {
   public:
      BlankChar() : Or(
         {
            new Character(' '),
            new Character('\t')
         }
      )
      {
      }
   };

   class Blanks : public Repeat
   {
   public:
      Blanks() : Repeat(
         new BlankChar()
      )
      {
      }
      
      virtual optional<bool>
      match(int character)
      {
         optional<bool> matched =
            Repeat::match(character);
         if (matched == true)
            cout << "$";
         else if (matched == false)
            cout << "!";
         else
            cout << "#";
         return matched;
      }
      
   };

   And blanks(
      {
         new Character('*'),
         new Blanks(),
         new Character('*')
      }
   );
   blanks.read("*   *");
   cout << "Blanks:" << blanks[2] << endl;
   
   // Optional using And and Or
   And andOpt(
      {
         new Word("Abc"),
         new Or(
            {
               new And(
                  {
                     new Word("Optional"),
                     new Word("OPSM")
                  }
               ),
               new Word("OPSM")
            }
         ),
         new Character(Match::eof)
      }
   );
   andOpt.read("AbcOPSM");
   cout << "AndOpt:" << andOpt << endl;

   And optional(
      {
         new Word("Abc"),
         new Optional(
            // optional word
            new Word("Optional"),
            // next word
            new Word("OPSM") 
         )
      }
   );
   
   optional.read("AbcOptionalOPSM");
   cout << "Optional:" << optional << endl;
   Optional& _optional = (Optional&)(optional[1]);
   cout << "Next:" << 
      _optional.next() << endl;
   cout << "optional:" << _optional.optional() << endl;
   return 0;
   
}