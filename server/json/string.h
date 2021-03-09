#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json {

   const MatchPtr Quote = new Character('\"');

   class _Character
   {
   public:
      virtual Char character() const = 0;
   };
   
   class _PlainCharacter :
      public Match,
      public _Character
   {
   public:
      Char _character;
      
   public:
      _PlainCharacter() :
         _character(0)
      {
         setMatch(
            not (
               Character('\\') or
               Character('\"') or
               Character('\r') or
               Character('\n') or
               Character('\b') or
               Character('\f')
            )
         );
      }
      
      _PlainCharacter(
         const _PlainCharacter& source
      ) : Match(source),
         _character(0)
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
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_PlainCharacter>
            (*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_PlainCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
      virtual Char character() const
      {
         return _character;
      }
      
   };
   
   const MatchPtr PlainCharacter =
      _PlainCharacter();
      
   const Match HexCharacter =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   class _Hex :
      public Match,
      public _Character
   {
   private:
      BString _hex;
      Char _character;
   protected:
      MatchPtr createMatch()
      {
         return Capture(
            Repeat(HexCharacter, 4, 4),
            _hex
         );
      }
      
   public:
      _Hex() : Match()
      {
         setMatch(
            createMatch()
         );
      }
      
      _Hex(const _Hex& source) :
         Match(source)
      {
         setMatch(
            createMatch()
         );
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
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_Hex>(*this);
      }
      
      virtual Char character() const
      {
         return _character;
      }
      
   };
      
   const MatchPtr Hex = _Hex();
   
   class _EscapedCharacter :
      public _Character,
      public Match
      
   {
   private:
      _Hex* _hex = nullptr;
      
   protected:
      Char _character;
     
      MatchPtr captureCharacter(
         Char input,
         Char output
      )
      {
         return
            Invoke(
               Character(input),
               [this, output](MatchPtr)
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
         Match(source)
      {
      }
      
      virtual void setup()
      {
         
         MatchPtr InvokeHex = new
            Invoke(
               _hex = new _Hex(),
               [this](MatchPtr)
               {
                  _character = 
                     _hex->character();
               }
            );
            
         MatchPtr match =
            Character('\\') and
            (
               captureCharacter('\\', '\\') or
               captureCharacter('b', '\b') or
               captureCharacter('f', '\f') or
               captureCharacter('r', '\r') or
               captureCharacter('n', '\n') or
               captureCharacter('t', '\t') or
               captureCharacter('\"', '\"') or
               (
                  Character('u') and InvokeHex
               )
            );
         
         setMatch(match);
      }
      
      virtual Char character() const
      {
         return _character;
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_EscapedCharacter>
            (*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_EscapedCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
   };
    
   const Match EscapedCharacter = 
      _EscapedCharacter();
      
   class _StringCharacter :
      public _Character,
      public Match
   {
   protected:
   
      MatchPtr
         _plainCharacter;
         
      MatchPtr
         _escapedCharacter;
      
      
   public:
      _StringCharacter()
      {
      }
      
      _StringCharacter(
         const _StringCharacter& source
      ) :
         Match(source)
      {
      }
      
      virtual void setup()
      {
         MatchPtr match =
            ( _plainCharacter =
              new _PlainCharacter() )
             or
            ( _escapedCharacter =
              new _EscapedCharacter() );
              
         setMatch(match);
      }
      
      virtual MatchPtrBase copy() const
      {
         return
            make_shared<_StringCharacter>
            (*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_StringCharacter";
         writeResult(out);
         out << "(";
         if (matched())
            out << (int)character();
         out << ")";
      }
      
      virtual Char character() const
      {
         MatchPtr which;
         
         if (_plainCharacter->matched())
            which = _plainCharacter;
         else if (
            _escapedCharacter->matched())
            which =_escapedCharacter;
         else
            throw runtime_error("No character match");
            
         shared_ptr<_Character> pointer =
            dynamic_pointer_cast<_Character>
            (which);
            
         return pointer->character();
      }
      
   };
         
   const MatchPtr StringCharacter =
      _StringCharacter();
   
   class _StringCharacters :
      public Repeat
   {
   public:
      BString _value;
      
   public:
      _StringCharacters() :
         Repeat(
            _StringCharacter(),
            0, 0
         )
      {
      }
      
      _StringCharacters(
         const _StringCharacters& source
      ) :
         Repeat(
            _StringCharacter(),
            0, 0
         )
      {
      }
      
      virtual void matchedItem(MatchPtr match)
      {
         shared_ptr<_StringCharacter> pointer =
            static_pointer_cast<_StringCharacter>
            (match);
            
         Char character = pointer->character();
         
         if ( _capture &&
              character != BString::EndOfFile )
            _value.push_back(character);
     
         Repeat::matchedItem(match);
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_StringCharacters>(*this);
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
      
      virtual BString value() const
      {
         return _value;
      }
      
      virtual void capture()
      {
      }
      
      virtual void write(ostream& out) const
      {
         out << "StringCharacters(\"";
         BString::writeEscaped(out, _value);
         out << "\")";
      }
      
   };
   
   class _String :
      public Match
   {
   protected:
      shared_ptr<_StringCharacters>
         _stringCharacters;
      
   public:
      _String()
      {
      }
      
      _String(const _String& source)
      {
      }

      virtual void setup()
      {
         _capture = true;
         
         _stringCharacters =
            make_shared<_StringCharacters>();
            
         _stringCharacters->_capture =
            this->_capture;
            
         MatchPtr match =
            Quote->copy() and
            MatchPtr(_stringCharacters) and
            Quote->copy();
            
         setMatch(match);
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_String>(*this);
      }
      
      virtual BString value() const
      {
         return _stringCharacters->_value;
      }
      
      virtual void write(ostream& out) const
      {
         out << "String";
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
   
   const MatchPtr String =
      Label("String", _String());
   
}

#endif