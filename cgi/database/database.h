
#include <sys/mman.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <mutex>
#include <shared_mutex>
#include <string.h>
#include <condition_variable>

#include "file.h"

using namespace std;

class Database : 
   public File {
public:
   typedef unsigned long Pointer;
   static const char*    version;
   
   static 
      Size const getPageAlignedSize(
         const Size pageSize,
         const Size value,
         const Size minimum = 0
      );
   
   Database(
      const string& filePath,
      const Size pageSize = getpagesize(), 
      const Size initialSize = 1000 * 1000,
      const Size increment = 1000 * 1000
   );
   
   ~Database();
   
   struct Branch {
      bool locked;
      Pointer parent;
      Pointer left;
      Pointer right;
   };
    
   Pointer putString(const Pointer start, const string& string);
   map<Pointer, mutex> keys;
   map<Pointer, condition_variable> guards;
protected:
   
   Size readBits(
      Pointer& pointer,
      const string& bits
   );
   Size writeBits(
      Pointer& pointer,
      const string& bits
   );
   string toBits(const string& string);

   const Size _increment;
   
public:
   struct Header {
      union {
         char buffer[4096];
         struct {
            char     version[256];
            Size     pageSize;
            Pointer  nextPointer;
         };
      };
   };
   
   struct Data : Header {
      Branch branches[];
   };
   
   Data *_data;

private:
   
   void mapFileToMemory();
   
   void setMemory();
   void* _memoryMap = NULL;
   Size _pageSize;

protected:
   typedef boost::shared_mutex Mutex;
   
   Mutex _access;
   
   typedef boost::shared_lock<Mutex>
      ReadLock;
   
   typedef boost::upgrade_lock<Mutex>
      WriteLock;
   
   typedef boost::upgrade_to_unique_lock<Mutex>
      UniqueLock;
      
protected:
   virtual Size resize(
      Size newSize
   );
   
   void grow(
      const Size length,
      WriteLock& writeLock
   );
};

typedef Database::Pointer Pointer;

