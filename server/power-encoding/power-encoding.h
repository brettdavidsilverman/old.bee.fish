#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
//#include <tgmath.h>
#include <math.h>

using namespace std;

inline void CHECK(bool boolean)
{
   if (boolean == false)
      throw runtime_error("Check failed");
}

namespace BeeFishPowerEncoding
{
   class PowerEncoding
   {
   public:
      virtual void writeBit(bool bit)
      {
         throw logic_error("Not implemented");
      };

      virtual bool readBit()
      {
         throw logic_error("Not implemented");
      };
   
      virtual bool peekBit()
      {
         throw logic_error("Not implemented");
      }

      
   public:
      PowerEncoding()
      {
      }
     
      template<typename T>
      PowerEncoding& operator <<
      (const T& value)
      {
      
         if (value == 0)
            writeBit(0);
         else
         {
            writeBit(1);
            
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
      PowerEncoding& operator >>
      (T& value)
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
   };
   
}

#endif