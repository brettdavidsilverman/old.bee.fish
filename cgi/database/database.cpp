#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <string.h>

#include "database.h"

#undef DEBUG

using namespace std;

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

Database::Size
Database::readBits(Database::Pointer& pointer, const std::string& bits) {

   ReadLock lock(_access);
      
   Size read = 0;
   Size length = bits.length();
   Branch* branches = _data->branches;
   
   while (read < length) {
      const char& bit = bits[read];
      Branch& branch = branches[pointer];
      if (bit == '0') {
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

Database::Size
Database::writeBits(Database::Pointer& pointer, const std::string& bits) {

   Size length = bits.length();
   Size written = 0;
   
   // get upgradable access
   WriteLock writeLock(_access);

   grow(length, writeLock);
   
   Branch* branches = _data->branches;
   
   while (written < length) {
      const char& bit = bits[written];
      Branch& branch = branches[pointer];
      
      //cout << bit;
      if (bit == '0') {
         // left branch
         if (branch.left == 0) {
            pointer = ++(_data->nextPointer);
            branch.left = pointer;
         }
         else
            pointer = branch.left;
      }
      else {
         // right branch
         if (branch.right == 0) {
            pointer = ++(_data->nextPointer);
            branch.right = pointer;
         }
         else
            pointer = branch.right;
      }
      
      ++written;
   }
   
   return written;
   
}

void Database::grow(
   Database::Size length,
   Database::WriteLock& writeLock
   )
{
   
   Size newSize             =
   (
      sizeof(Header)          +
      (_data->nextPointer + length + 1) *
         sizeof(Branch)
   );
   
   if (newSize > _size) {
      
      UniqueLock
         uniqueLock(writeLock);
     
      resize(newSize + _data->pageSize);
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
   const File::Size pageSize, 
   const File::Size initialSize,
   const File::Size increment
 
) :
   File(
      path,
      getPageAlignedSize(
         initialSize,
         getpagesize()
      )
   ),
   _increment(increment)
{

   mapFileToMemory();

}
  
Database::~Database() {
   
   if (_memoryMap) {
      munmap(_memoryMap, _size);
      _memoryMap = NULL;
   }
   
}


Database::Size
Database::resize(
   const Database::Size newSize
) {
   Database::Size result;
   Database::Size oldSize   = _size;
   Database::Size pagedSize =
      getPageAlignedSize(newSize, _data->pageSize);
   
   if (oldSize < pagedSize) {
      
      result = ftruncate(
         _fileNumber,
         pagedSize
      );
         
      if (result != 0) {
         perror("Truncating database");
         throw "Error resizing database";
      }
         
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
      
      _size = pagedSize;
      
      setMemory();
   }
   
   
   return _size;
}
   
void Database::mapFileToMemory() {
   int fileNo = _fileNumber;
   _memoryMap = mmap(
      NULL,
      _size,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      fileNo,
      0
   );
   setMemory();
}

void Database::setMemory() {

   _data = (Data*)_memoryMap;
 
   
}

Database::Size const
Database::getPageAlignedSize(
   const Database::Size pageSize,
   const Database::Size value,
   const Database::Size minimum)
{

   if (value == 0)
      return 0;
   
   Database::Size newValue = value;
   if (newValue < minimum)
      newValue = minimum;
      
   Database::Size mod =
      newValue % pageSize;
   
   if (mod != 0)
      newValue += (
         pageSize - mod
      );
   
   return newValue;

}

