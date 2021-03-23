#ifndef BEE_FISH_B_STRING__CHAR_H
#define BEE_FISH_B_STRING__CHAR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <openssl/md5.h>
#include "utf-8.h"

namespace bee::fish::b_string {

   class Char
   {
   public:
      typedef UTF8Character::Value Value;
      inline static const size_t BitCount = 32;
      inline static const size_t BytesPerChar = 4;
      
      Value _character;
   public:
      Char() :
         _character(0)
      {
      }
      
      Char(const Char& source) :
         _character(source._character)
      {
      }
      
      Char(Value character) :
         _character(character)
      {
      }
      
      static Char fromHex(const string& hex)
      {
         Char result;
         
         std::stringstream stream;
         stream << std::hex << hex;
         stream >> result._character;
         
         return result;
      }
      
      virtual ~Char()
      {
      }
      
      operator Value () const
      {
         return _character;
      }
      
      virtual bool operator == (const Char& rhs)
      {
         return (_character == rhs._character);
      }
      
      virtual bool operator == (char rhs)
      {
         return (_character == (Char::Value)rhs);
      }
      
      Char toLower() const
      {
         return tolower(_character);
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
      
      void writeEscaped(
         ostream& out
      ) const
      {
         std::ios_base::fmtflags f( out.flags() );
         
         switch (_character)
         {
         //case '\"':
         //   out << "\\\"";
         //   break;
         //case '\\':
         //   out << "\\\\";
         //   break;
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
            UTF8Character::write(out, _character);
         }
         
         cout.flags( f );
      }

      virtual void write(ostream& out) const
      {
         UTF8Character::write(out, _character);
      }
      
      friend ostream& operator <<
      (ostream& out, const Char& character)
      {
         character.write(out);
         
         return out;
      }
      
      
      // https://unicodebook.readthedocs.io/unicode_encodings.html#surrogates
      bool isSurrogatePair(
         const Char& second
      )
      {
         const Char& first = *this;
         
         return ( ( 0xD800 <= first && 
                    first <= 0xDBFF ) &&
                  ( 0xDC00 <= second &&
                    second <= 0xDFFF) );
      }
      
      Char& joinSurrogatePair(
         const Char& second
      )
      {
         Value first = _character;
         
         Value character = 0x10000;
         
         character += (first & 0x03FF) << 10;
         character += (second & 0x03FF);
         
         _character = character;
         
         return *this;
      }
      

      
   };
   
   
   inline double log2(const Char& character)
   {
      return log2(character._character);
   }
   
}

#endif


