#ifndef BEE_FISH_JSON__UTF_8_H
#define BEE_FISH_JSON__UTF_8_H

#include <bitset>
#include "../parser/parser.h"

namespace bee::fish::json {
   
   typedef wchar_t WideChar;
   
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
   
   class WideCharacter
   {
   private:
      WideChar _character;
      
   public:
      WideChar& _characterRef;
      
   public:
      WideCharacter(WideChar& characterRef) :
         _characterRef(characterRef)
      {
      }
      
      WideCharacter() :
         _characterRef(_character)
      {
      }
      
      WideCharacter(
         const WideCharacter& source
      ) :
         _characterRef(source._characterRef)
      {
      }
      
      WideChar& character()
      {
         return _characterRef;
      }
      
      const WideChar& character() const
      {
         return _characterRef;
      }
   };
   
   class _UTF8Character :
      public Match,
      public WideCharacter
   {
   protected:
      unsigned int    _firstByteCount;
      unsigned int    _byteCount;

   public:
   
      _UTF8Character()
      {
         _firstByteCount = 0;
         _byteCount      = 0;
      }
       
      _UTF8Character(const _UTF8Character& source) :
         WideCharacter(source)
      {
         _firstByteCount = source._firstByteCount;
         _byteCount      = source._byteCount;
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
            // Check if we've read enough bytes.
            if ( ++_byteCount ==
                   _firstByteCount )
            {
               // We have read the first and
               // subsequent bytes... Success!
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
               character() = (
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
            character() =
               (character() << 6) |
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
      
      virtual bool escape() const
      {
         return false;
      }
      
      virtual Match* copy() const
      {
         return new _UTF8Character(*this);
      }
      
      
   };
   
   const Match
      UTF8Character = _UTF8Character();
      
}

#endif