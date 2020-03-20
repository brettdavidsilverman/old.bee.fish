#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <string.h>
#include <tgmath.h>

#include "database.h"

#undef DEBUG

using namespace std;

const char* Database::version =
   "Db.b2 Version Alpha";

Database::Pointer
Database::putString(
   const Pointer start,
   const string& string
) {
 
   std::string bits = toBits(string);
   
   Pointer pointer = start;
   
   Size readCount = readBits(
      pointer,
      bits
   );

   
   if (readCount < bits.length()) {

      Size writeCount = writeBits(
         pointer,
         bits.substr(readCount)
      );
      
   }
   
   return pointer;
}


Size
Database::readBits(
   Pointer& pointer,
   const string& bits
) {
   ReadLock readLock(_access);
   
   Size readCount = 0;
   Size length = bits.length();
   Branch* branches = _data->branches;
   while (readCount < length) {
      
      Branch& branch =
         branches[pointer];
      
      if (branch.locked) {
         mutex& key = keys[pointer];
         unique_lock<mutex> lock(key);
         condition_variable& guard = guards[pointer];
         while (branch.locked)
            guard.wait(lock);
         cout << "g+" << endl;
      }
         
      if (bits[readCount] == '0') {
         // left branch
         if (branch.left == 0)
            break;
         pointer = branch.left;
      }
      else {
         // right branch
         if (branch.right == 0)
            break;
         pointer = branch.right;
      }
      
      ++readCount;
   }
   
   return readCount;
   
}


Size
Database::writeBits(
   Pointer& pointer,
   const string& bits
) {
   
   {
      WriteLock writeLock(_access);
      enlarge(bits.length(), writeLock);
   }
   
   Pointer& nextPointer = _data->nextPointer;
   Branch* branches = _data->branches;
   
   Pointer gate = pointer;
   mutex& key = keys[gate];
   condition_variable& guard = guards[gate];
   
   {
      lock_guard lock(key);
      branches[gate].locked = true;
   }
   
   Size length = bits.length();
   Size written = 0;
   /*
   while (written < length) {
   
      Branch& branch = 
         branches[pointer];
      
      if (bits[written] == '0') {
      
         // left branch
         if (branch.left == 0)
            branch.left =
               ++nextPointer;
               
            pointer = branch.left;
      }
      else {
      
         // right branch
         if (branch.right == 0)
            branch.right =
               ++nextPointer;
               
         pointer = branch.right;
      }
      
      ++written;
   }
   */
   {
      lock_guard lock(key);
      branches[gate].locked = false;
   }
   
   guard.notify_all();
   
   guards.erase(gate);
   keys.erase(gate);
   
   return written;
   
   
}


void 
Database::enlarge(
   const Size length,
   Database::WriteLock& writeLock
)
{
   Size newSize =
      sizeof(Header)          +
      (_data->nextPointer      +
        length * CHAR_BIT)     *
      sizeof(Branch);
   
   newSize =
      getPageAlignedSize(
         _pageSize,
         newSize
      );
   
   Size size = fileSize();
   
   if (newSize > size) {
   
      Size difference =
         newSize - size;
         
      Size increments =
         ceil(
            (float)difference /
            (float)_increment
         );
      
      newSize = size +
         _increment * increments;
 
      resize(newSize, writeLock);
      
   }
  
}


std::string
Database::toBits(const string& string) {

   std::string bits;
   
   for (const char& c: string) {
      for (int i = 0; i < CHAR_BIT; i++) {
         char mask = 0b10000000 >> i;
         bool bit = c & mask;
         bits.push_back(bit ? '1' : '0');
      }
   }
   
   return bits;
}



Database::Database(
   const string& path,
   const Size pageSize,
   const Size initialSize,
   const Size increment
)
 : File(
      path,
      Database::getPageAlignedSize(
         pageSize,
         initialSize,
         sizeof(Database::Data) + pageSize
      )
   ),
   _increment(
      Database::getPageAlignedSize(
         pageSize,
         increment
      )
   )
{

   mapFileToMemory();
   
   if (isNew()) {
      _pageSize = pageSize;
      sprintf(
         _data->version,
         "%s (%d x 8) bits",
         Database::version,
         _pageSize
      );
      _data->pageSize = _pageSize;
      _data->nextPointer = 0;
      _data->branches[0].left = 0;
      _data->branches[0].right = 0;
   }
   else
      _pageSize = _data->pageSize;
}
  

Database::~Database() {
   
   if (_memoryMap) {
      munmap(_memoryMap, fileSize());
      _memoryMap = NULL;
   }
   
   cout << "~Database::keys:" << keys.size() << endl;
}


void
Database::mapFileToMemory() {

   _memoryMap = mmap(
      NULL,
      fileSize(),
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      _fileNumber,
      0
   );
   setMemory();
}


void
Database::setMemory() {

   _data = (Data*)_memoryMap;
   
}


Size
Database::resize(
   const Size newSize,
   Database::WriteLock& writeLock
) {

   UniqueLock uniqueLock(writeLock);
   
   Size result;
   Size oldSize   = fileSize();
   Size pagedSize =
      getPageAlignedSize(_pageSize, newSize);
   
   if (pagedSize > oldSize) {

      File::resize(pagedSize);
      
      void* memoryMap =
         mremap(
            _memoryMap,
            oldSize,
            pagedSize,
            MREMAP_MAYMOVE
         );
      
      if (memoryMap == MAP_FAILED) {
         perror("Error remapping memory");
         throw "Error remapping memory";
      }
      
      _memoryMap = memoryMap;
      
      setMemory();

   }
   
   
   return pagedSize;
}


Size const
Database::getPageAlignedSize(
   const Size pageSize,
   const Size value,
   const Size minimum)
{

   Size newValue = value;
   
   Size mod =
      value % pageSize;
   
   if (mod != 0)
      newValue += (
         pageSize - mod
      );
      
   if (newValue < minimum)
      newValue = minimum;
      
   return newValue;

}

