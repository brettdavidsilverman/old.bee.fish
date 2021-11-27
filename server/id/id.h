#ifndef BEE_FISH_SERVER__ID_H
#define BEE_FISH_SERVER__ID_H

#include <string>
#include <iomanip>
#include <exception>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>    
#include "../power-encoding/encoding.h"
#include "../b-string/string.h"

using namespace std;
using namespace std::chrono;
using namespace BeeFishPowerEncoding;

namespace BeeFishId
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
      
      
      BString _key;
      BString _name;
      Timestamp _timestamp;
      
      Id() : _timestamp()
      {
      }
      
      Id(
         const BString& name,
         long ms = 0,
         unsigned int inc = 0
      ) :
         _name(name),
         _timestamp(ms, inc)
      {
      }
      
      static Id fromKey(const BString& key)
      {
         return decodeKey(key);
      }
      
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {
      
         Id copy(id);
         
         copy.write(out);
         
         return out;
      }
      
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const Id& id
      )
      {
         
         Id copy(id);
         
         Data raw = Data::fromBase64(
            copy.key()
         );
         
         BitStream bits(raw);
         
         for (auto bit : bits)
         {
            stream.writeBit(bit);
         }

         return stream;
      }

      virtual void write(ostream& out)
      {
         const BString& key = this->key();
         out << key;
      }
      
      const BString& key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      const Timestamp& timestamp() const
      {
         return _timestamp;
      }
      
      const BString& name()
      {
         return _name;
      }
      
      BString toString()
      {
         std::stringstream out;
         
         out << "{"
             << "\"" << "name" << "\""
             << ":\"";
         _name.writeEscaped(out);
         out << "\""
             << ","
             <<  "\"" << "ms" << "\""
             << ":" << _timestamp.ms
             << ","
             <<  "\"" << "inc" << "\""
             << ":" <<  _timestamp.inc
             << "\"}";
             
         return out.str();
      }
      
   private:
      
      BString createKey()
      {
         BitStream stream;
      
         // encode timestamp
         stream.writeBit(true);
         
        // stream.writeBit(true);
         stream << _name;
         
         stream.writeBit(true);
         stream << _timestamp.ms;
         
         stream.writeBit(true);
         stream << _timestamp.inc;
         
         stream.writeBit(false);
         
         
         // get the data
         Data key = stream.toData();

         return key.toBase64();
      }
      
      static Id decodeKey(const BString& key) {
   
      
         // extract the raw data
         Data raw = Data::fromBase64(key);
         
         // extract the timestamp
         // from the key
         
         // Create a string of char bits
         BitStream stream(raw);
        
         
         // read the first "1"
         CHECK(stream.readBit());

         BString name;
         stream >> name;
         
         // read 1 for ms
         CHECK(stream.readBit());
         unsigned long milliseconds;
         stream >> milliseconds;
         
         // read 1 for inc
         CHECK(stream.readBit());
         unsigned long increment;
         stream >> increment;
         
         // read 0
         CHECK(stream.readBit() == false);


         // count 0
         CHECK(stream.count() == 0);
         
         Id id(name, milliseconds, increment);
         id._key = key;
         
         return id;
      }
      

   };
   
   //unsigned long Timestamp::_lastMs = 0;
   //unsigned long Timestamp::_lastInc = 0;

 
}

#endif