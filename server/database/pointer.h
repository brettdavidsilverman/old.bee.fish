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
      Branch& branch =
         _database->getBranch(_index);
         
      return (
         branch._left == 0 &&
         branch._right == 0
      );
   }
   
protected:

   void traverse(
      ostream& out,
      Index index
   ) const
   {
      Branch& branch =
         _database->getBranch(index);
      
      if (branch._left) {
         out << '1';
         traverse(
            out,
            branch._left
         );
      }
      else
         out << '0';
      
      if (branch._right) {
         out << '1';
         traverse(
            out, 
            branch._right
         );
      }
      else
         out << '0';
   }

   virtual bool writeBit(bool bit)
   {

      Branch& branch = 
         _database->getBranch(
            _index
         );

      // Update the branch
      //++branch._count;
      
      // Choose the path based on bit
      Index& index =
         bit ?
            branch._right :
            branch._left;
      /*
      if (bit)
         index = &(branch._right);
      else
         index = &(branch._left);
    */
      // If this path is empty...
      if (index == 0) {
      
         // Get the next index
         Index next =
            _database->getNextIndex();
         
         // Set the next path
         index = next;
            
         // Get the new branch
        // Branch& newBranch =
         //   _database->getBranch(next);
 
         // Set the new branchs fields
         //newBranch._parent = index;
        // newBranch._bit = bit;
       //  newBranch._count = 0;
 
         // Follow this path
        // index = next;
      }
      
      
      // save the index
      _index = index;
      
      if (_index >=
          _database->getLength() - 1)
         _database->resize();
         
      return true;
      
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
   
   virtual bool writeBit(bool bit)
   {
      if (_eof)
      {
         string str = "Past eof {";
         str += to_string(_index);
         str += "}";
         throw runtime_error(str);
      }
   
      Index index = _index;
      
      Branch& branch =
         _database->getBranch(index);
      
      if (bit)
         index = branch._right;
      else
         index = branch._left;
         
      if (index == 0)
         _eof = true;
      else
         _index = index;
         
      return !_eof;
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