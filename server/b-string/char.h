#ifndef BEE_FISH_B_STRING__CHAR_H
#define BEE_FISH_B_STRING__CHAR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <openssl/md5.h>
#include "utf-8.h"
#include "../power-encoding/power-encoding.h"

using namespace bee::fish::power_encoding;

namespace bee::fish::b_string {

   class Char
   {
   public:
      typedef UTF8Character::Value Value;
      Value _character;
      
      Char()
      {
      }
      /*
      Char(const Char& source) :
         vector<bool>(source)
      {
      }
      */
      Char(Value character)
      {
         _character = character;
      }
      /*
      static Char fromHex(const string& hex)
      {
         Char result;
         
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
      virtual ~Char()
      {
      }
      
      operator Value () const
      {
         return _character;
         /*
         class Decoder : public PowerEncoding
         {
         private:
            const Char& _charBits;
            Char::const_iterator _it;
         public:
            Decoder(const Char& charBits) :
               _charBits(charBits),
               _it(_charBits.begin())
            {
            }
            
            virtual void writeBit(bool bit) {};
      
            virtual bool readBit()
            {
               return *(_it++);
            };
      
            virtual bool peekBit()
            {
               return *_it;
            }
         };
         
         Decoder decoder(*this);
         Value character;
         decoder >> character;
         return character;
         */
      }
      /*
      virtual bool operator == (const Char& rhs)
      {
         return vector<bool>::operator == (rhs);
      }
      
      virtual bool operator == (char rhs)
      {
         Value character = *this;
         return (character == rhs);
      }
      */
      
     friend PowerEncoding& 
     operator <<
     (PowerEncoding& encoding, const Char& character)
     {

        encoding << character._character;
      /*
      for (bool bit : character)
      {
         writeBit(bit);
      }
      */
         return encoding;
     }
      
     friend PowerEncoding&
     operator >>
     (PowerEncoding& encoding, Char& character)
     {
        encoding >> character._character;
      
      /*character.clear();
      size_t count = 1;
      bool bit;
      while (count > 0)
      {
         bit = readBit();
         
         if (bit)
            ++count;
         else
            --count;
            
         character.push_back(bit);
      }
      */
         return encoding;
      }
      
      Char toLower() const
      {
         Value character = *this;
         Value lower = tolower(character);
         return Char(lower);
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
         case UTF8Character::EndOfFile:
            out << "{-1}";
            break;
         default:
            if (character <= 0x001F)
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << (Char::Value)character;
            else if (character > 0x10FFFF)
            {
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << (Char::Value)
                   ((character & 0xFFFF0000) >>
                       15);
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << (Char::Value)
                   (character & 0x0000FFFF);
            }
            else
               UTF8Character::write(out, character);
         }
         
        // out.flags( f );
      }

      virtual void write(ostream& out) const
      {
         Value character = *this;
         UTF8Character::write(out, character);
      }
      
      friend ostream& operator <<
      (ostream& out, const Char& character)
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
      
      Char& joinSurrogatePair(
         const Value second
      )
      {
         Value first = *this;
         
         Value character = 0x10000;
         
         character += (first & 0x03FF) << 10;
         character += (second & 0x03FF);
         
        // this->clear();
         _character = character;
         
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
      
   };
   /*
   
   inline double log2(const Char& character)
   {
      return log2(character._character);
   }
   */
}

#endif


