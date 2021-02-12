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
      const UTF8Byte* _first;
      vector<bitset<8> > _subsequent;
      unsigned int _byteCount = 0;
      
   public:
      UTF8Character()
      {
         _first = NULL;
      }
       
      virtual bool match(int character)
      {
         bitset<8> bits(character);
         bool matched = false;
         
         if (_first == NULL)
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
                   _first->_byteCount )
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
            const UTF8Byte* byte = &(*it);
            if (byte->match(bits))
            {
               _first = byte;
               return true;
            }
               
         }
         
         return false;
      }
      
      bool matchSubsequent(const bitset<8> bits)
      {
         if (UTF8Subsequent.match(bits))
         {
            _subsequent.push_back(bits);
            return true;
         }
         
         return false;

      }
      
      
      virtual Match* copy() const
      {
         return new UTF8Character();
      }
   };
   
   const Match UTF8String = Repeat(UTF8Character());

   /*

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