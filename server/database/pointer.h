#ifndef BEE_FISH_DATABASE__POINTER_H
#define BEE_FISH_DATABASE__POINTER_H

#include <optional>
#include <iostream>
#include "../power-encoding/power-encoding.h"
#include "file.h"
#include "database.h"


using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

class Pointer : public PowerEncoding<ostream>
{
protected:
   ostream& _log;
   
public:
   Pointer( Database* database,
            Index value = 0,
            ostream& log = clog ) :
      PowerEncoding(log),
      _log(log),
      _database(database),
      _index(value),
      _array(_database->_array),
      _last(_database->_last)
   {
   }
   
   Pointer(const Pointer& source) :
      PowerEncoding(source._log),
      _log(source._log),
      _database(source._database),
      _index(source._index),
      _array(source._array),
      _last(source._last)
   {
   }
   
   virtual Index& operator*() {
      return _index;
   }
   
   Pointer& walkPath(const string& bits)
   {
      // Resize by increment if
      // our index is larger
      File::Size size =
         _database->fileSize();
      bool resize = false;
      while ( (_index     + 
              bits.size() ) *
              sizeof(Index) * 2
                 >= size )
      {
         size += _database->increment();
         resize = true;
      }
     
      if (resize)
         _database->resize(size);
      
      
      for (const char& c: bits) {
         walkBit(c == '1');
      }
      
      return *this;
   }
   
   

   optional<Pointer> walkPath(
      const string& bits
   ) const
   {
      Index index = _index;
      
      for (const char& c: bits)
      {
         if (walkBit(c == '1', index) == 0)
            return nullopt;
      }
         
      return Pointer(_database, index);
   }

  
   void traverse(ostream& out) const {
      traverse(out, _index);
   }
   
   friend ostream& operator <<
   (ostream& out, const Pointer& pointer)
   {
      pointer.traverse(out);
      return out;
   }
   
   Pointer& operator=(const Index& index) {
      _index = index;
      return *this;
   }
   
   Pointer& operator=(const Pointer& rhs)
   { 
      _database = rhs._database;
      _index    = rhs._index;
      _array    = rhs._array;
      _last     = rhs._last;
      
      return *this;
   }
   
   Pointer& operator << (char bit)
   {
      if (bit == '0')
         walkBit(false);
      else if (bit == '1')
         walkBit(true);
      else
         throw runtime_error("Invalid bit");
         
      return *this;
   }
   
   bool isDeadEnd() {
      return (
         _array[_index] == 0 &&
         _array[_index + 1] == 0
      );
   }
   
protected:

   void traverse(
      ostream& out,
      Index index
   ) const
   {
      if (_array[index]) {
         out << '1';
         traverse(out, _array[index]);
      }
      else
         out << '0';
      
      if (_array[index + 1]) {
         out << '1';
         traverse(out, _array[index + 1]);
      }
      else
         out << '0';
   }

   Pointer& walkBit(bool bit)
   {
      Index index = _index;
   
      // If right, select the next column
      if (bit)
         ++index;
    
      // If this row/column is empty...
      if (_array[index] == 0) {
         // Grow last by two columns
         (*_last) += 2;
         // Set the row/column
         _array[index] = *_last;
      }
      
      // set the last index
      _index = _array[index];
      
      _log << (bit ? '1' : '0');
      
      return *this;

   }
   

   Index walkBit(
      bool bit,
      Index& index
   ) const
   {
      // If right, select the next column
      if (bit == true)
         ++index;
      
      // return the next pointer
      return index = _array[index];

   }

   Database* _database;
   Index _index;
   Index* _array;
   Index* _last;
   
};

}

#endif