#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H
#include <atomic>
#include "index.h"

using namespace std;

namespace bee::fish::database
{

   struct Branch
   {
      //Index  _parent;
      Index  _left;
      Index  _right;
      /*
      Branch(const Branch& source)
      {
         _parent = source._parent;
         _left   = source._left;
         _right  = source._right;
      }
      
      Branch& operator = (const Branch& source)
      {
         _parent = source._parent;
         _left   = source._left;
         _right  = source._right;
         return *this;
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
         
         if ( /*( ( _parent && rhs._parent ) &&
                ( _parent != rhs._parent ) ) ||*/
              ( ( _left && rhs._left ) &&
                ( _left != rhs._left ) )    ||
              ( ( _right && rhs._right ) &&
                ( _right != rhs._right ) ) )
            throw
               runtime_error("Multiple writes");
         
        // _parent |= rhs._parent;
         _left   |= rhs._left;
         _right  |= rhs._right;
         
         return *this;
      }
      
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         out << '('
             << branch._left
           //  << ','
            // << branch._parent
             << ','
             << branch._right
             << ')';
          
         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   //inline const Index Branch::Root = 0;
   
}

#endif