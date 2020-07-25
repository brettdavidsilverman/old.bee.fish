#ifndef BEE_FISH_SERVER__ID_H
#define BEE_FISH_SERVER__ID_H

#include <string>
#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>    


using namespace std;
using namespace std::chrono;

namespace bee::fish::server
{
   class Id
   {
      using Clock =
         high_resolution_clock;
         
      using TimePoint =
         time_point<Clock>;
      
      using Ms = milliseconds;
      
      const TimePoint timestamp;
      const unsigned int increment;
      
      string _name;
      
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
      
      unsigned int Increment(
         const TimePoint& timestamp,
         TimePoint& _lastTimestamp,
         unsigned int& _increment
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
      
   public:
      
      static TimePoint _lastTimestamp;
      static unsigned int _increment;

      Id() :
         timestamp(Clock::now()),
         increment(
            Increment(
               timestamp,
               _lastTimestamp,
               _increment
            )
         )
      {
         cerr << "Id()";
      }
      
      Id(long long ms, unsigned int inc) :
         timestamp(Ms(ms)),
         increment(inc)
      {
      }
      
      Id(const string& str) :
         timestamp(
            Ms(
               atol(Parts(str)[0].c_str())
            )
         ),
         increment(atoi(Parts(str)[1].c_str()))
      {
      }
      
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {

         const Clock::duration since_epoch =
            id.timestamp.time_since_epoch();
            
         out << "Id(\""
             <<  duration_cast<Ms>
                 (since_epoch).count()
             << ":"
             << id.increment
             << "\")"
             << endl;
         
         return out;
      }
         
   };
   
   Id::TimePoint Id::_lastTimestamp = Clock::now();
   unsigned int Id::_increment = 0;
}

#endif