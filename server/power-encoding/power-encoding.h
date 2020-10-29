#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>

using namespace std;

namespace bee::fish::power_encoding
{
   class PowerEncoding
   {
   protected:
      virtual void writeBit(bool bit)
      {
         throw logic_error("Not implemented");
      }
      
      virtual bool readBit()
      {
         throw logic_error("Not implemented");
      }
      
      virtual bool peekBit()
      {
         throw logic_error("Not implemented");
      }
      
   public:
      PowerEncoding()
      {
      }
     
      template<typename T>
      PowerEncoding& 
      operator << (const T& value)
      {
      
         if (value == 0)
            writeBit(false);
         else
         {
            writeBit(true);
            
            auto _power     =
                 power(value);
               
            auto _remainder =
                 remainder(value, _power);

            *this << _power;
            *this << _remainder;
         }
         return *this;
      }
     
      template<typename T>
      PowerEncoding& operator >> (T& value)
      {
         bool bit = readBit();
         if (bit == 0)
         {
            value = T();
            return *this;
         }
            
         unsigned long _power;
         (*this) >> _power;
         
         unsigned long _remainder;
         (*this) >> _remainder;
         
         value = exp2(_power) + _remainder;
         
         return *this;
      }
      
      template<typename T>
      unsigned long power(T value)
      {
         unsigned long power =
            floor(log2(value));

         return power;
      }
      
      template<typename T>
      unsigned long remainder
      (
         T value,
         unsigned long power
      )
      {
         unsigned long remainder =
            value - exp2(power);

         return remainder;
      }

      
      PowerEncoding&
      operator << (const std::string& str)
      {
         writeBit(true);
         
         for (const char& c : str)
         {
            (*this) << c;
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
      
      
      PowerEncoding& operator >>
      (std::string& value)
      {
         
         bool bit = readBit();
         if (!bit)
            throw runtime_error("Expected '1' bit");
         
         char c;
         
         while (peekBit())
         {
            (*this) >> c;
            value += c;
         }
         
         bit = readBit();
         
         if (bit)
            throw runtime_error("Expected '0' bit");
         
         return *this;
         
      }
      
      PowerEncoding&
      operator << (const std::wstring& wstr)
      {
         
         writeBit(true);
         
         for (const wchar_t wc : wstr)
            (*this) << wc;
         
         writeBit(false);
         
         return *this;
      }
     
      PowerEncoding&
      operator << (const wchar_t* wstr)
      {
         return operator << (wstring(wstr));
      }
      
      PowerEncoding& operator >>
      (std::wstring& value)
      {
         bool bit = readBit();
         if (!bit)
            throw runtime_error("Expected '1' bit");
         
         wchar_t wc;
         
         while (peekBit())
         {
            (*this) >> wc;
            value += wc;
         }
         
         bit = readBit();
         
         if (bit)
            throw runtime_error("Expected '0' bit");
         
         return *this;
         
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