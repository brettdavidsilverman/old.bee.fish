#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <tgmath.h>
#include <math.h>

using namespace std;

namespace bee::fish::power_encoding
{
   template<class Out>
   class PowerEncoding : public ostream
   {
   
   public:
      PowerEncoding(Out& out) :
         ostream(out.rdbuf()),
         _out(out)
      {
      }
   protected:
      Out& _out;
   public:
     
      template<typename T>
      PowerEncoding& 
      operator << (const T value)
      {
         if (value == 0)
            _out << '0';
         else
         {
            _out << '1';
            
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

      PowerEncoding&
      operator << ( const std::string& str)
      {
         _out << '1';
         
         for (const char c : str)
            (*this) << c;
         
         _out << '0';
         
         return *this;
      }
      
      PowerEncoding&
      operator << (const char* str)
      {
         return operator<<((string)str);
      }
      
      PowerEncoding&
      operator << (const std::wstring& wstr)
      {
         _out << '1';
         
         for (const wchar_t c : wstr)
            (*this) << c;
         
         _out << '0';
         
         return *this;
      }
     
      PowerEncoding&
      operator << (const wchar_t* wstr)
      {
         return operator<<((wstring)wstr);
      }
      
            
      
   };
   
   class Counter : public ostream
   {
   protected:
      long long _counter = 0;
      
   public:
      
      
      Counter(ostream& out) :
         ostream(out.rdbuf())
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