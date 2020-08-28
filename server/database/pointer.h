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
      _index(index)
   {
   }
   
   Pointer(const Pointer& source) :
      PowerEncoding(),
      _database(source._database),
      _index(source._index)
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
      
      return *this;
   }
   
   bool operator == (const Index& rhs)
   {
      return (_index == rhs);
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
         _database->_array[_index    ]
            == 0 &&
         _database->_array[_index + 1]
            == 0
      );
   }
   
protected:

   void traverse(
      ostream& out,
      Index index
   ) const
   {
      Index* array = _database->_array;
      
      if (array[index]) {
         out << '1';
         traverse(out, array[index]);
      }
      else
         out << '0';
      
      if (array[index + 1]) {
         out << '1';
         traverse(out, array[index + 1]);
      }
      else
         out << '0';
   }

   virtual void writeBit(bool bit)
   {
      Index index = _index;
      Index* array = _database->_array;
      Index* last = _database->_last;
      
      // If right, select the next column
      if (bit)
         ++index;
    
      // If this row/column is empty...
      if (!array[index]) {
      
         // Grow last by two columns
         *last += 2;
         
         // Check to see if the database
         // is long enough
         if ( *last  >=
              _database->_length )
         {
            _database->resize();
            array = _database->_array;
            last = _database->_last;
         }
        
         // Set the row/column
         array[index] = *last;
         
      }
      
      // set the last index
      _index = array[index];
      

   }
   

   Database* _database;
   Index _index;
   
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
         
      index = _database->_array[index];
      
      if (index == 0)
         _eof = true;
      else
         _index = index;
   }
   
   ReadOnlyPointer& operator=
   (const Index& index)
   {
      _index = index;
      return *this;
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