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
// The _next points to the furthest element.
class Database : 
   public File {
public:
   typedef File::Size Index;
   typedef unsigned long long Count;
   
   static const char* version;
   inline static Size pageSize = getpagesize();
   
   Database(
      const string& filePath,
      const Size initialSize = 1000 * 1000,
      const Size increment = 1000 * 1000
   ) :
   File(
      filePath,
      getPageAlignedSize(
         initialSize,
         getpagesize()
      )
   ),
   _increment(increment)
   {
      mapFileToMemory();
      Header& header = _data->header;
      
      if (isNew()) {
         strcpy(header.version, BEE_FISH_DATABASE_VERSION);
      }
      else if (strcmp(header.version, BEE_FISH_DATABASE_VERSION) != 0) {
         std::string error = "Invalid file version.";
         error += " Expecting ";
         error += BEE_FISH_DATABASE_VERSION;
         error += ". Got ";
         error += header.version;
         throw runtime_error(error);
      }
   }

   ~Database()
   {
   
      if (_memoryMap) {
         munmap(_memoryMap, _size);
         _memoryMap = NULL;
      }
   
   }

    
   Size increment() const
   {
      return _increment;
   }

   const Size _increment;
   
   friend class Pointer;
   
public:
   struct Header {
      union {
         char buffer[4096];
         struct {
            char   version[256];
            Index  next;
         };
      };
   };
   
   struct Fork
   {
      Index parent;
      Count count;
      Index left;
      Index right;
   };
   
   struct Data :
      Header
   {
      Header header;
      Fork array[];
   };
   
   Data *_data;
   Index _length = 0;
private:
   
   void mapFileToMemory() {

      _memoryMap = mmap(
         NULL,
         _size,
         PROT_READ | PROT_WRITE,
         MAP_SHARED,
         _fileNumber,
         0
      );
   
      if (_memoryMap == MAP_FAILED) {
         throw runtime_error(
            "Error mapping memory"
         );
      }
   
      setData();
   
   }

   void setData()
   {
      _data = (Data*)_memoryMap;
      setLength();
   }

   void* _memoryMap = NULL;
   
   void setLength()
   {
      _length = (
         _size - sizeof(Header)
      ) / sizeof(Fork);
   }


protected:
   Size resize(
      Size size = 0
   )
   {
 
      if (size == 0)
         size = _size + _increment;
         
      Size oldSize = _size;
      Size newSize =
         getPageAlignedSize(size);
   
      if (newSize > oldSize) {
      
         void* memoryMap =
            mremap(
               _memoryMap,
               oldSize,
               newSize,
               MREMAP_MAYMOVE,
               0
            );
            
         if (memoryMap == MAP_FAILED) {
            throw runtime_error(
               "Error remapping memory"
            );
         }

         _memoryMap = memoryMap;
      
         File::resize(newSize);

         setData();
      }
   
      setLength(); 
      return _size;
   }
   
   static Size getPageAlignedSize
   (
      const Size value,
      const Size minimum = 0
   )
   {

      if (value == 0)
         return 0;
   
      Size newValue = value;
      if (newValue < minimum)
         newValue = minimum;
      
      Size mod =
         newValue % pageSize;
   
      if (mod != 0)
         newValue += (
            pageSize - mod
         );
   
      return newValue;

   }
   
   friend ostream& operator <<
   (ostream& out, const Database& db)
   {
      out << "Database " 
          << db.filePath
          << " "
          << endl
          << db._data->header.version
          << endl
          << " next "
          << db._data->header.next;
          
      return out;
   }

};

typedef Database::Index Index;
typedef Database::Fork Fork;

}

#endif