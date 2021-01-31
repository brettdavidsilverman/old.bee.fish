#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iomanip>
#include <iostream>       // std::cout, std::hex
#include <string>         // std::string, std::u16string
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8_utf16

#include "../parser/parser.h"

namespace bee::fish::json {
      
   const Match PlainCharacter =
      not (
         Character('\\') or
         Character('\"') or
         Character('\r') or
         Character('\n')
      );
   
   const Match Hex =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   const Match UnicodeHex =
      Character('u') and (
         Hex and
         Hex and
         Hex and
         Hex
      );
      
   const Match EscapedCharacter =
      Character('\\') and (
         Character('\\') or
         Character('b') or
         Character('f') or
         Character('r') or
         Character('n') or
         Character('t') or
         Character('\"') or
         UnicodeHex
      );
    
   const Match StringCharacter =
      PlainCharacter or
      EscapedCharacter;
      
   const Match Quote = Character('\"');
   
   const Match String =
      Quote and
      Repeat(StringCharacter, 0) and
      Quote;
         
      
   /*
            
         virtual string& hex()
         {
            return (*this)[1].value();
         }
            
         virtual string character()
         {
            const char *hexString = hex().c_str();
            unsigned int hexNumber;
            sscanf(hexString, "%x", &hexNumber);
            char high = (char)(hexNumber >> 8);
            char low = (char)(hexNumber & 0x00FF);
            string value;
            value += high;
            value += low;
            return value;
         }
            
         virtual string character() {
            switch (value()[1]) {
            case '\\':
               return "";
            case 'b':
               return "\b";
            case 'f':
               return "\f";
            case 'r':
               return "\r";
            case 'n':
               return "\n";
            case 't':
               return "\t";
            case '\"':
               return "\"";
            case 'u':
               return _unicodeHex->character();
            }
               
            return "\0";
         }
      };
       
      class StringCharacter : public Or
      {
      protected:
         PlainCharacter* _plainCharacter;
         EscapedCharacter* _escapedCharacter;
      public:
         StringCharacter() : Or (
            _plainCharacter =
               new PlainCharacter(),
            _escapedCharacter =
               new EscapedCharacter()
         )
         {
            _capture = true;
         }
         
         virtual string character()
         {
            string retval;
            
            if (_index == 0)
            {
               retval = _plainCharacter->character();
            }
            else
            {
               retval =
                  _escapedCharacter->
                     character();
            }
            
            return retval;
         }
         
      };
      
      class StringCharacters: public Repeat
      {
      protected:
         string _value;
         
      public:
      
         StringCharacters() :
            Repeat(
               new StringCharacter()
            )
         {
            _capture = true;
         }
         
         
         virtual void addItem(Match* item)
         {
            if (_capture)
            {
               //cerr << *item << endl;
               StringCharacter* character =
                  (StringCharacter*)item;
               string str = character->character();
               _value += str;
              // wcerr << _wvalue << endl;
            }
            Repeat::addItem(item);
         }
         
         virtual string& value()
         {
            return _value;
         }
         
      };
      
      virtual void write(ostream& out)
      {
         out << '\"';
         
         string& str = value();
         String::write(out, str);
         
         out << '\"';
      }
      
      static void write(ostream& out, const string& str)
      {
         for (auto it = str.begin();
                   it != str.end();
                   ++it )
                   
         {
           
             wchar_t wc = *it;
             long number = ( long)wc;
             short high = (number & 0xFF00) >> 8;
             short low = (number & 0x00FF);
            
            if (high)
            {
               out << "\\u"
                   << std::hex 
                   << std::uppercase 
                   << std::setw(2) 
                   << std::setfill('0') 
                   << high
                   << low;
            }
            else
            {
               Match::write(
                  out,
                  (unsigned int)low
               );
            }
            
         }
         
      }
      
      
      static void write(wostream& wout, const wstring& wstr)
      {
         wout << wstr;
      }
      
      static void write(ostream& out, const string& str)
      {
         for (const char& c : str)
         {
            if (c >= 0x00 && c <= 0x1F)
            {

               out << "\\u00"
                   << std::uppercase 
                   << std::setw(2) 
                   << std::setfill('0') 
                   << std::right
                   << std::hex
                   << (unsigned char)c;
            }
            else
               Match::write(out, c);
         }
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
   */
   
}

#endif