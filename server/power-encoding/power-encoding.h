#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>

#ifndef GEN_CHAR_BITS
#include "char-bits.h"
#endif

using namespace std;

extern vector<string> _charBits;

namespace bee::fish::power_encoding
{
   class PowerEncoding
   {
   protected:
      virtual void writeBit(bool bit) = 0;
      
   public:
      PowerEncoding()
      {
      }
     
      template<typename T>
      PowerEncoding& 
      operator << (const T value)
      {
      
         if (value == 0)
            writeBit(false);
         else
         {
            writeBit(true);
            
            T _power     =
               power(value);
               
            T _remainder =
               remainder(value, _power);
            
            *this << _power;
            *this << _remainder;
         }
         return *this;
      }
     
      template<typename T>
      T power(T value)
      {
         T power =
            floor(log2(value));

         return power;
      }
      
      template<typename T>
      T remainder(T value, T power)
      {
         T remainder =
            value - exp2(power);

         return remainder;
      }

      PowerEncoding& operator <<
      (const char& value)
      {
         writeChar(value);
         return *this;
      }
      
      PowerEncoding& operator <<
      (unsigned char c)
      {
         writeChar(c);
         return *this;
      }
      
      PowerEncoding&
      operator << ( const std::string& str)
      {
      
         writeBit(true);
         
         for (const char& c : str)
         {
            writeChar(c);
         }
         
         writeBit(false);
         
         return *this;
      }
      
      PowerEncoding&
      operator << (const char* str)
      {
         const std::string string(str);
         return operator << (string);
      }
      
      PowerEncoding&
      operator << (const std::wstring& wstr)
      {
         writeBit(true);
         
         for (const wchar_t c : wstr)
            (*this) << c;
         
         writeBit(false);
         
         return *this;
      }
     
      PowerEncoding&
      operator << (const wchar_t* wstr)
      {
         return operator << (wstring(wstr));
      }
      
   private:
      bool writeChar(unsigned char value)
      {
#ifdef GEN_CHAR_BITS
         throw logic_error("Not implemented");
#else
         const std::string& bits =
            _charBits[value];
    
         for (const char& bit : bits)
         {
            writeBit(bit == '1');
         }
#endif   
         return true;
      }
      
   };
   
   class Counter : PowerEncoding
   {
   protected:
      long long _counter = 0;
      
   public:
      
      
      Counter()
      {
      }
      
      Counter& operator << (char c)
      {
         if (c == '0')
            --_counter;
         else if (c == '1')
            ++_counter;
         else
            throw runtime_error("Invalid bit");
            
         (ostream&)(*this) << c;
         
         return *this;
      }
      
      long long counter() const
      {
         return _counter;
      }
      
      static Counter& endl(Counter& os)
      {
         (ostream&)(os) << std::endl;
         os._counter = 0;
         
         return os;
      }
      
      Counter& operator<<(Counter& (*pf)(Counter&))
      {
         return (*pf)(*this);
      }
   };
   
   
}

#endif