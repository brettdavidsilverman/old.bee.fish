#ifndef BEE_FISH_DATABASE__INDEX_H
#define BEE_FISH_DATABASE__INDEX_H

#include <cmath>
#include "config.h"
#include "file.h"

using namespace std;

namespace bee::fish::database {

   extern const Size branchesPerPage;
      
   struct Index
   {
      File::Size _pageIndex;
      unsigned int _branchIndex;

      Index& operator++()
      {
         
         if (++_branchIndex == branchesPerPage)
         {
            _branchIndex = 0;
            ++_pageIndex;
         }

         return *this;
      }

      bool operator !()
      {
         return
             (_pageIndex   == 0 &&
              _branchIndex == 0);
      }
      
      operator bool()
      {
         return
            (_pageIndex   != 0 ||
             _branchIndex != 0);
      }
      
      bool operator == (const Index& rhs)
      {
         return
            _pageIndex
               == rhs._pageIndex &&
            _branchIndex
               == rhs._branchIndex;
      }
      
      friend ostream& operator <<
      (ostream& out, const Index& index)
      {
         out << "{"
             << index._pageIndex
             << ","
             << index._branchIndex
             << "}";
             
          return out;
      }
      
      static const Index root;
   };
   
   inline const Index Index::root = {1, 0};
}

#endif