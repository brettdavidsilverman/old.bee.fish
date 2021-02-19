#ifndef BEE_FISH_JSON__UTF_8_H
#define BEE_FISH_JSON__UTF_8_H

#include <bitset>
#include "../parser/parser.h"

namespace bee::fish::json {
   
   typedef wchar_t WideChar;
   
   struct UTF8Byte : public Match
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
      
      virtual Match* copy() const
      {
         return new UTF8Byte(*this);
      }
      
      virtual bool match(const bitset<8>& bits)
      {

         bool matched =
            ((bits & _matchMask) == bits);
         
         if (matched)
            success();
         else
            fail();
            
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
   protected:
      WideChar& _characterRef;
      
   public:
      WideCharacter() :
         _character(0),
         _characterRef(_character)
      {
      }
      
      WideCharacter(WideChar& character) :
         _character(-1),
         _characterRef(character)
      {
      }
      
      WideCharacter(
         const WideCharacter& source
      ) :
         _character(-1),
         _characterRef(source._characterRef)
      {
      }
      
      const WideChar& character() const
      {
         return _characterRef;
      }
      
      WideChar& character()
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
       
      _UTF8Character(WideChar& wideChar) :
         WideCharacter(wideChar)
      {
         _firstByteCount = 0;
         _byteCount      = 0;
      }
      
      _UTF8Character(
         const _UTF8Character& source
      ) :
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
               // All bytes match
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
            UTF8Byte byte = *it;
            
            // See if its a match.
            if (byte.match(bits))
            {
               // Store how many subsequent bytes
               // (including this one).
               _firstByteCount = byte._byteCount;
               
               // Start the character value
               // using the bytes extract mask.
             
               WideChar wchar = (
                  bits & byte._extractMask
               ).to_ulong();
               
               character() = wchar;
               
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
         UTF8Byte byte = UTF8Subsequent;
         if (byte.match(bits))
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
      
      virtual Match* copy() const
      {
         return new _UTF8Character(*this);
      }
      
      
   };
   
   const Match
      UTF8Character =
         Label(
            "UTF8Character",
            _UTF8Character()
         );
      
}

#endif