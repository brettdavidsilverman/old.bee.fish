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
   long _bitCount = 0;
   
   Pointer( Database* database,
            Index index = {0, 0} ) :
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
   
   friend Database& operator <<
   (Database& db, const Pointer& pointer)
   {
      db << pointer._index;
      return db;
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
         !branch._left &&
         !branch._right
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
      ++branch._count;
      
      // Choose the path based on bit
      Index& index =
         bit ?
            branch._right :
            branch._left;

      // If this path is empty...
      if (!index) {
      
         // Get the next index
         Index& next =
            _database->getNextIndex();
         
        // cerr << "Next index " << next << endl;
         
         // Get the new branch
         Branch& newBranch =
            _database->getBranch(next);
 
         // Set the new branchs fields
         newBranch._parent = index;
         newBranch._bit = bit;
         newBranch._count = 1;
         
         // Follow this path
         index = next;
      }
      
      
      // save the index
      _index = index;
      
      _database->_isDirty = true;
      
      ++_bitCount;
      
      return true;
      
   }
   

   Database* _database;
public:
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
         stringstream s;
         s << "Past eof "
           << _index;
         throw runtime_error(s.str());
      }
   
      Branch& branch =
         _database->getBranch(_index);
      
      Index index =
         bit ?
            branch._right :
            branch._left;
         
      if (!index)
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