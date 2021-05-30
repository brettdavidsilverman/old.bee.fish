#ifndef BEE_FISH_B_STRING__CHARACTER_H
#define BEE_FISH_B_STRING__CHARACTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <vector>
#include <openssl/md5.h>
#include "utf-8.h"
#include "../power-encoding/power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::b_string {

   typedef vector<bool> CharacterBase;
   
   class Character :
      public CharacterBase
   {
   protected:
      
      class Encoder :
         public PowerEncoding
      {
      private:
         Character& _character;
         
      public:
         Encoder(
            Character& character
         ) :
            _character(character)
         {
         }
            
         virtual void writeBit(bool bit)
         {
            _character.push_back(bit);
         };
      
         virtual bool readBit()
         {
            throw logic_error("Not implemented");
         };
      
         virtual bool peekBit()
         {
            throw logic_error("Not implemented");
         }
      };
      
      class Decoder :
         public PowerEncoding
      {
      private:
         const Character& _character;
         Character::const_iterator _it;
      public:
         Decoder(
            const Character& character
         ) :
            _character(character),
            _it(_character.cbegin())
         {
         }
            
         virtual void writeBit(bool bit)
         {
            throw logic_error("Not implemented");
         };
      
         virtual bool readBit()
         {
            if (_it == _character.cend())
               throw runtime_error("Past end of file");
               
            bool bit = *_it;
            ++_it;
            return bit;
         };
      
         virtual bool peekBit()
         {
            return *_it;
         }
      };
         
   public:
      typedef UTF8Character::Value Value;
      
      Character()
      {
      }
      
      Character(const Character& source) :
         CharacterBase(source)
      {

      }
      
      Character(const CharacterBase& source) :
         CharacterBase(source)
      {
      }
      
      Character(Value value)
      {
         Encoder encoder(*this);
         encoder << value;
      }
 
      virtual ~Character()
      {
      }
      
      Character& operator = (const Character& source)
      {
          vector<bool>::operator = (source);
          
          return *this;
      }
      
      operator Value () const
      {
         Decoder decoder(*this);
         Value value;
         decoder >> value;
         return value;
      }
       
      bool operator == (
         const Character& rhs
      )
      {
         const CharacterBase& lhs = *this;
         const CharacterBase& _rhs = rhs;
         
         bool result = ( lhs == _rhs );
         
         return result;
      }
      
      bool operator == (char rhs)
      {
         return (
            (Value)*this == (unsigned char)rhs
         );
      }

      // Character bits are already power
      // encoded, so simply write the bits
      // to the encoding
      friend PowerEncoding& 
      operator <<
      (
         PowerEncoding& encoding,
         const Character& character
      )
      {
         encoding.writeBit(true);
         for (auto bit : character)
         {
            encoding.writeBit(bit);
         }
      
         return encoding;
      }
      
      // Extract bits from encoding,
      // keeping track of +/- count.
      // Finish when count reaches zero.
      friend PowerEncoding&
      operator >>
      (
         PowerEncoding& encoding,
         Character& character
      )
      {
         character.clear();
         
         CHECK( encoding.readBit() == true );
         
         size_t count = 1;
         
         bool bit;
         
         while (count > 0)
         {
            bit = encoding.readBit();
         
            if (bit)
               ++count;
            else
               --count;

            character.push_back(bit);

         }
      
         
         return encoding;
      }
      
      Character toLower() const
      {
         Value character = *this;
         Value lower = tolower(character);
         return Character(lower);
      }
      

      void writeEscaped(
         ostream& out
      ) const
      {
        // std::ios_base::fmtflags f( out.flags() );
         Value character = *this;
 
         switch (character)
         {
         case '\"':
            out << "\\\"";
            break;
         case '\\':
            out << "\\\\";
            break;
         case '\b':
            out << "\\b";
            break;
         case '\f':
            out << "\\f";
            break;
         case '\r':
            out << "\\r";
            break;
         case '\n':
            out << "\\n";
            break;
         case '\t':
            out << "\\t";
            break;
         default:
            if (character <= 0x001F)
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << character;
            else if (character > 0x10FFFF)
            {
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << 
                   ((character & 0xFFFF0000) >>
                       15);
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   <<
                   (character & 0x0000FFFF);
            }
            else
               UTF8Character::write(out, character);
         }
         
        // out.flags( f );
      }

      virtual void write(ostream& out) const
      {
         UTF8Character::write(
            out,
            (Value)(*this)
         );
      }
      
      friend ostream& operator <<
      (ostream& out, const Character& character)
      {
         character.write(out);
         
         return out;
      }
      
      
      // https://unicodebook.readthedocs.io/unicode_encodings.html#surrogates
      bool isSurrogatePair(
         const Value second
      )
      {
         const Value first = *this;
         
         return ( ( 0xD800 <= first && 
                    first <= 0xDBFF ) &&
                  ( 0xDC00 <= second &&
                    second <= 0xDFFF) );
      }
      
      Character& joinSurrogatePair(
         const Value second
      )
      {
         Value first = *this;
         
         Value value = 0x10000;
         
         value += (first & 0x03FF) << 10;
         value += (second & 0x03FF);
         
         Character character = value;
         
         clear();
         
         for ( auto bit : character )
         {
            push_back(bit);
         }

         return *this;
      }
      
     /*
      virtual void writeBit(bool bit)
      {
         push_back(bit);
      };
      
      virtual bool readBit() 
      {
         throw logic_error("Not implemented");
      }
      
      virtual bool peekBit()
      {
         throw logic_error("Not implemented");
      }
      */
      
     /*
      static Character fromHex(const string& hex)
      {
         Character result;
         
         std::stringstream stream;
         stream << std::hex << hex;
         stream >> result._character;
         
         return result;
      }
      string toHex() const
      {
         stringstream stream;
         stream
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << std::uppercase
            << _character;
         return stream.str();
      }
      */

   };
   /*
   
   inline double log2(const Character& character)
   {
      return log2(character._character);
   }
   */
}

#endif


