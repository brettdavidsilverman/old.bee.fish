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

Database::Size Database::pageSize = getpagesize();

Database::Pointer
Database::walkPath(const std::string& string) {

   for (const char& c: string) {
      for (int i = 0; i < CHAR_BIT; ++i) {
         char mask = (0b10000000 >> i);
         bool bit = (c & mask);
         walkBit(bit);
      }
   }
   
   return walkBit(false);
}

Pointer
Database::walkBit(bool bit) {

   Pointer index = pointer;
      
   if (bit == Right)
      ++index;
    
   if (index >= _length)
      resize(_size + _increment);
      
   if (_array[index] == 0) {
      (*_last) += 2;
      _array[index] = *_last;
   }
      
   return (pointer = _array[index]);
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
      strcpy(_data->version, VERSION);
   }
   else if (strcmp(_data->version, VERSION) != 0) {
      std::string error = "Invalid file version.";
      error += " Expecting ";
      error += VERSION;
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

