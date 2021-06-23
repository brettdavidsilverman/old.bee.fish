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


   class Character
   {
   protected:
         
   public:
      typedef UTF8Character::Value Value;
      Value _value = 0;
      
      Character()
      {
      }
      
      Character(const Character& source) :
         _value(source._value)
      {

      }
      
      Character(Value value) :
         _value(value)
      {
      }
 
      virtual ~Character()
      {
      }

      operator Value () const
      {
         return _value;
      }
       
      bool operator == (
         const Character& rhs
      )
      {
         const Character& lhs = *this;
         
         bool result = 
           ( lhs._value == rhs._value );
         
         return result;
      }
      
      bool operator == (char rhs)
      {
         return (
            _value == (UTF8Character::Value)rhs
         );
      }

      friend PowerEncoding& 
      operator <<
      (
         PowerEncoding& encoding,
         const Character& character
      )
      {
         encoding.writeBit(true);
         encoding << character._value;
         return encoding;
      }
      
      friend PowerEncoding&
      operator >>
      (
         PowerEncoding& encoding,
         Character& character
      )
      {
         CHECK( encoding.readBit() == true );
         
         Value value;
         encoding >> value;
         
         character._value = value;
         
         return encoding;
      }
      
      Character toLower() const
      {
         Value lower = tolower(_value);
         return Character(lower);
      }
      

      void writeEscaped(
         ostream& out
      ) const
      {
        // std::ios_base::fmtflags f( out.flags() );
         Value character = _value;
 
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
         
         _value = value;

         return *this;
      }
     
      
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

}

#endif


