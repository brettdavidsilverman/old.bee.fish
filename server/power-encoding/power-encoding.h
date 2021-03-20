#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>
#include "../b-string/b-string.h"
#include "../parser/parser.h"

using namespace std;
using namespace bee::fish::b_string;
using namespace bee::fish::parser;


namespace bee::fish::power_encoding
{
   class PowerEncoding
   {
   public:
      virtual void writeBit(bool bit)
      {
         cout << (bit ? '1' : '0');
      }
      
      virtual bool readBit()
      {
         char c;
         cin >> c;
         return (c != '0');
      }
      
      virtual bool peekBit()
      {
         char c = (char)(cin.peek());
         return (c != '0');
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
         if (bit == false)
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
      operator << (const BString& str)
      {
         writeBit(true);
         
         for (const Char& c : str)
         {
            (*this) << c;
         }
         
         writeBit(false);
         
         return *this;
      }
      
      PowerEncoding& operator >>
      (BString& value)
      {
         
         bool bit = readBit();
         if (!bit)
            throw runtime_error("Expected '1' bit");
         
         Char c;
         
         while (peekBit())
         {
            (*this) >> c;
            value.push_back(c);
         }
         
         bit = readBit();
         
         if (bit)
            throw runtime_error("Expected '0' bit");
         
         return *this;
         
      }
      
      PowerEncoding&
      operator << (const char* str)
      {
         return operator << (BString(str));
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