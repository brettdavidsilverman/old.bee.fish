#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <tgmath.h>
#include <math.h>

using namespace std;

namespace bee::fish::power_encoding
{
   class PowerEncoding : public ostream
   {
   public:
      PowerEncoding(ostream& out) :
         ostream(out.rdbuf())
      {
      }
      
      friend PowerEncoding&
      operator << (PowerEncoding& out, const std::string& str)
      {
         (ostream&)out << '1';
         
         for (const char c : str)
            out << c;
         
         (ostream&)out << '0';
         
         return out;
      }
      
      friend PowerEncoding&
      operator << (PowerEncoding& out, const char* str)
      {
         (ostream&)out << '1';
         for (const char* pc = str;
              *pc != 0;
              ++pc)
            out << *pc;
        
         (ostream&)out << '0';
         
         return out;
      }
      
      template<typename T>
      PowerEncoding& 
      operator << (const T value)
      {
         if (value == 0)
            (ostream&)*this << '0';
        // else if (value == 1)
        //    (ostream&)*this << '1' << '0';
         else
         {
            (ostream&)*this << '1';
            T _power     = power(value);
            T _remainder = remainder(value, _power);
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
        // cout << "p" << power;
         return power;
      }
      
      template<typename T>
      T remainder(T value, T power)
      {
         T remainder =
            value - exp2(power);
         //cout << "r" << remainder;
         return remainder;
      }
      
      
   };
}

#endif