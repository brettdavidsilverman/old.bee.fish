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
      
      
   public:
      const system_clock::time_point
         timestamp;
      const unsigned int increment;
      
      string _name;
      
 
      static system_clock::time_point _lastTimestamp;
      static unsigned int _increment;

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
      
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {

         // Convert time_point to signed integral type
         auto integral_duration =
            id.timestamp.time_since_epoch().count();

         out << "Id(\""
             << integral_duration
             << ":"
             << id.increment
             << "\")";
         
         return out;
      }
      
   private:
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
         const system_clock::time_point& timestamp,
         system_clock::time_point& _lastTimestamp,
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
   };
   
   system_clock::time_point
      Id::_lastTimestamp =
         system_clock::now();
         
   unsigned int Id::_increment = 0;
}

#endif