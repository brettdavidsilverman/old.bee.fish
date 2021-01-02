#ifndef BEE_FISH_SERVER__ID_H
#define BEE_FISH_SERVER__ID_H

#include <string>
#include <iomanip>
#include <exception>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>    
#include "../power-encoding/encoding.h"

using namespace std;
using namespace std::chrono;
using namespace bee::fish::power_encoding;

namespace bee::fish::server
{
   class Id
   {
      
   public:
      
      class Timestamp
      {
      public:
         unsigned long ms;
         unsigned long inc;
         
         Timestamp() :
            ms(milliseconds()),
            inc(increment(ms))
         {
         }
         
         Timestamp(
            unsigned long milliseconds,
            unsigned long increment
         )
         {
            ms = milliseconds;
            inc = increment;
         }
         
      private:
         inline static unsigned long _lastMs = 0;
         inline static unsigned long _lastInc = 0;
         
         static unsigned long milliseconds()
         {
            unsigned long now =
               std::chrono::duration_cast
                  <std::chrono::milliseconds>
                  (
                     std::chrono::system_clock
                        ::now()
                        .time_since_epoch()
                  ).count();
            return now;
         }
         
         static unsigned long increment(
            unsigned long ms
         )
         {
            if (ms <= _lastMs)
               ++_lastInc;
            else
               _lastInc = 0;
               
            _lastMs = ms;
            
            return _lastInc;
         }
         
      
      };
      
      Timestamp timestamp;
      string _name;
      wstring _key;
 
      Id() : timestamp()
      {
      }
      
      Id(long ms, unsigned int inc) :
         timestamp(ms, inc)
      {
      }
      
      Id(const string& str) :
         timestamp(
            atol(
               Parts(str)[0].c_str()
            ),
            atol(
               Parts(str)[1].c_str()
            )
         )
      {
      }
      
      Id(const wstring& key)
      {
         decodeKey(key);
      }
      
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {

         out << "Id(\""
             << id.timestamp.ms
             << ":"
             << id.timestamp.inc
             << "\")";
         
         return out;
      }
      
      wstring key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      string hex()
      {
      
         wstring key = Id::key();
      
         string hex = "";
      
         for (unsigned int i = 0; i < key.length(); ++i)
         {
             unsigned long number = key[i];
             unsigned int high =
                (number & 0xFF00) >> 8;
             unsigned int low = number & 0x00FF;

             hex +=
                toHex(high) + ":" + toHex(low);
            
            if (i < key.length() - 1)
               hex += ",";
         }
      
         return hex;
      
      }
      
   private:
   
       string toHex(unsigned int number)
       {
         stringstream stream;
         stream
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << number;
         return stream.str();
      }
      
      static void CHECK(bool boolean)
      {
         if (boolean == false)
            throw runtime_error("Check failed");
      }
      
      wstring createKey()
      {
         stringstream stream;
      
         Encoding encoding(stream, stream);
     
         // encode timestamp
         encoding.writeBit(true);
         
         encoding.writeBit(true);
         encoding << timestamp.ms;
         
         encoding.writeBit(true);
         encoding << timestamp.inc;
         
         encoding.writeBit(false);
         
         
         // Convert bits to wide char string
 
         BitString bitString(
            stream.str()
         );
         
         wstring key =
            bitString.wstr();

         return key;
      }
      
      void decodeKey(const wstring& key) {
   
         _key = key;
      
         // extract the timestamp
         // from the key
         BitString bitString(_key);
         
         stringstream
            stream(bitString.bits());
            
         Encoding
            encoding(stream, stream);
         
         // read the first "1"
         CHECK(encoding.readBit());
      
         // read 1 for ms
         unsigned long ms;
         CHECK(encoding.readBit());
         encoding >> ms;
         timestamp.ms = ms;
         
         // read 1 for inc
         CHECK(encoding.readBit());
         unsigned long increment;
         encoding >> increment;
         timestamp.inc = increment;
         
         // read 0
         CHECK(encoding.readBit() == false);

      }
      
      class Parts
      {
      protected:
         vector<string> _array{"", ""};
      public:
         Parts(string str) {

            boost::split(
               _array,
               str,
               boost::is_any_of(":")
            );
         }
         
         string operator[]
         (size_t index) const
         {
            return _array[index];
         }
         
      };
      
      class BitString
      {
      protected:
         string _bits;
         wstring _wstr;
         
      public:
         BitString(string bits)
         {
            _bits = bits;
            wchar_t wc = 0;
            unsigned int i = 0;
            for (char c : bits)
            {
               if (c == '1')
                  wc |= 1;
                  
               if (++i < 16)
                  wc = wc << 1;
               else
               {
                  _wstr.push_back(wc);
                  wc = 0;
                  i = 0;
               }
            }
            
            if (i > 0)
               _wstr.push_back(wc);
            
         }
         
         BitString(wstring wstr)
         {
            _wstr = wstr;
            _bits = "";
            
            for ( wchar_t wc : wstr )
            {
                for (int i = 15; i >= 0; --i)
                {
                   unsigned int mask = (1 << i);
                   bool bit = wc & mask;
                   if (bit)
                      _bits.push_back('1');
                   else
                      _bits.push_back('0');
                }
            }
         }
         
         wstring wstr()
         {
            return _wstr;
         }
         
         string bits()
         {
            return _bits;
         }
         
      };

   };
 
}

#endif