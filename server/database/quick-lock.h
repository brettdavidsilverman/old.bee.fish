#ifndef BEE_FISH_DATABASE__QUICK_LOCK_H
#define BEE_FISH_DATABASE__QUICK_LOCK_H
//#include "atomic_flag.hpp"

#include <atomic>
#include <thread>

//#undef DEBUG

using namespace std;

namespace bee::fish::database
{
   class QuickLock
   {
   private:
     
      //std::experimental::atomic_flag
      mutex   _mutex;
      atomic<bool>   _locked;
      
   public:
   
      ~QuickLock()
      {
         if (_locked.load())
            _mutex.unlock();
         
      }
      
      void lock()
      {
#ifdef DEBUG
            cerr << 'L';
#endif
         _mutex.lock();
         _locked.store(true);
         
         /*
         while (_flag.test_and_set())
            boost::this_thread::yield();
         
         */
      }
      
      void unlock()
      {

#ifdef DEBUG
         cerr << 'U';
#endif
         _mutex.unlock();
         _locked.store(false);
         
         //_flag.clear();
         
      }
      
      void wait()
      {
#ifdef DEBUG
         cerr << 'W';
#endif
         while(_locked.load())
            this_thread::yield();
      }
      
   };
};

#endif