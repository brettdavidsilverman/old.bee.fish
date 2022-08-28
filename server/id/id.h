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
#include "../b-script/b-script.h"

using namespace std;
using namespace std::chrono;
using namespace BeeFishPowerEncoding;

namespace BeeFishId
{
   
   class Timestamp
   {
   public:
      unsigned long _seconds;
      unsigned long _milliseconds;
      unsigned long _increment;

      Timestamp() :
         _seconds(seconds()),
         _milliseconds(milliseconds() % 1000),
         _increment(increment(_seconds, _milliseconds))
      {
      }
         
      Timestamp(
         unsigned long seconds,
         unsigned long milliseconds,
         unsigned long increment
      )
      {
         _seconds = seconds;
         _milliseconds = milliseconds;
         _increment = increment;
      }
         
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
         
      static unsigned long seconds()
      {
         unsigned long now =
            std::chrono::duration_cast
               <std::chrono::seconds>
               (
                  std::chrono::system_clock
                     ::now()
                     .time_since_epoch()
               ).count();
         return now;
      }

      static std::string epoch() {
         unsigned long seconds = Timestamp::seconds();
         unsigned long milliseconds = Timestamp::milliseconds();
         return Timestamp::formatEpochMilliseconds(
            seconds,
            milliseconds
         );
      }

      static std::string formatEpochMilliseconds(
         unsigned long seconds,
         unsigned long milliseconds
      ) {
         std::stringstream stream;
         stream << seconds;
         stream << std::setfill('0') << std::setw(3) << (milliseconds  % 1000);
         return stream.str();
      }

      std::string epochMilliseconds() {
         return Timestamp::formatEpochMilliseconds(
            _seconds,
            _milliseconds
         );
      }

   private:

      static unsigned long& lastSeconds() {
         static unsigned long _lastSeconds = 0;
         return _lastSeconds;
      }

      static unsigned long& lastMilliseconds() {
         static unsigned long _lastMilliseconds = 0;
         return _lastMilliseconds;
      }
      
      static unsigned long& lastIncrement() {
         static unsigned long _lastIncrenebt = 0;
         return _lastIncrenebt;
      }
         
      static unsigned long increment(
         unsigned long seconds,
         unsigned long milliseconds
      )
      {
         unsigned long& _lastSeconds = lastSeconds();
         unsigned long& _lastMilliseconds = lastMilliseconds();
         unsigned long& _lastIncrement = lastIncrement();

         if (seconds <= _lastSeconds)
            ++_lastIncrement;
         else if (seconds > _lastSeconds)
            _lastIncrement = 0;
         else if (milliseconds <= _lastMilliseconds)
            ++_lastIncrement;
         else
            _lastIncrement = 0;
               
         _lastSeconds = seconds;
         _lastMilliseconds = milliseconds;
            
         return _lastIncrement;
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
         const BString& name
      ) :
         _name(name),
         _timestamp()
      {
      }

      Id(
         const BString& name,
         long seconds,
         long milliseconds,
         unsigned int increment
      ) :
         _name(name),
         _timestamp(seconds, milliseconds, increment)
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
         
         out << copy.key();
         
         return out;
      }
     
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const Id& id
      )
      {
         
         id.write(stream);
         
         return stream;

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
         BeeFishBScript::Object output;

         output["name"] = _name;
         output["epochMilliseconds"] = _timestamp.epochMilliseconds();
         //output["milliseconds"] = (BeeFishBScript::Number)_timestamp._milliseconds;
         output["increment"] = (BeeFishBScript::Number)_timestamp._increment;

         return output.bstr();
      }
      
   private:
      
      PowerEncoding& write(
         PowerEncoding& stream
      ) const
      {
         // encode timestamp
         stream.writeBit(1);
         stream << _name;
         
         stream.writeBit(1);
         stream << _timestamp._seconds;

         stream.writeBit(1);
         stream << _timestamp._milliseconds;
         
         stream.writeBit(1);
         stream << _timestamp._increment;
         
         stream.writeBit(0);
         
         return stream;
      }
      
      BString createKey()
      {
      
         BitStream stream;
         
         write(stream);
         
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
         assert(stream.readBit() == 1);

         stream.resetCount();

         BString name;
         stream >> name;
         
         // read 1 for seconds
         assert(stream.readBit() == 1);
         unsigned long seconds;
         stream >> seconds;

         // read 1 for ms
         assert(stream.readBit() == 1);
         unsigned long milliseconds;
         stream >> milliseconds;
         
         // read 1 for inc
         assert(stream.readBit() == 1);
         unsigned long increment;
         stream >> increment;
         
         // read 0
         assert(stream.readBit() == 0);


         Id id(name, seconds, milliseconds, increment);
         id._key = key;
         
         return id;
      }
      

   };
   
   //unsigned long Timestamp::_lastMs = 0;
   //unsigned long Timestamp::_lastInc = 0;

 
}

#endif