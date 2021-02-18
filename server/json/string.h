#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>       // std::cout, std::hex

#include "../parser/parser.h"
#include "utf-8.h"

namespace bee::fish::json {

   const Match Quote = Label(
      "Quote",
      Character('\"')
   );

   class _PlainCharacter : public _UTF8Character
   {
   public:
      _PlainCharacter()
      {
      }
      
      _PlainCharacter(const _PlainCharacter& source) :
         _UTF8Character(source)
      {
      }
      
      virtual bool match(int character)
      {
         if (exclude(character))
         {
            fail();
            return false;
         }
         
         return
            _UTF8Character::match(character);
      }
      
      virtual bool exclude(int character) const
      {
         switch (character)
         {
         case L'\\':
         case L'\"':
         case L'\r':
         case L'\n':
         case L'\b':
         case L'\f':
         case ((WideChar)Match::EndOfFile):
            return true;
         default:
            return false;
         }
      }
     
      virtual Match* copy() const
      {
         return new _PlainCharacter(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_PlainCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
   };
   
   const Match PlainCharacter =
      _PlainCharacter();
      
   const Match HexChar =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   class _HexBytes : public Match
   {
   protected:
      stringstream _stream;
      uint16_t _value;
      
      Match createMatch()
      {
         return
            Invoke(
               Repeat(HexChar, 4, 4),
               [this](Invoke& invoke)
               {
                  _stream << invoke.value();
               }
            );
      }
      
   public:
      uint16_t& _valueRef;
      
   public:
      _HexBytes(uint16_t &valueRef) :
         Match(createMatch()),
         _valueRef(valueRef)
      {
         _stream << std::hex;
      }
      
      _HexBytes() :
         Match(createMatch()),
         _valueRef(_value)
      {
         _stream << std::hex;
      }
      
      _HexBytes(const _HexBytes& source) :
         Match(createMatch()),
         _valueRef(source._valueRef)
      {
         _stream << std::hex;
      }
      
      virtual void success()
      {
         _stream >> _valueRef;
         Match::success();
         
      }
      
      virtual uint16_t& value()
      {
         return _valueRef;
      }
      
      virtual Match* copy() const
      {
         return new _HexBytes(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_HexBytes";
         writeResult(out);
         out << "(\"" << _stream.str() << "\")";
      }
      
      
      
   };
      
   const Match HexBytes = _HexBytes();
   
   class _EscapedCharacter :
      public Match,
      public WideCharacter
   {
   protected:
   
      Match captureCharacter(char match, WideChar capture)
      {
         return
            Invoke(
               Character(match),
               [this, capture](Invoke&)
               {
                  character() = capture;
               }
            );
      }
      
      Match createMatch()
      {
         return
            Character('\\') and (
               captureCharacter('\\', '\\') or
               captureCharacter('b', '\b') or
               captureCharacter('f', '\f') or
               captureCharacter('r', '\r') or
               captureCharacter('n', '\n') or
               captureCharacter('t', '\t') or
               captureCharacter('\"', '\"') or
               (
                  Character('u') and
                  Invoke(
                     _HexBytes(),
                     [this](Invoke& invoke)
                     {
                        Match& item =
                           invoke.item();
                        _HexBytes& hexBytes = (_HexBytes&)item;
                        cerr << "$$$$" << hexBytes.value();
                       // _character = hex._value;
                     }
                  )
               )
            );
      }
      
      
   public:
      _EscapedCharacter() :
         Match(createMatch())
      {
      }
      
      _EscapedCharacter(
         const _EscapedCharacter& source
      ) :
         Match(createMatch()),
         WideCharacter(source)
      {
      }
      
      virtual Match* copy() const
      {
         return new _EscapedCharacter(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_EscapedCharacter";
         writeResult(out);
         out << "(";
         out << match();
         out << ")";
      }
      
   };
    
   const Match EscapedCharacter = 
      _EscapedCharacter();
      
   class _StringCharacter :
      public Match,
      public WideCharacter
   {
   protected:
      Match captureCharacter(
         const Match& match
      )
      {
         return Invoke(match,
            [this](Invoke& invoke)
            {
               Match& item =
                  invoke.item();
               /*
               cerr << "****" << item << endl;
               
               WideCharacter& witem =
                  (WideCharacter&)(item);
               _character = witem._character;
               cerr << (int)_character;
               */
            }
         );
      }
      
      Match createMatch()
      {
         return
            captureCharacter(
               _PlainCharacter()
            ) or
            captureCharacter(
               _EscapedCharacter()
            );
      }
      
   public:
      _StringCharacter() :
         Match(createMatch())
      {
      }
      
      _StringCharacter(
         const _StringCharacter& source
      ) :
         Match(createMatch()),
         WideCharacter(source)
      {
      }
      
      virtual Match* copy() const
      {
         return new _StringCharacter(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "_StringCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
   };
         
   const Match StringCharacter =
      _StringCharacter();
   
   class _String : public Match
   {
   protected:
      wstring _wstring;
      
   public:
      _String() : Match(
         Quote and
         Repeat(
            Invoke(
               _StringCharacter(),
               [this](Invoke& invoke)
               {
               /*
                  Match& strchar =
                     invoke.item();
                  WideCharacter& wchar =
                     (WideCharacter&)(invoke.item().item().item());
                  _wstring += wchar.character();
               */
               }
            ),
            0
         ) and
         Quote
      )
      {
      }
      
      _String(const _String& source) :
         _String()
      {
         _wstring = source._wstring;
      }
      
      virtual wstring& value()
      {
         return _wstring;
      }
      
      virtual Match* copy() const
      {
         return new _String(*this);
      }
   };
   
   const Match String =
      Label("String", _String());
   
}

#endif