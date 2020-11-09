#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H
#include <atomic>
#include "index.h"

using namespace std;

namespace bee::fish::database
{

   typedef unsigned long long Count;
   extern Count count;
   struct Branch
   {
      std::atomic_flag _lock;
      bool _isLocked;
      Index  _parent;
      Index  _left;
      Index  _right;
     
      Branch()
      {
         _lock.clear();
         _isLocked = false;
         _parent = 0;
         _left   = 0;
         _right  = 0;
      }
      
      Branch(const Branch& source)
      {
         _isLocked = source._isLocked;
         _parent = source._parent;
         _left   = source._left;
         _right  = source._right;
      }
      
      Branch& operator = (const Branch& source)
      {
         _isLocked = source._isLocked;
         _parent = source._parent;
         _left   = source._left;
         _right  = source._right;
         return *this;
      }
      
      bool isDeadEnd() {
         return (
            !_left &&
            !_right
         );
      }
      
      void lock()
      {
         ++count;
        // cerr << "+(" << count << ')';
         while (!_lock.test_and_set())
            this_thread::yield();
         _isLocked = true;
      }
      
      void unlock()
      {
         --count;
         //cerr << "-(" << count << ')';
         _lock.clear();
         _isLocked = false;
      }
      
      void wait()
      {
         //cerr << 'W';
         while(_isLocked)
            this_thread::yield();
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