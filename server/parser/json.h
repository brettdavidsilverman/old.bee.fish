#ifndef BEE_FISH_PARSER__JSON_H
#define BEE_FISH_PARSER__JSON_H

#include "or.h"
#include "optional.h"
#include "word.h"
#include "and.h"
#include "range.h"

namespace bee::fish::parser::json
{
   class Number : public And
   {
   public:
      Number() : And(
         new Integer(),
         new Optional(
            new Fraction()
         ),
         new Optional(
            new Exponent()
         )
      )
      {
      }
      
      class IntegerCharacter : public Range
      {
      public:
         IntegerCharacter() : Range('0', '9')
         {
         }
            
      };
      
      class Integer : public
         Repeat<IntegerCharacter>
      {
      public:
         Integer() : Repeat()
         {
         }

      };
      
      class Fraction : public And
      {
      public:
         Fraction() : And(
            new Character('.'),
            new Integer()
         )
         {
         }
      };
      
      class Exponent : public And
      {
      public:
         Exponent() : And(
            new Or(
               new Character('E'),
               new Character('e')
            ),
            new Sign(),
            new Integer()
         )
         {
         }
      };
      
      class Sign : public Or
      {
      public:
         Sign() : Or(
            new Character('+'),
            new Character('-')
         )
         {
         }
      };
         
   };
   
   class String : public And
   {
   public:
      String() : And(
         new Character('\"'),
         new Optional(
            new Repeat<StringCharacter>()
         ),
         new Character('\"')
      )
      {
      }
      
      class StringCharacter : public Or
      {
      public:
         StringCharacter() : Or (
            new Not(
               new Or(
                  new Character('\\'),
                  new Character('\"')
               )
            ),
            new EscapedCharacter()
         )
         {
         }
         
         class EscapedCharacter : public And
         {
         public:
            EscapedCharacter() : And(
               new Character('\\'),
               new Or(
                  new Character('\\'),
                  new Character('b'),
                  new Character('f'),
                  new Character('r'),
                  new Character('n'),
                  new Character('t'),
                  new Character('\"'),
                  new UnicodeHex()
               )
            )
            {
            }
         };
         
         class UnicodeHex : public And
         {
         public:
            UnicodeHex() : And(
               new Character('u'),
               new Hex(),
               new Hex(),
               new Hex(),
               new Hex()
            )
            {
            }
            
            class Hex : public Or
            {
            public:
               Hex() : Or(
                  new Range('0', '9'),
                  new Range('a', 'f'),
                  new Range('A', 'F')
               )
               {
               }
            };
            
         };
         
      };
   };
   
   class JSON : public Or
   {
   public:
      JSON() : Or(
         new Number(),
         new String(),
       //  new Array(),
       //  new Object(),
         new Word("true"),
         new Word("false"),
            new Word("null")
         )
      {
      }
   };
}

#endif