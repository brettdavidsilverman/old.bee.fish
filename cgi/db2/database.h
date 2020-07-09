
#include <sys/mman.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <mutex>
#include <shared_mutex>
#include <string.h>
#include <condition_variable>

#include "file.h"
#include "version.h"

using namespace std;

// Store a memory mapped array of
// [left, right] elements.
// The pointer points to the next elememt
// in the array.
// A zero is stored if the branch
// hasnt been visited yet.
// The _last points to the furthest element.
class Database : 
   public File {
public:
   typedef File::Size Pointer;
   static const char* version;
   static Size pageSize;
   static 
      Size const getPageAlignedSize(
         const Size value,
         const Size minimum = 0
      );
   
   Database(
      const string& filePath,
      const Size initialSize = 1000 * 1000,
      const Size increment = 1000 * 1000
   );
   
   ~Database();
    
   Pointer walkPath(const string& bits);
   Pointer walkBit(bool bit);
   void traverse(ostream& out, const Pointer pointer) const;
   Pointer pointer = 0;
   
   const Size _increment;
   
public:
   struct Header {
      union {
         char buffer[4096];
         struct {
            char     version[256];
            Pointer  last;
         };
      };
   };
   
   struct Data :
      Header
   {
      Pointer array[];
   };
   
   Data *_data;
   Pointer* _array;
   Pointer* _last;
   Pointer _length = 0;
private:
   
   void mapFileToMemory();
   
   void setData();
   void* _memoryMap = NULL;
   void setLength();
   
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
};

typedef Database::Pointer Pointer;

ostream& operator << (ostream& out, const Database&);