#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H

#include "index.h"

using namespace std;

namespace bee::fish::database
{

   typedef unsigned long long Count;
   
   struct Branch
   {
      //std::atomic_flag _locked;
      Index  _parent;
      Index  _left;
      Index  _right;
     /*
      Branch()
      {
         _parent = 0;
         _left   = 0;
         _right  = 0;
      }
      */
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
   
      Branch& operator |= (const Branch& rhs)
      {
         
         if ( ( ( _parent && rhs._parent ) &&
                ( _parent != rhs._parent ) ) ||
              ( ( _left && rhs._left ) &&
                ( _left != rhs._left ) )    ||
              ( ( _right && rhs._right ) &&
                ( _right != rhs._right ) ) )
            throw
               runtime_error("Multiple writes");
         
         _parent |= rhs._parent;
         _left   |= rhs._left;
         _right  |= rhs._right;
         
         return *this;
      }
      
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         out << '('
             << branch._left
             << ','
             << branch._parent
             << ','
             << branch._right
             << ')';
          
         return out;
      }
      
      static const Index Root;
         
   };
   
   inline const Index Branch::Root = 0;
   
}

#endif