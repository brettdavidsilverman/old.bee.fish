#ifndef BEE_FISH_DATABASE__QUICK_LOCK_H
#define BEE_FISH_DATABASE__QUICK_LOCK_H
#include <atomic>

using namespace std;

namespace bee::fish::database
{
   class QuickLock
   {
   private:
     
      mutex _lock;
      atomic_flag _flag;
      bool _isLocked = false;
   public:
  
      
      ~QuickLock()
      {
         return;
         if (_isLocked)
            unlock();
      }
      
      void lock()
      {
         return;
         _lock.lock();
         _isLocked = true;
         return;
         while (_flag.test_and_set())
            boost::this_thread::yield();
         
      }
      
      void unlock()
      {
         return;
         _lock.unlock();
         _isLocked = false;
         return;
         _isLocked = false;
         _flag.clear();
         
      }
      
      void wait()
      {
         return;
         if (_isLocked)
         {
            _lock.lock();
            _lock.unlock();
         }
         return;
         while(_isLocked)
            boost::this_thread::yield();
      }
      
   };
};

#endif