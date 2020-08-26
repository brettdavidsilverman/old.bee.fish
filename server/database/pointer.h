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

class Pointer :
   public PowerEncoding
{

public:
   Pointer( Database* database,
            Index index = 0 ) :
      PowerEncoding(),
      _database(database),
      _index(index),
      _array(_database->_array),
      _last(_database->_last)
   {
   }
   
   Pointer(const Pointer& source) :
      PowerEncoding(),
      _database(source._database),
      _index(source._index),
      _array(source._array),
      _last(source._last)
   {
   }
   
   virtual Index& operator*() {
      return _index;
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
   
   Pointer& operator << (bool bit)
   {
      writeBit(bit);
      return *this;
   }
   
   Pointer& operator <<
   (const string& str)
   {
      PowerEncoding::operator << (str);
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

   virtual void writeBit(bool bit)
   {
      Index index = _index;
   
      // If right, select the next column
      if (bit)
         ++index;
    
      // If this row/column is empty...
      if (_array[index] == 0) {
      
         // Grow last by two columns
         (*_last) += 2;
         
         // Check to see if the database
         // is long enough
         if ( *_last  >=
              _database->_length )
            _database->resize();
            
        
         // Set the row/column
         _array[index] = *_last;
         
      }
      
      // set the last index
      _index = _array[index];
      

   }
   

   Database* _database;
   Index _index;
   Index* _array;
   Index* _last;
   
};

class ReadOnlyPointer :
   public Pointer
{

public:
   ReadOnlyPointer( Pointer& pointer ) :
      Pointer(pointer)
   {
 
      _eof = isDeadEnd();
   }
   
   virtual void writeBit(bool bit)
   {
      if (_eof)
         return;
   
      Index index = _index;
      
      if (bit)
         index++;
         
      index = _array[index];
      
      if (index == 0)
         _eof = true;
      else
         _index = index;
   }
   
   virtual bool eof()
   {
      return _eof;
   }
protected:

   bool _eof;
};

}

#endif