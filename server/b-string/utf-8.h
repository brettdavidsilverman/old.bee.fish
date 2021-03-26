#ifndef BEE_FISH_B_STRING__UTF_8_H
#define BEE_FISH_B_STRING__UTF_8_H

#include <vector>
#include <bitset>
#include <wchar.h>
#include <optional>

using namespace std;

namespace bee::fish::b_string {
;
   
   struct UTF8Character
   {
   protected:
      unsigned int _expectedByteCount = 0;
      unsigned int _byteCount = 0;
      
   public:
      typedef uint32_t Value;
      
      inline static const Value EndOfFile = -1;
   
      Value          _character = 0;
      optional<bool> _result = nullopt;
      
      UTF8Character()
      {
         
      }
      
      UTF8Character(
         const UTF8Character& source
      ) :
         _expectedByteCount(source._expectedByteCount),
         _byteCount(source._byteCount),
         _character(source._character),
         _result(nullopt)
      {
      }
      
      void reset()
      {
         _expectedByteCount = 0;
         _byteCount = 0;
         _character = 0;
         _result = nullopt;
      }
      
      bool match(const char& character)
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
                   _expectedByteCount )
            {
               // All bytes match
               _result = true;
               
            }
            
         }
         else
            _result = false;
            
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
               _expectedByteCount = byte._byteCount;
               
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
         UTF8Byte byte = UTF8Subsequent;
         if (byte.match(bits))
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
      
                  
      static bool read(
         istream& input,
         Value& character
      )
      {
         int nextChar;
         UTF8Character utf8;
         
         while ( !input.eof() )
         {
            nextChar = input.get();
            
            if ((Value)nextChar == EndOfFile)
            {
               character = EndOfFile;
               return true;
            }
               
            utf8.match(nextChar);
            
            if (utf8._result != nullopt)
               break;
         }
      
         if (utf8._result == true)
         {
            character = utf8._character;
            return true;
         }
         else
            return false;
      }
      
      static void write(
         ostream& out,
         const Value& character
      )
      {
         if (character <= 0x007F)
         {
            // 1 byte ascii character
            
            char c1 = (char)character;
            out << c1;
         }
         else if (character <= 0x07FF)
         {
            // 2 byte unicode
            
            //110xxxxx 10xxxxxx
            char c1 = ( 0b00011111         &
                      ( character >> 6 ) ) |
                        0b11000000;
                            
            char c2 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2;
         }
         else if (character <= 0xFFFF)
         {
            // 3 byte unicode
            
            //1110xxxx 10xxxxxx 10xxxxxx
            char c1 = ( 0b00001111          &
                      ( character >> 12 ) ) |
                        0b11100000;
                           
            char c2 = ( 0b00111111          &
                      ( character >>  6 ) ) |
                        0b10000000;
                           
            char c3 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                           
            out << c1 << c2 << c3;

         }
         else if (character <= 0x10FFFF)
         {
            // 4 byte unicode
            
            //11110xxx 10xxxxxx
            //10xxxxxx 10xxxxxx
            char c1 = ( 0b00000111         &
                      ( character >> 18) ) |
                        0b11110000;
                            
            char c2 = ( 0b00111111         &
                      ( character >> 12) ) |
                        0b10000000;
                            
            char c3 = ( 0b00111111         &
                      ( character >>  6) ) |
                        0b10000000;
                            
            char c4 = ( 0b00111111 &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2 << c3 << c4;
         }
         else
         {
            // Invalid character
            out << "{"
                << (uint32_t)character
                << "}";
         }
   
      }
      

      
   protected:
   
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
      
         bool match(const bitset<8>& bits)
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
   

   };
 
}

#endif