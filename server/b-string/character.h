#ifndef BEE_FISH_B_STRING__CHARACTER_H
#define BEE_FISH_B_STRING__CHARACTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <vector>
#include <openssl/md5.h>
#include "../power-encoding/power-encoding.h"
#warning "deprecated"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::b_string {


   class Character
   {
   protected:
         
   public:
      typedef wchar_t Value;
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
         return
           ( _value == rhs._value );
         
      }
      
      bool operator == (char rhs)
      {
         return (
            _value == (Value)rhs
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
            else //if (character > 0x10FFFF)
            {
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   << 
                   ((character & 0xFFFF0000) >>
                       16);
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill('0')
                   <<
                   (character & 0x0000FFFF);
            }

         }
         
      }

      virtual void write(ostream& out) const
      {
         out << _value;
      }
      
      friend ostream& operator <<
      (ostream& out, const Character& character)
      {
         character.write(out);
         
         return out;
      }

   };

}

#endif


