#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json {

   const Character Quote = Character('\"');
   
   class _PlainCharacter :
      public Match
   {
   public:
      _PlainCharacter() 
      {
         _match = new Not(
            new Or(
               new Character('\\'),
               new Character('\"'),
               new Character('\r'),
               new Character('\n'),
               new Character('\b'),
               new Character('\f')
            )
         );
      }
      
      _PlainCharacter(
         const _PlainCharacter& source
      ) : Match(source)
      {
      }
      
      virtual bool match(Char character)
      {
         bool matched = 
            Match::match(character);
            
         if (matched)
            _character = character;
            
         return matched;
      }
      
      virtual Match* copy() const
      {
         return new _PlainCharacter(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex
      ) const
      {
         out << tabs(tabIndex)
             << "_PlainCharacter";
         writeResult(out);
         out << "("
             << (int)character()
             << ")";
      }
      
   };
   
   const _PlainCharacter PlainCharacter;
      
   const Or HexCharacter(
      new Range('0', '9'),
      new Range('a', 'f'),
      new Range('A', 'F')
   );
   
   class _Hex :
      public Match
   {
   private:
      BString _hex;

   protected:
      Match* createMatch()
      {
         return new Capture(
            new Repeat(
               HexCharacter.copy(),
               4, 4
            ),
            _hex
         );
      }
      
   public:
      _Hex() : Match()
      {
         _match = createMatch();
      }
      
      _Hex(const _Hex& source) :
         Match()
      {
         _match = createMatch();
      }
      
      virtual void success()
      {
         std::stringstream stream;
         stream << std::hex << _hex;
         uint16_t u16;
         stream >> u16;
         _character = u16;
         Match::success();
         
      }
      
      virtual Match* copy() const
      {
         return new _Hex(*this);
      }
      
   };
      
   const _Hex Hex;
   
   class _EscapedCharacter :
      public Match
      
   {
   private:
      _Hex* _hex = nullptr;
      
   protected:
     
      Match* captureCharacter(
         Char input,
         Char output
      )
      {
         return
            new Invoke(
               new Character(input),
               [this, output](Match*)
               {
                  _character = output;
               }
            );
      }
      
      
   public:
      _EscapedCharacter() :
         Match()
      {
      }
      
      _EscapedCharacter(
         const _EscapedCharacter& source
      ) :
         Match()
      {
      }
      
      virtual void setup()
      {
         
         Match* invokeHex = new
            Invoke(
               _hex = new _Hex(),
               [this](Match*)
               {
                  _character = 
                     _hex->character();
               }
            );
            
         Match* match = new And(
            new Character('\\'),
            new Or(
               captureCharacter('\\', '\\'),
               captureCharacter('b', '\b'),
               captureCharacter('f', '\f'),
               captureCharacter('r', '\r'),
               captureCharacter('n', '\n'),
               captureCharacter('t', '\t'),
               captureCharacter('\"', '\"'),
               new And(
                  new Character('u'),
                  invokeHex
               )
            )
         );
         
         _match = match;
         
      }
      
      virtual Char character() const
      {
         return _character;
      }
      
      virtual Match* copy() const
      {
         return new _EscapedCharacter(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "_EscapedCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
   };
    
   const _EscapedCharacter EscapedCharacter;
      
   class _StringCharacter :
      public Match
   {
   protected:
   
      Or* _items = new Or(
         new _PlainCharacter(),
         new _EscapedCharacter()
      );
      
      
   public:
      _StringCharacter()
      {
         _match = _items;
      }
      
      _StringCharacter(
         const _StringCharacter& source
      )
      {
         _match = _items;
      }
      
      virtual Match* copy() const
      {
         return
            new _StringCharacter(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "_StringCharacter";
         writeResult(out);
         out << "(";
         if (matched())
            out << (int)character();
         out << ")";
      }
      
      virtual Char character() const
      {
         return _items->_item->character();
      }
      
   };
         
   const _StringCharacter StringCharacter;
   
   class _StringCharacters :
      public Repeat
   {
   public:
      BString _value;
      
   public:
      _StringCharacters() :
         Repeat(
            StringCharacter.copy(),
            0, 0
         )
      {
      }
      
      _StringCharacters(
         const _StringCharacters& source
      ) :
         Repeat(
            StringCharacter.copy(),
            0, 0
         )
      {
      }
      
      virtual void matchedItem(Match* match)
      {
         Char character = match->character();
         
         if (character != BString::EndOfFile &&
             _capture)
         {
            if ( _value.size() &&
                 isSurrogatePair(
                    _value[_value.size() - 1],
                    character
                 )
               )
            {
               _value[_value.size() - 1] =
                  joinSurrogatePair(
                     _value[_value.size() - 1],
                     character
                  );
            }
            else
               _value.push_back(character);
         }
         
         Repeat::matchedItem(match);
      }
      
      // https://unicodebook.readthedocs.io/unicode_encodings.html#surrogates
      bool isSurrogatePair(Char first, Char second)
      {
         return ( ( 0xD800 <= first && 
                    first <= 0xDBFF ) &&
                  ( 0xDC00 <= second &&
                    second <= 0xDFFF) );
      }
      
      Char joinSurrogatePair(Char first, Char second)
      {
         Char character = 0x10000;
         character += (first & 0x03FF) << 10;
         character += (second & 0x03FF);
         
         return character;
      }
      
      
      virtual Match* copy() const
      {
         return new _StringCharacters(*this);
      }
      
      friend ostream& operator <<
      (
         ostream& out,
         _StringCharacters& stringCharacters
      )
      {
         if (stringCharacters.matched())
            BString::writeEscaped(
               out,
               stringCharacters._value
            );
         else
            stringCharacters.write(out);
            
         return out;
      }
      
      virtual const BString& value() const
      {
         return _value;
      }
      
      virtual void capture()
      {
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) 
             << "StringCharacters(\"";
         BString::writeEscaped(out, _value);
         out << "\")";
      }
      
   };
   
   class _String :
      public Match
   {
   protected:
      _StringCharacters*
         _stringCharacters = nullptr;
      
   public:
      _String()
      {
      }
      
      _String(const _String& source)
      {
      }

      virtual void setup()
      {
         _stringCharacters =
            new _StringCharacters();
            
         _stringCharacters->_capture =
            _capture;
            
         _match = new And(
            Quote.copy(),
            _stringCharacters,
            Quote.copy()
         );
            
      }
      
      virtual Match* copy() const
      {
         return new _String(*this);
      }
      
      virtual const BString& value() const
      {
         return _stringCharacters->value();
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "_String";
         writeResult(out);
         out << "(";
         if (matched())
         {
            out << "\"";
            BString::writeEscaped(out, value());
            out << "\"";
         }
         out << ")";
            
      }
      
   };
   
   const _String String;
     // Label("String", _String());
   
}

#endif