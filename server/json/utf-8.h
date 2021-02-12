#ifndef BEE_FISH_JSON__UTF_8_H
#define BEE_FISH_JSON__UTF_8_H

#include <bitset>
#include "../parser/parser.h"

namespace bee::fish::json {
   
   class UTF8Byte : public Match
   {
   protected:
      bitset<8> _matchMask;
      bitset<8> _extractMask;
      bitset<32> _char;
      
   public:
      UTF8Byte(
         bitset<8> matchMask,
         bitset<8> extractMask
      ) :
         _matchMask(matchMask),
         _extractMask(extractMask)
      {
      }
      
      UTF8Byte(
         const UTF8Byte& source
      ) :
         _matchMask(source._matchMask),
         _extractMask(source._extractMask),
         _char(source._char)
      {
      }
      
      virtual bool match(int character)
      {
         bool matched;
         
         bitset<8> bits(character);
         
         if (character == Match::EndOfFile)
            matched = false;
         else
         {
            matched =
               ((bits & _matchMask) == bits);
         }
         
         if (matched)
         {
            _char = bits.to_ulong();
            _char &= _extractMask.to_ulong();
            success();
         }
         else
         {
            fail();
         }
         
         return matched;
      }
      
      virtual Match* copy() const
      {
         return new UTF8Byte(*this);
      }
      
      virtual void write(ostream& out)
      {
         out << "UTF8Byte";
         
         writeResult(out);
         
         out << "()";
      }
      
      wchar_t character()
      {
         return _char.to_ulong();
      }
      
   };
   
   const UTF8Byte UTF8Subsequent = 
      UTF8Byte(
         0b10111111,
         0b00111111
      );

   class UTF8Character : public Match
   {
   protected:
      const UTF8Byte _first;
      unsigned int _byteCount;
      
   public:
       UTF8Character(
          const UTF8Byte& first,
          unsigned int byteCount
       ) :
          Match(
             first and
             Repeat(
                UTF8Subsequent,
                byteCount - 1,
                byteCount - 1
             )
          ),
          _first(first),
          _byteCount(byteCount)
      {
      }
       
      UTF8Character(const UTF8Character& source) :
         UTF8Character(
            source._first,
            source._byteCount
         )
      {
      }
      
      virtual Match* copy() const
      {
         return new UTF8Character(*this);
      }
   };
   
   const UTF8Byte UTF8Character_1 = 
      UTF8Byte(
         0b01111111,
         0b01111111
      );
      
   const UTF8Character UTF8Character_2 = 
      UTF8Character(
         UTF8Byte(
            0b11011111,
            0b00011111
         ),
         2
      );

   const UTF8Character UTF8Character_3 = 
      UTF8Character(
         UTF8Byte(
            0b11101111,
            0b00001111
         ),
         3
      );

   const UTF8Character UTF8Character_4 = 
      UTF8Character(
         UTF8Byte(
            0b11110111,
            0b00000111
         ),
         4
      );

   const Match UTF8Character =
      UTF8Character_1 or
      UTF8Character_2 or
      UTF8Character_3 or
      UTF8Character_4;
      
   const Match UTF8String = Repeat(UTF8Character);
   
      /*
   const Match PlainCharacter =
      not (
         Character('\\') or
         Quote or
         Character('\r') or
         Character('\n') or
         Character(Match::EndOfFile)
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
         
   const Match String = Label(
      "String",
      Quote and
      Repeat(StringCharacter, 0) and
      Quote
   );
   */
}

#endif