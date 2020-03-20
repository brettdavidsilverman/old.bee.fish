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

const File::Size Database::pageSize = getpagesize();

Database::Pointer
Database::putString(const Database::Pointer start, const string& string) {

   std::string bits = toBits(string);
   
   Pointer pointer = start;
   
   Size bitsRead = readBits(
      pointer,
      bits
   );
   
   if (bitsRead < bits.length()) {

      bits = bits.substr(bitsRead);
     
      Size written = writeBits(
         pointer,
         bits
      );
   }
   
   return pointer;
}

File::Size
Database::readBits(
   Database::Pointer& pointer,
   const std::string& bits
) {
   
   ReadLock readLock(_access);
   
   Size read = 0;
   Size length = bits.length();
   
   while (read < length) {
      
      Branch& branch =
         *(_branches + pointer);
      
      //if (branch.locked)
      //   break;
         
      if (bits[read] == '0') {
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
      
      ++read;
   }
   
   return read;
   
}

File::Size
Database::writeBits(Database::Pointer& pointer, const std::string& bits) {
   
   WriteLock writeLock(_access);
   
   enlarge(bits.length());//, writeLock);
   
  // Branch& gate = *(_branches + pointer);
  // gate.locked = true;
   
   Size length = bits.length();
   Size written = 0;

   while (written < length) {
   
      Branch& branch = 
         *(_branches + pointer);
      
      if (bits[written] == '0') {
      
         // left branch
         if (branch.left == 0)
            branch.left =
               ++(*_nextPointer);
               
            pointer = branch.left;
      }
      else {
      
         // right branch
         if (branch.right == 0)
            branch.right =
               ++(*_nextPointer);
               
         pointer = branch.right;
      }
      
      ++written;
   }
   
  // gate.locked = false;
   pointer = *_nextPointer;
   
   return written;
   
}

void Database::enlarge(
   const File::Size length
  // WriteLock& writeLock
)
{

   Size newSize =
      sizeof(Pointer)          +
      (*_nextPointer           +
        length * CHAR_BIT)     *
      sizeof(Branch);
   
   newSize =
      getPageAlignedSize(newSize);
   
   Size size = fileSize();
   
   if (newSize > size) {
   
      File::Size difference =
         newSize - size;
         
      File::Size increments =
         ceil(
            (float)difference /
            (float)_increment
         );
      
      newSize = size +
         _increment * increments;
 
      resize(newSize);//, writeLock);
      
   }
  
}

std::string Database::toBits(const std::string& string) {

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
   const File::Size initialSize,
   const File::Size increment
)
 : File(
      path,
      Database::getPageAlignedSize(
         initialSize
      )
   ),
   _increment(
      Database::getPageAlignedSize(
         increment
      )
   )
{


   mapFileToMemory();
}
  
Database::~Database() {
   
   if (_memoryMap) {
      munmap(_memoryMap, fileSize());
      _memoryMap = NULL;
   }
   
}

void Database::mapFileToMemory() {

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

void Database::setMemory() {

   _nextPointer = (File::Size*)_memoryMap;
   _branches = (Branch*)(
      (char*)_memoryMap + sizeof(Size)
   );
   
}

File::Size
Database::resize(
   const File::Size newSize,
   WriteLock& writeLock
) {
   
   UniqueLock uniqueLock(writeLock);
    
   File::Size result;
   File::Size oldSize   = fileSize();
   File::Size pagedSize =
      getPageAlignedSize(newSize);
   
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

File::Size const
Database::getPageAlignedSize(
   const File::Size value)
{

   if (value <= 0)
      return Database::pageSize;
   
   File::Size newValue = value;
   
   File::Size mod =
      value % Database::pageSize;
   
   if (mod != 0)
      newValue += (
         Database::pageSize - mod
      );
      
   return newValue;

}

