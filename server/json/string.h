#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>       // std::cout, std::hex
#include <fstream>

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
      
      _PlainCharacter(WideChar& wideChar) :
         _UTF8Character(wideChar)
      {
      }
      
      virtual bool ignore(int character) const
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
     
      virtual bool match(int character)
      {
         if (ignore(character))
         {
            fail();
            return false;
         }
         
         bool matched =
            _UTF8Character::match(character);
            
         return matched;
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
      
   const Match HexCharacter =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   class _Hex :
      public Match,
      public WideCharacter
   {
   private:
      string _hex;
   protected:
      
      Match createMatch()
      {
         return
            Capture(
               Repeat(HexCharacter, 4, 4),
               _hex
            );
      }
      
   public:
      _Hex() :
         Match(
            createMatch()
         ),
         WideCharacter()
      {
      }
      
      _Hex(const WideCharacter& wideCharacter) :
         Match(
            createMatch()
         ),
         WideCharacter(wideCharacter)
      {
      }
      
      _Hex(const _Hex& source) :
         Match(
            createMatch()
         ),
         WideCharacter(source),
         _hex(source._hex)
      {
      }
      
      virtual void success()
      {
         std::stringstream stream;
         stream << std::hex << _hex;
         uint16_t u16;
         stream >> u16;
         character() = u16;
         Match::success();
         
      }
      
      virtual Match* copy() const
      {
         return new _Hex(*this);
      }
      
      
   };
      
   const Match Hex = _Hex();
   
   class _EscapedCharacter :
      public WideCharacter,
      public Match
      
   {
   protected:
   
      Match captureCharacter(char match, WideChar wchar)
      {
         return
            Invoke(
               Character(match),
               [this, wchar](Capture&)
               {
                  character() = wchar;
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
               _Hex(*this)
            )
         );
      }
      
      
   public:
      _EscapedCharacter() :
         WideCharacter(),
         Match(createMatch())
      {
      }
      
      _EscapedCharacter(WideChar& wideChar) :
         WideCharacter(wideChar),
         Match(createMatch())
      {
      }
      
      _EscapedCharacter(
         const _EscapedCharacter& source
      ) :
         WideCharacter(source),
         Match(createMatch())
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
      public WideCharacter,
      public Match
   {
   protected:
      
      Match createMatch()
      {
         return
            _PlainCharacter(character()) or
            _EscapedCharacter(character());
      }
      
   public:
      _StringCharacter() :
         WideCharacter(),
         Match(createMatch())
      {
      }
      
      _StringCharacter(WideChar& wideChar) :
         WideCharacter(wideChar),
         Match(createMatch())
      {
      }
      
      _StringCharacter(
         const _StringCharacter& source
      ) :
         WideCharacter(source),
         Match(createMatch())
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
   
   class _StringCharacters :
      public wstringstream,
      protected WideCharacter,
      public Repeat
   {
   protected:
      wostream& _wideStreamRef;
   public:
      _StringCharacters() :
         WideCharacter(),
         Repeat(
            _StringCharacter(character()),
            0, 0
         ),
         _wideStreamRef(*this)
      {
      }
      
      _StringCharacters(wostream& wideStream) :
         WideCharacter(),
         Repeat(
            _StringCharacter(character()),
            0, 0
         ),
         _wideStreamRef(wideStream)
      {
      }
      
      _StringCharacters(
         const _StringCharacters& source
      ) :
         WideCharacter(source),
         Repeat(
            _StringCharacter(character()),
            0,
            0
         ),
         _wideStreamRef(source._wideStreamRef)
      {
      }
      
      virtual void matchedItem(Match* match)
      {
         _wideStreamRef << character();
         Repeat::matchedItem(match);
      }
      
      virtual Match* copy() const
      {
         return new _StringCharacters(*this);
      }
      
   };
   
   class _String :
      public Match,
      public wstringstream
   {
   protected:
      Match _match;
      _StringCharacters _stringCharacters;
      
      virtual Match createMatch()
      {
         Match match =
            Quote and
            _stringCharacters and
            Quote;
            
         return match;
      }
      
   public:
      _String() :
         _stringCharacters(*this)
      {
         _match = createMatch();
      }
      
      _String(const _String&source) :
         _stringCharacters(source._stringCharacters)
      {
         _match = createMatch();
      }
      
      virtual bool match(int character)
      {
         bool matched = _match.match(character);
         
         if (_match.result() == true)
            success();
         else if (_match.result() == false)
            Match::fail();
            
         return matched;
      }
      
      
      virtual Match* copy() const
      {
         return new _String(*this);
      }
      
      const wstring value() const
      {
         return str();
      }
      
   };
   
   const Match String =
      Label("String", _String());
   
}

#endif