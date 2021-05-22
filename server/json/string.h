#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json {

   const Label Quote =
      Label(
         "Quote",
         new bee::fish::parser::
         Character('\"')
      );
   
   class _PlainCharacter :
      public Match
   {
   public:
      _PlainCharacter() 
      {
         _match = new Not(
            new Or(
               new bee::fish::parser::
                  Character('\\'),
               new bee::fish::parser::
                  Character('\"'),
               new bee::fish::parser::
                  Character('\r'),
               new bee::fish::parser::
                  Character('\n'),
               new bee::fish::parser::
                  Character('\b'),
               new bee::fish::parser::
                  Character('\f')
            )
         );
      }
      
      _PlainCharacter(
         const _PlainCharacter& source
      ) : Match(source)
      {
      }
      
      virtual bool match(const Char& character)
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
               new bee::fish::parser::
                  Character(input),
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
            new bee::fish::parser::
               Character('\\'),
            new Or(
               captureCharacter('\\', '\\'),
               captureCharacter('b', '\b'),
               captureCharacter('f', '\f'),
               captureCharacter('r', '\r'),
               captureCharacter('n', '\n'),
               captureCharacter('t', '\t'),
               captureCharacter('\"', '\"'),
               new And(
                  new bee::fish::parser::
                     Character('u'),
                  invokeHex
               )
            )
         );
         
         _match = match;
         _setup = true;
         
      }
      
      virtual const Char& character() const
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
      
      virtual const Char& character() const
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
         const Char& character = match->character();
         
         _value.push_back(character);
         
         Repeat::matchedItem(match);
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
            stringCharacters._value.
               writeEscaped(
                  out
               );
         else
            stringCharacters.write(out);
            
         return out;
      }
      
      virtual const BString& value() const
      {
         return _value;
      }
      
      virtual void capture(const Char& character)
      {
         //_value.push_back(character);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) 
             << "StringCharacters(\"";
         _value.writeEscaped(out);
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
         _capture = source._capture;
      }

      virtual void setup()
      {
         _stringCharacters =
            new _StringCharacters();
            
         _stringCharacters->_capture = _capture;
         
         _match = new And(
            Quote.copy(),
            _stringCharacters,
            Quote.copy()
         );
         
         Match::setup();
            
      }
      
      virtual Match* copy() const
      {
         return new _String(*this);
      }
      
      virtual const BString& value() const
      {
         // cerr << _value << endl;
         // cerr << _stringCharacters->value() << endl;
         
         return _stringCharacters->value();
      }
      
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         if (matched())
         {
            const BString& value =
               this->value();
 
            out << "String(\"";
            
            value.writeEscaped(out);
            
            out << "\")";
         }
         else
         {
            out << tabs(tabIndex)
                << "_String";
            writeResult(out);
            out << "()";
         }
            
      }
      
      
   };
   
   const _String String;
     // Label("String", new _String());
   
}

#endif