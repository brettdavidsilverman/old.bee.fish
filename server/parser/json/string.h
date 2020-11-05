#ifndef BEE_FISH_PARSER_JSON__STRING_H
#define BEE_FISH_PARSER_JSON__STRING_H

#include "../parser.h"

namespace bee::fish::parser::json {

   class String : public And
   {
   protected:
 
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
      
      virtual std::string name()
      {
         return "String";
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
            _capture = true;
            _inputs[0]->_capture = true;
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
      protected:
         wstring _wvalue = L"";
         
      public:
      
         StringCharacters() : Repeat()
         {
            _capture = true;
         }
         
         
         virtual void addItem(Match* item)
         {
            if (_capture)
            {
               StringCharacter* character =
                  (StringCharacter*)item;
               _wvalue += character->character();
            }
            Repeat::addItem(item);
         }
         
         virtual wstring& wvalue()
         {
            return _wvalue;
         }
         
      };
      
      virtual void write(ostream& out)
      {
         out << '\"';
         
         wstring& wstr = wvalue();
         write(out, wstr);
         
         out << '\"';
      }
      
      static void write(ostream& out, const wstring& wstr)
      {
         for (const wchar_t wc : wstr) {
            unsigned short cHigh = (wc & 0xFF00) >> 16;
            unsigned short cLow = (wc & 0x00FF);
            
            if (cHigh)
            {
               out << "\\u";
               out << std::hex << std::setw(4);
               out << cHigh;
               out << cLow;
               out << std::dec;
            }
            else
            {
               Match::write(out, cLow);
            }
         }
      }
      
      static void write(wostream& wout, const wstring& wstr)
      {
         wout << wstr;
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