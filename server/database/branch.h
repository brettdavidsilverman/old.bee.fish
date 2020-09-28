#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H

#include "index.h"

using namespace std;

namespace bee::fish::database
{
   typedef unsigned long long Count;
   
   struct Branch
   {
      Index _parent;
      Count _count;
      bool  _bit;
      Index _left;
      Index _right;
      /*
      Branch& operator --()
      {
         Index parent = _parent;
         --_count;
         if (_count == 0)
         {
            memset(this, 0, sizeof(Branch));
         }
         return --_parent;
      }
      */
   };
   
}

#endif