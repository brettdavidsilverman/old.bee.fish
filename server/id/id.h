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
      BString _key;
 
      Id() : timestamp()
      {
      }
      
      Id(long ms, unsigned int inc) :
         timestamp(ms, inc)
      {
      }
      
      Id(const BString& str) :
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
      
      Id(const BString& key)
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
      
      BString key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      BString hex()
      {
      
         BString key = Id::key();
      
         BString hex = "";
      
         for (unsigned int i = 0; i < key.size(); ++i)
         {
             uint32_t number = key[i];
             hex += toHex(number);
            if (i < key.size() - 1)
               hex += ",";
         }
      
         return hex;
      
      }
      
   private:
   
       string toHex(uint32_t number)
       {
         stringstream stream;
         stream
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << number;
         return stream.str();
      }
      
      static void CHECK(bool boolean)
      {
         if (boolean == false)
            throw runtime_error("Check failed");
      }
      
      BString createKey()
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
 
         BitString bitString =
            BitString::fromBits(
               stream.str()
            );
         
         BString key =
            bitString.key();

         return key;
      }
      
      void decodeKey(const BString& key) {
   
         _key = key;
      
         // extract the timestamp
         // from the key
         BitString bitString =
            BitString::fromKey(_key);
         
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
         vector<BString> _array{"", ""};
      public:
         Parts(BString str) {

            bool colon = false;
            for ( size_t index = 0;
                  index < str.size(); 
                  ++index )
            {
               if (str[index] == ':')
                  colon = true;
               else if (!colon)
                  _array[0].push_back(str[index]);
               else
                  _array[1].push_back(str[index]);
            }

         }
         
         const BString& operator[]
         (size_t index) const
         {
            return _array[index];
         }
         
      };
      
      class BitString
      {
      protected:
         BString _bits;
         BString _key;
         
      public:
         static BitString fromBits
         (const BString& bits)
         {
            BitString bitString;
            bitString._bits = bits;
            Char wc = 0;
            unsigned int i = 0;
            for (char c : bits)
            {
               if (c == '1')
                  wc |= 1;
                  
               if (++i < 32)
                  wc = wc << 1;
               else
               {
                  bitString._key.push_back(wc);
                  wc = 0;
                  i = 0;
               }
            }
            
            if (i > 0)
               bitString._key.push_back(wc);
               
            return bitString;
            
         }
         
         static BitString fromKey
         (const BString& key)
         {
            BitString bitString;
            
            bitString._key = key;
            bitString._bits = "";
            
            for ( const Char& wc : key )
            {
                for (int i = 31; i >= 0; --i)
                {
                   unsigned int mask = (1 << i);
                   bool bit = wc & mask;
                   if (bit)
                      bitString._bits.push_back('1');
                   else
                      bitString._bits.push_back('0');
                }
            }
            
            return bitString;
         }
         
         const BString& key() const
         {
            return _key;
         }
         
         const BString& bits() const
         {
            return _bits;
         }
         
      };

   };
 
}

#endif