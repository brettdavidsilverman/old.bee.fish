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
   
   
   const Index& index() const
   {
      return _index;
   }
   
   bool isDeadEnd() {
      Fork& fork =
         _database->_data->array[_index];
         
      return (
         fork.left == 0 &&
         fork.right == 0
      );
   }
   
protected:

   void traverse(
      ostream& out,
      Index index
   ) const
   {
      Fork* array =
         _database->_data->array;
      
      if (array[index].left) {
         out << '1';
         traverse(
            out,
            array[index].left
         );
      }
      else
         out << '0';
      
      if (array[index].right) {
         out << '1';
         traverse(
            out, 
            array[index].right
         );
      }
      else
         out << '0';
   }

   virtual void writeBit(bool bit)
   {
      Index index = _index;
      
      Fork& fork = 
         _database->_data->array[index];

      ++fork.count;
      
      if (bit)
         index = fork.right;
      else
         index = fork.left;
    
      // If this row/column is empty...
      if (index == 0) {
      
         Index& next =
            _database->
            _data->
            header.next;

         // Grow next
         ++next;
         
         // Check to see if the database
         // is long enough
         if ( next  >=
              _database->_length )
         {
#ifdef DEBUG
            cout << "Resizing..." << endl;
#endif
            _database->resize();
            
            fork = 
               _database->
               _data->
               array[index];
               
            next =
               _database->
               _data->
               header.next;
         }
         
         Index parent = index;
         
         // Set the branch
         if (bit)
            index = fork.right = next;
         else
            index = fork.left = next;
         
         // Set the new branchs parent
         fork =
            _database->
            _data->
            array[index];
            
         fork.parent = parent;
      }
      
      // save the index
      _index = index;
      

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
      Fork& fork =
         _database->_data->array[index];
      
      if (bit)
         index = fork.right;
      else
         index = fork.left;
         
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