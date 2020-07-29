#ifndef BEE_FISH_PARSER__JSON_H
#define BEE_FISH_PARSER__JSON_H

#include <map>
#include "or.h"
#include "optional.h"
#include "word.h"
#include "and.h"
#include "range.h"
#include "lazy-load.h"

namespace bee::fish::parser
{
      
   
   class JSON : public And
   {
   public:
      JSON() : And(
            new Optional(
               new BlankSpace()
            ),
            new Or(
               new True(),
               new False(),
               new Null(),
               new Number(),
               new String(),
               new Array(),
               new Object()
            ),
            new Optional(
               new BlankSpace()
            )
         )
         {
         }
      
      class True : public Word
      {
      public:
         True() : Word("true")
         {
         }
      };
      
      class False : public Word
      {
      public:
         False(): Word("false")
         {
         }
      };
      
      class Null : public Word
      {
      public:
         Null(): Word("null")
         {
         }
      };

      class Number : public And
      {
      public:
         Number() : And(
            new Optional(
               new Sign()
            ),
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
      
      
      class Identifier : public And
      {
      public:
         Identifier() : And(
            new Or(
               new And(
                  new Character('_'),
                  new Or(
                     new Range('0', '9'),
                     new Range('A', 'Z'),
                     new Range('a', 'z')
                  )
               ),
               new Or(
                  new Range('A', 'Z'),
                  new Range('a', 'z')
               )
            ),
            new Optional(
               new Repeat<IdentifierCharacter>()
            )
         )
         {
         }
         
         class IdentifierCharacter : public Or
         {
         public:
            IdentifierCharacter() : Or(
               new Character('_'),
               new Range('0', '9'),
               new Range('A', 'Z'),
               new Range('a', 'z')
            )
            {
            }
         };
      };
      
      class Array : public And
      {
      public:
         Array() : And(
            new Character('['),
            new Optional(
               new And(
                  new LazyLoad<JSON>(),
                  new Optional(
                     new Repeat<Item>()
                  )
               )
            ),
            new Character(']')
         )
         {
         }
         
         class Item : public And
         {
         public:
            Item() : And(
               new Character(','),
               new LazyLoad<JSON>()
              
            )
            {
            }
         };
         
      };
      
      class Object : 
         public And
      {
      public:
         Object() : And(
            new Character('{'),
            new Optional(
               new BlankSpace()
            ),
            new Optional(
               new Fields()
            ),
            new Optional(
               new BlankSpace()
            ),
            new Character('}')
         )
         {
         }
         
         class Fields :
             public And,
             public map<string, JSON*>
         {
         public:
            Fields() : And(
               new Field(),
               new Optional(
                  new Repeat<Item>()
               )
            )
            {
            }
         };
         
         class Field : public And
         {
         public:
            Field() : And(
               new Label(),
               new Optional(
                  new BlankSpace()
               ),
               new Character(':'),
               new Optional(
                  new BlankSpace()
               ),
               new LazyLoad<JSON>()
            )
            {
            }
            
            string& label() {
               return (*this)[0].value();
            }
            
            JSON& _value() {
               return (JSON&)(*this)[4].value();
            }
            
            class Label : public Or
            {
            public:
               Label() : Or(
                  new String(),
                  new Identifier()
               )
               {
               }
               
            };
            
         };
         
         class Item : public And
         {
         public:
             Item() : And(
               new Optional(
                  new BlankSpace()
               ),
               new Character(','),
               new Optional(
                  new BlankSpace()
               ),
               new LazyLoad<Field>()
              
            )
            {
            }
            
            Field& field() {
               return (Field&)(*this)[3];
            }
         };
         
      };
      
      class BlankSpaceCharacter : public Or
      {
      public:
         BlankSpaceCharacter() : Or (
            new Character(0x0020),
            new Character(0x000A),
            new Character(0x000D),
            new Character(0x0009),
            new Character('\r'),
            new Character('\n')
         )
         {
         }
      };
      
      class BlankSpace : public Repeat<BlankSpaceCharacter>
      {
      public:
         BlankSpace() : Repeat()
         {
         }
      };
      

   };
   
   
}

#endif