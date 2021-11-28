#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON {

   class Quote : public BeeFishParser::Character {
   public:
      Quote() : Character('\"') {

      }
   };

   class PlainCharacter : public Match
   {
   protected:
      Match* _match;

   public:
      PlainCharacter() : Match() 
      {
         _match = new Not(
            new Or(
               new BeeFishParser::
                  Character('\\'),
               new BeeFishParser::
                  Character('\"'),
               new BeeFishParser::
                  Character('\r'),
               new BeeFishParser::
                  Character('\n'),
               new BeeFishParser::
                  Character('\b'),
               new BeeFishParser::
                  Character('\f')
            )
         );
      }
      
      virtual bool match(const Char& character)
      {
         bool matched = 
            _match->matchCharacter(character);
            
         if (matched)
            _character = character;
            
         if (_match->_result == true)
            success();
         else if (_match->_result == false)
            fail();

         return matched;
      }
      
   };
   
   class HexCharacter : public Or {
   public:
      HexCharacter() : Or (
         new Range('0', '9'),
         new Range('a', 'f'),
         new Range('A', 'F')
      )
      {

      }
   };

   class Hex : public Capture
   {
   private:
      BString _hex;
      BString _value;

   public:
      Hex() : Capture(
         new Repeat<HexCharacter>(
            4, 4
         ),
         _hex
      )
      {
      }
            
      virtual void success()
      {
         cout << "<" << _hex << ">" << endl;
         std::stringstream stream;
         stream << std::hex << _hex;
         uint32_t u32;
         stream >> u32;
         _value = Char(u32);
         cout << "{" << u32 << "}" << endl;
         Match::success();

      }

      virtual const BString& value() const {
         return _value;
      }
          
   };
      
/*   
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
               new BeeFishParser::
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
            new BeeFishParser::
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
                  new BeeFishParser::
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
      ) :
         Match()
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
      public Capture
   {
      
   public:
      _StringCharacters() :
         Capture(
            new Repeat(
               StringCharacter.copy(),
               0, 0
            )
         )
      {
      }
      
      _StringCharacters(
         const _StringCharacters& source
      ) :
         Capture(
            new Repeat(
               StringCharacter.copy(),
               0, 0
            )
         )
      {
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
            stringCharacters.value().
               writeEscaped(
                  out
               );
         else
            stringCharacters.write(out);
            
         return out;
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
      
      _String(const _String& source) :
         Match()
      {
      }

      virtual void setup()
      {
         _stringCharacters =
            new _StringCharacters();
            
         _match = new And(
            Quote.copy(),
            _stringCharacters,
            Quote.copy()
         );
         
         _setup = true;
            
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
   
     // Label("String", new _String());
*/
}

#endif