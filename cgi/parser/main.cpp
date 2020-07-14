#include <iostream>
#include "parser.h"
#include "request.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::server;


Match* operator and (Match& and1, Match& and2)
{
   return new And({&and1, &and2});
}

Match* operator or (Match& or1, Match& or2)
{
   return new Or({&or1, &or2});
}

Match* operator not (Match& _not)
{
   return new Not(&_not);
}

int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.parser "
        << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_DATE
        << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_PARSER__BUILD_NUMBER
        << endl;
        
        
        
   Match* start =
      *(new Character('a')) and
      *(new Character('b'));
   start->read("ab");
   cerr << *start;
   return 0;
   
/*
   bee::fish::server::GenericHeader request;
   
   Match& match = request;

   cerr << "Reading from stdin." << endl;
   bool success = match.read("hello: joe\r\n");
   
   cerr << endl;
   if (success)
      cerr << "ok joe" << endl
           << request.name() << ":\t"
           << request.value() << " "
           << endl;
    */
   /*
   
   if (success == true)
      cerr << "ok joe" << endl
           << request.method() << " "
           << request.path() << " "
           << request.version() << endl;
   */
   
        
   cerr << "Hit enter:";
   cin.get();
   
   
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

   
   And sentence(
      {
         new Word("Hello"),
         new Character(' '),
         new Word("World")
      }
   );
   
   sentence.read("Hello World");
  
   cerr << "Sentence: " << sentence << endl;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ciword.read("breTT");
   cerr << "Case Insensitive Word: " << ciword << endl;
   cerr << endl;

   // And
   And _and(
      {
         new Word("Brett"),
         new Character('.'),
         new Word("Silverman")
      }
   );
   _and.read("Brett.Silverman");
   
   cerr << "And:" << _and << endl;
   cerr << endl;

   
   // Or
   Or _or(
      {
         new Word("Brett")
      }
   );
   
   _or.read("Brett");
   
   cerr << "Or:" << _or << endl;
   cerr << endl;

   // Not
   Not _not1(
      new Range('a', 'z')
   );
   
   _not1.read("A");
   cerr << "Not1:" << _not1 << endl;
   cerr << endl;

   Not _not2(
      new Range('a', 'z')
   );
   
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
   Repeat repeat(new CharA());
   repeat.read("AAA");
   cerr << "Repeat: " << repeat << endl;
   cerr << endl;

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

   And blanks(
      {
         new Character('*'),
         new Blanks(),
         new Character('*')
      }
   );
   blanks.read("*   *");
   cerr << "Blanks:" << blanks << endl;
   cerr << endl;

   // Optional

   And optional(
      {
         new Word("Brett"),
         new Optional(
            new Blanks(),
            new Word("ABC")
         )
      }
   );
   
   optional.read("Brett  ABC");
   cerr << "Optional:" << optional << endl;
   cerr << "optional:" << 
      optional[1][0] << endl;
   cerr << "next:" <<
      optional[1][1] << endl;
   cerr << endl;
   
   Optional opt(
      new Repeat(
         new Character('.')
      ),
      new Word("Brett")
   );
   opt.read(".Brett");
   
   cerr << endl << "*********" << endl;
   /*
   Or __or (
        // {
           // new And(
               {
                  new Repeat(
                     new Character('.')
                  ),
                  new Word("Brett")
               }
           // )
            new Word("Brett")
       //  }
      );
   __or.read(".Brett");
   cerr << "_Opt" << __or << endl;
   
   cerr << endl << "*********" << endl;
   
*/
   
   return 0;
   
}