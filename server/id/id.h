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
#include "../b-string/bit-string.h"
#include "../b-string/data.h"
#include "../b-string/base64.h"
using namespace std;
using namespace std::chrono;
using namespace bee::fish::power_encoding;

namespace bee::fish::server
{
   
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
      
   class Id
   {
      
   public:
      
      Timestamp _timestamp;
      Data _key;
 
      Id() : _timestamp()
      {
      }
      
      Id(long ms, unsigned int inc) :
         _timestamp(ms, inc)
      {
      }
      
      static Id fromString(const BString& str)
      {
         vector parts = str.split(':');
         
         return Id(
            atol(
               parts[0].c_str()
            ),
            atol(
               parts[1].c_str()
            )
         );
         
      }
      
      static Id fromKey(const Data& key)
      {
         return decodeKey(key);
      }
      
      friend ostream& operator <<
      (
         ostream& out, Id& id
      )
      {
         id.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out)
      {
         const Data& key = this->key();
         out << key.toBase64();
      }
      
      const Data& key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      const Timestamp& timestamp() const
      {
         return _timestamp;
      }
      
      
      BString toString()
      {
         std::stringstream out;
         
         out << "{"
             <<  "\"" << "ms" << "\""
             << ":" << _timestamp.ms
             << ","
             <<  "\"" << "inc" << "\""
             << ":" <<  _timestamp.inc
             << "\"}";
             
         return out.str();
      }
      
   private:
   
       
      static void CHECK(bool boolean)
      {
         if (boolean == false)
            throw runtime_error("Check failed");
      }
      
      Data createKey()
      {
         BitEncoding encoding;
      
         // encode timestamp
         encoding.writeBit(true);
         
         encoding.writeBit(true);
         encoding << _timestamp.ms;
         
         encoding.writeBit(true);
         encoding << _timestamp.inc;
         
         encoding.writeBit(false);
         
         
         // Convert bits to bit string
 
         BitString bitString =
            BitString::fromBits(
               encoding.bits()
            );
         
         // get the data
         Data key =
            bitString.data();

         return key;
      }
      
      static Id decodeKey(const Data& key) {
   
      
         // extract the raw data
         Data raw = key;// Data::fromBase64(key);
         
         // extract the timestamp
         // from the key
         
         // Create a string of char bits
         BitString bitString =
            BitString::fromData(raw);
         
         // Create the encoding
         BitEncoding
            encoding(bitString.bits());
         
         // read the first "1"
         CHECK(encoding.readBit());
      
         // read 1 for ms
         unsigned long milliseconds;
         CHECK(encoding.readBit());
         encoding >> milliseconds;
         
         // read 1 for inc
         CHECK(encoding.readBit());
         unsigned long increment;
         encoding >> increment;
         
         // read 0
         CHECK(encoding.readBit() == false);

         Id id(milliseconds, increment);
         id._key = key;
         
         return id;
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
      
      

   };
 
}

#endif