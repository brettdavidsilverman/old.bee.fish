#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <vector>
//#include <tgmath.h>
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
  
      virtual void writeBits(string bits)
      {
         for (char bit : bits)
            writeBit(bit != '\0');
      }
      
      
   protected:
   
      template<typename T>
      void writeValue(const T& value)
      {
         if (isNull(value))
            writeBit(false);
         else if (value == 2)
         {
            writeBits("11000");
         }
         else
         {
            writeBit(true);
            
            auto _power     =
                 power(value);
               
            T _remainder =
                 remainder(value, _power);

            writeValue(_power);
            writeValue(_remainder);
         }
      }

      template<typename T>
      void readValue(T& value)
      {
      
         bool bit = readBit();
         
         if (bit == 0)
         {
            value = T();
            return;
         }
            
         unsigned long _power;
         readValue(_power);
         
         T _remainder;
         readValue(_remainder);
         
         value = add(_power, _remainder);
         
      }
      
      // Generic add, power, remainder and isNull
      
      template<typename T>
      unsigned long power(T value)
      {
         auto power =
            floor(log2(value));

         return power;
      }
      
      template<typename T>
      T remainder
      (
         T value,
         unsigned long power
      )
      {
         T remainder =
            value - exp2(power);

         return remainder;
      }
      
      template<typename T>
      T add
      (
         unsigned long power,
         T remainder
      )
      {
         return (T)(exp2(power) + remainder);
      }
      
      template<typename T>
      bool isNull(T value)
      {
         return value == 0;
      }
      
      
      // Char templates
      char remainder
      (
         const char& value,
         unsigned long power
      )
      {
         return (char)
            (
               (unsigned int)value -
               exp2(power)
            );
      }
      
      char add
      (
         unsigned long power,
         char remainder
      )
      {
         char return_value =
            (char)
            (
               exp2(power) +
               (unsigned int)remainder
            );
            
         return return_value;
      }
      
      unsigned long power(char value)
      {
         return floor(log2((unsigned int)value));
      }
      
      bool isNull(char value)
      {
         return (value == '\0');
      }
      
      // wchar_t template
      
      wchar_t remainder
      (
         const wchar_t& value,
         unsigned long power
      )
      {
         return (wchar_t)
            (
               (unsigned int)value -
               exp2(power)
            );
      }
      
      wchar_t add
      (
         unsigned long power,
         wchar_t remainder
      )
      {
         wchar_t return_value =
            (wchar_t)
            (
               exp2(power) +
               (unsigned int)remainder
            );
            
         return return_value;
      }
      
      unsigned long power(wchar_t value)
      {
         return floor(log2((unsigned int)value));
      }
      
      bool isNull(wchar_t value)
      {
         return (value == L'\0');
      }
      
   public:
      PowerEncoding()
      {
      }
     
      template<typename T>
      PowerEncoding& 
      operator << (const T& value)
      {
      
         writeBit(true);
         writeValue(value);
         
         return *this;
      }
      
      template<typename T>
      PowerEncoding& operator >> (T& value)
      {
         bool bit = readBit();
         
         if (!bit)
            throw runtime_error("Expected to read a 1 bit.");
            
         readValue(value);
         return *this;
      }
      
      
      template<typename Array, typename Element>
      PowerEncoding&
      operator << (const Array& array)
      {
      
         writeBit(true);
         
         for (const Element& element : array)
         {
            writeBit(true);
            writeValue(element);
         }
    
         writeBit(false);
         
         return *this;
      }
      
      template<typename Array, typename Element>
      PowerEncoding& operator >>
      (Array& value)
      {
         
         bool bit = readBit();
         
         if (!bit)
            throw runtime_error("Expecting to read 1");
            
         Element element;
         
         while (readBit())
         {
            readValue(element);
            value.push_back(element);
         }
         
         return *this;
         
      }
      
      PowerEncoding&
      operator << (const std::string& str)
      {
         return
            operator << 
            < std::string, char > 
            (str);
         
      }
    
      PowerEncoding& operator >>
      (std::string& value)
      {
         
         return
            operator >>
            < std::string, char >
            (value);
         
      }
      
      PowerEncoding&
      operator << (const char* str)
      {
         return
            operator << (string(str));
      }
      
      
      
      PowerEncoding&
      operator << (const std::wstring& wstr)
      {
         operator << 
            < std::wstring, wchar_t> 
            (wstr);

         return *this;
      }
    
      PowerEncoding& operator >>
      (std::wstring& value)
      {
         
            operator >>
            < std::wstring, wchar_t >
            (value);
            
         return *this;
         
      }
      
      PowerEncoding&
      operator << (const wchar_t* wstr)
      {
         
         wstring _wstr(wstr);
         
         operator << 
            < std::wstring, wchar_t >
            (_wstr);

         return *this;
          
      }
    
      template<typename T>
      PowerEncoding& operator >>
      (std::vector<T>& value)
      {
         
         return
            operator >>
            < std::vector<T>, T >
            (value);
         
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