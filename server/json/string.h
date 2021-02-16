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
      
      virtual bool escape() const
      {
         switch (character())
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
         return new _PlainCharacter();
      }
      
      virtual void write(ostream& out) const
      {
         out << "_PlainCharacter";
         writeResult(out);
         out << "(";
         out << (int)character();
         out << ")";
      }
      
      virtual WideChar character() const
      {
         return WideCharacter::character();
      }
      
   };
   
   const Match PlainCharacter =
      _PlainCharacter();
      
   const Match HexCharacter =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   class _UnicodeHex : public Match
   {
   protected:
      string _hex;
      uint16_t _value;
      
   public:
      _UnicodeHex() :
         Match(
            Character('u') and
            Capture(
               Repeat(HexCharacter, 4, 4),
               _hex
            )
         )
      {
      }
      
      _UnicodeHex(const _UnicodeHex& source) :
         _UnicodeHex()
      {
         _hex = source._hex;
         _value = source._value;
      }
      
      virtual void success()
      {
         std::stringstream stream;
         stream << std::hex << _hex;
         stream >> _value;
         Match::success();
         
      }
      
      virtual Match* copy() const
      {
         return new _UnicodeHex(*this);
      }
      
      uint16_t value()
      {
         return _value;
      }
      
   };
      
   const Match UnicodeHex = _UnicodeHex();
   
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
                  _character = capture;
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
            Invoke(
               _UnicodeHex(),
               [this](Invoke& invoke)
               {
                  _UnicodeHex& hex =
                     (_UnicodeHex&)(invoke.item().item());
                  _character = hex.value();
               }
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
         out << (int)character();
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
               cerr << item << endl;
               
               WideCharacter& witem =
                  (WideCharacter&)(item.item());
               _character = witem.character();
               cerr << (int)witem.character();
              
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