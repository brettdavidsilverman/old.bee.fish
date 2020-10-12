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
      Index _left;
      Index _right;
      
      bool isDeadEnd() {
         return (
            !_left &&
            !_right
         );
      }
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
   
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         out << '('
             << branch._left
             << ','
             << branch._right
             << ')';
          
         return out;
      }
   };
   
}

#endif