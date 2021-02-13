#ifndef BEE_FISH_JSON__UTF_8_H
#define BEE_FISH_JSON__UTF_8_H

#include <bitset>
#include "../parser/parser.h"

namespace bee::fish::json {
   
   struct UTF8Byte
   {
      bitset<8>  _matchMask;
      bitset<8>  _extractMask;
      size_t     _byteCount;
      
      UTF8Byte(
         bitset<8> matchMask,
         bitset<8> extractMask,
         size_t    byteCount
      ) :
         _matchMask(matchMask),
         _extractMask(extractMask),
         _byteCount(byteCount)
      {
      }
      
      UTF8Byte(
         const UTF8Byte& source
      ) :
         _matchMask(source._matchMask),
         _extractMask(source._extractMask),
         _byteCount(source._byteCount)
      {
      }
      
      bool match(const bitset<8>& bits) const
      {
         bool matched =
            ((bits & _matchMask) == bits);
         
         return matched;
      }
      
      
   };
   
   const vector<UTF8Byte> FirstByte{
      UTF8Byte(
         0b01111111,
         0b01111111,
         1
      ),
      UTF8Byte(
         0b11011111,
         0b00011111,
         2
      ),
      UTF8Byte(
         0b11101111,
         0b00001111,
         3
      ),
      UTF8Byte(
         0b11110111,
         0b00000111,
         4
      )
   };
   
   const UTF8Byte UTF8Subsequent = 
      UTF8Byte(
         0b10111111,
         0b00111111,
         1
      );
   
   class UTF8Character : public Match
   {
   protected:
      unsigned int    _firstByteCount;
      unsigned int    _byteCount;
      wchar_t         _character;
      unsigned int    _count  = 0;
   public:
   
      UTF8Character()
      {
         _firstByteCount = 0;
         _byteCount      = 0;
         _character      = 0;
      }
       
      UTF8Character(const UTF8Character& source)
      {
         _firstByteCount = source._firstByteCount;
         _byteCount      = source._byteCount;
         _character      = source._character;
      }
      
      virtual bool match(int character)
      {
         bitset<8> bits(character);
         bool matched = false;
         
         if (_byteCount == 0)
         {
            matched = matchFirstByte(bits);
         }
         else
         {
            matched = matchSubsequent(bits);
         }
         
         if (matched)
         {
            if ( ++_byteCount ==
                   _firstByteCount )
            {
               success();
            }
            
         }
         else
            fail();
            
         return matched;

         
      }
      
      bool matchFirstByte(const bitset<8>& bits)
      {
         // Match first byte
         for (auto it = FirstByte.cbegin();
                   it != FirstByte.cend();
                 ++it )
         {
            // Get the byte.
            const UTF8Byte& byte = *it;
            
            // See if its a match.
            if (byte.match(bits))
            {
               // Store how many subsequent bytes
               // (including this one).
               _firstByteCount = byte._byteCount;
               
               // Start the character value
               // using the bytes extract mask.
               _character = (
                  bits & byte._extractMask
               ).to_ulong();
               
               return true;
            }
               
         }
         
         return false;
      }
      
      bool matchSubsequent(const bitset<8> bits)
      {
         // Match subsequent bytes.
         
         // Check the byte is a subsequent
         // byte.
         if (UTF8Subsequent.match(bits))
         {
            // Left shift the 6 usable bits
            // onto the character value.
            _character =
               (_character << 6) |
               (
                  bits &
                     UTF8Subsequent._extractMask
               ).to_ulong();
               
            // We matched.
            return true;
         }
         
         // We didn't
         return false;

      }
      
      wchar_t character()
      {
         return _character;
      }
      
      virtual Match* copy() const
      {
         return new UTF8Character();
      }
      
      virtual void success()
      {
         Match::success();
         wcout << character();
      }
      
   };
   
   const Match UTF8String = Repeat(UTF8Character());

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