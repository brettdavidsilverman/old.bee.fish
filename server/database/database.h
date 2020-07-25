#ifndef BEE_FISH_DATABASE__DATABASE_H
#define BEE_FISH_DATABASE__DATABASE_H

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

namespace bee::fish::database {

class Pointer;

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
   typedef File::Size Index;
   
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
    
   Size increment() const;
   const Size _increment;
   
   friend class Pointer;
   
public:
   struct Header {
      union {
         char buffer[4096];
         struct {
            char   version[256];
            Index  last;
         };
      };
   };
   
   struct Data :
      Header
   {
      Index array[];
   };
   
   Data *_data;
   Index* _array;
   Index* _last;
   Index _length = 0;
private:
   
   void mapFileToMemory();
   
   void setData();
   void* _memoryMap = NULL;
   void setLength();

protected:
   virtual Size resize(
      Size newSize
   );
};

typedef Database::Index Index;

ostream& operator << (ostream& out, const Database&);

}

#endif