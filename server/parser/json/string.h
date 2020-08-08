#ifndef BEE_FISH_PARSER_JSON__STRING_H
#define BEE_FISH_PARSER_JSON__STRING_H

#include "../parser.h"

namespace bee::fish::parser::json {

   class String : public And
   {
   public:
      String() : And(
         new Character('\"'),
         new Optional(
            new StringCharacters()
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
            
            virtual wchar_t character() {
               switch (value()[1]) {
               case '\\':
                  return '\\';
               case 'b':
                  return '\b';
               case 'f':
                  return '\f';
               case 'r':
                  return '\r';
               case 'n':
                  return '\n';
               case 't':
                  return '\t';
               case '\"':
                  return '\"';
               case 'u':
                  UnicodeHex& hex = 
                     (UnicodeHex&)(
                        ((Or&)((*this)[1])).item()
                     );
                  return hex.character();
               }
               
               return '\0';
            }
         };
         
         class UnicodeHex : public And
         {
         protected:
         
         public:
            UnicodeHex() : And(
               new Character('u'),
               new And(
                  new Hex(),
                  new Hex(),
                  new Hex(),
                  new Hex()
               )
            )
            {
            }
            
            virtual string& hex()
            {
               return (*this)[1].value();
            }
            
            virtual wchar_t character()
            {
               const char *hexString = hex().c_str();
               unsigned int hexNumber;
               sscanf(hexString, "%x", &hexNumber);
               return (wchar_t)hexNumber;
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
         
         virtual wchar_t character()
         {
            if (_index == 0)
               return value()[0];
            else
            {
               EscapedCharacter&
                  escaped =
                     (EscapedCharacter&)
                        (*this)[1];
               return escaped.character();
            }
         }
         
      };
      
      class StringCharacters: public
         Repeat<StringCharacter>
        
      {
         
      public:
      
         StringCharacters() : Repeat()
         {
         }
         
         virtual void addItem(Match* item)
         {
            StringCharacter* character =
               (StringCharacter*)item;
            _wvalue += character->character();
            delete item;
         }
         
         
      };
      
      virtual void write(ostream& out)
      {
         out << '\"';
         
         wstring& wstr = wvalue();
         Match::write(out, wstr);
         
         out << '\"';
      }

      virtual StringCharacters& characters()
      {
         Optional& optional =
            (Optional&)(*this)[1];
            
         return (StringCharacters&)
            (optional.item());
      }
      
      virtual wstring& wvalue()
      {
         return characters().wvalue();
      }
      
      virtual string& value()
      {
         return characters().value();
      }


   };
   
}

#endif