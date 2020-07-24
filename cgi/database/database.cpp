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

namespace bee::fish::database {

Database::Size Database::pageSize = getpagesize();

Database::Pointer
Database::walkPath(const std::string& string) {

   for (const char& c: string) {
      walkBit(0b10000000 & c);
      walkBit(0b01000000 & c);
      walkBit(0b00100000 & c);
      walkBit(0b00010000 & c);
      walkBit(0b00001000 & c);
      walkBit(0b00000100 & c);
      walkBit(0b00000010 & c);
      walkBit(0b00000001 & c);
   }
   
   
   return walkBit(false);
}

Database::Pointer
Database::walkPath(const std::string& string, Pointer& index) const {

   for (const char& c: string) {
      if (walkBit(0b10000000 & c, index) == 0)
         return 0;
      if (walkBit(0b01000000 & c, index) == 0)
         return 0;
      if (walkBit(0b00100000 & c, index) == 0)
         return 0;
      if (walkBit(0b00010000 & c, index) == 0)
         return 0;
      if (walkBit(0b00001000 & c, index) == 0)
         return 0;
      if (walkBit(0b00000100 & c, index) == 0)
         return 0;
      if (walkBit(0b00000010 & c, index) == 0)
         return 0;
      if (walkBit(0b00000001 & c, index) == 0)
         return 0;
   }
   
   return walkBit(false, index);
}

inline Pointer
Database::walkBit(bool bit) {

   Pointer index = pointer;
   
   // If right, select the next column
   if (bit == true)
      ++index;
    
   // Resize by increment if
   // our index is larger
   if (index >= _length)
      resize(_size + _increment);
   
   // If this row/column is empty...
   if (_array[index] == 0) {
      // Grow last by two columns
      (*_last) += 2;
      // Set the row/column
      _array[index] = *_last;
   }
      
   // return the last pointer
   return (pointer = _array[index]);
}

inline Pointer
Database::walkBit(bool bit, Pointer& index) const {

   // If right, select the next column
   if (bit == true)
      ++index;
      
   // return the next pointer
   return index = _array[index];
}

ostream& operator << (ostream& out, const Database& db) {
   db.traverse(out, db.pointer);
   return out;
}

void Database::traverse(ostream& out, Pointer pointer) const {
   if (_array[pointer]) {
      out << '1';
      traverse(out, _array[pointer]);
   }
   else
      out << '0';
      
   if (_array[pointer + 1]) {
      out << '1';
      traverse(out, _array[pointer + 1]);
   }
   else
      out << '0';
}

Database::Database(
   const string& path,
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
   
   if (isNew()) {
      strcpy(_data->version, BEE_FISH_DATABASE_VERSION);
   }
   else if (strcmp(_data->version, BEE_FISH_DATABASE_VERSION) != 0) {
      std::string error = "Invalid file version.";
      error += " Expecting ";
      error += BEE_FISH_DATABASE_VERSION;
      error += ". Got ";
      error += _data->version;
      perror(error.c_str());
      throw error;
   }
}
  
Database::~Database() {
   
   if (_memoryMap) {
      munmap(_memoryMap, _size);
      _memoryMap = NULL;
   }
   
}

void Database::mapFileToMemory() {

   _memoryMap = mmap(
      NULL,
      _size,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      _fileNumber,
      0
   );
   
   if (_memoryMap == MAP_FAILED) {
      perror("Error mapping memory");
      throw "Error mapping memory";
   }
   
   setData();
   
}

Database::Size
Database::resize(
   const Database::Size size
) {
   Database::Size result;
   Database::Size oldSize = _size;
   Database::Size newSize =
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
         perror("Error remapping memory");
         throw "Error remapping memory";
      }

      _memoryMap = memoryMap;
      
      File::resize(newSize);

      setData();
   }
   
   setLength(); 
   return _size;
}

void
Database::setData() {
   _data = (Data*)_memoryMap;
   setLength();
   _array = _data->array;
   _last = &(_data->last);
}

void
Database::setLength() {
   _length = (
      _size - sizeof(Header)
   ) / sizeof(Pointer);
}

Database::Size const
Database::getPageAlignedSize(
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

}

