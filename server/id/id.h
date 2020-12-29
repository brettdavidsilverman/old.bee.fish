#ifndef BEE_FISH_SERVER__ID_H
#define BEE_FISH_SERVER__ID_H

#include <string>
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
      const system_clock::time_point
         timestamp;
      const unsigned int increment;
      
      string _name;
      wstring _key;
 
      inline static system_clock::time_point
         _lastTimestamp =
            system_clock::now();

      inline static unsigned int 
         _increment = 0;

      Id() :
         timestamp(system_clock::now()),
         increment(
            Increment(
               timestamp,
               _lastTimestamp,
               _increment
            )
         )
      {
      }
      
      Id(long long integral, unsigned int inc) :
         timestamp{system_clock::duration{integral}},
         increment(inc)
      {
      }
      
      Id(const string& str) :
         timestamp{
            system_clock::duration{
               atol(
                  Parts(str)[0].c_str()
               )
            }
         },
         increment(
            atoi(
               Parts(str)[1].c_str()
            )
         )
      {
      }
      
      // Convert time_point to signed integral type
      auto integral_duration() const
      {
         return timestamp
            .time_since_epoch()
            .count();
      }

      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {

         out << "Id(\""
             << id.integral_duration()
             << ":"
             << id.increment
             << "\")";
         
         return out;
      }
      
      wstring key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      
   private:
   
      
      wstring createKey()
      {
         auto time = integral_duration();
         
         stringstream stream;
      
         Encoding encoding(stream, stream);
     
         // encode timestamp
         encoding.writeBit(true);
      
         encoding << time;
         
         encoding << increment;
         
         cerr << encoding.count() << endl;
         
         // Convert bits to wide char string
 
         BitString bitString(
            stream.str()
         );
         
         wstring key =
            bitString.wstr();

         return key;
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
         
         wstring wstr()
         {
            return _wstr;
         }
         
      };

      unsigned int Increment(
         const system_clock::time_point&
            timestamp,
         system_clock::time_point&
            _lastTimestamp,
         unsigned int&
            _increment
      )
      {
         unsigned int increment;
         if (timestamp == _lastTimestamp)
            increment = ++_increment;
         else {
            _lastTimestamp = timestamp;
            increment = _increment = 0;
         }
         return increment;
      }
   };
 
}

#endif