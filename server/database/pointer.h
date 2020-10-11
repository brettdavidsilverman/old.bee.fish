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
   
      Pointer( Database& database,
               Index index = Index::root ) :
         PowerEncoding(),
         _database(database),
         _index(index),
         _pageIndex(index._pageIndex)
      {
         readPage();
      }
   
      Pointer(const Pointer& source) :
         PowerEncoding(),
         _database(source._database),
         _index(source._index),
         _page(source._page),
         _pageIndex(source._pageIndex)
      {
      }
   
      virtual ~Pointer()
      {
         if (_isDirty)
            writePage();
      }
      
      virtual void writeBit(bool bit)
      {

         Branch& branch = 
            getBranch(
               _index
            );
      
         // Choose the path based on bit
         Index& index =
            bit ?
               branch._right :
               branch._left;

         // If this path is empty...
         if (!index)
         {
            // Get the next index
            index = _database.getNextIndex();
            
            _isDirty = true;

#ifdef DEBUG
            cerr << '+';
#endif
         }
         else 
         {
#ifdef DEBUG
            cerr << '=';
#endif
         }
#ifdef DEBUG
            cerr << (bit ? '1' : '0');
#endif
         
         // save the index
         _index = index;
         
  
      }
      
      virtual bool readBit()
      {
         Branch& branch =
            getBranch(_index);
            
         if (branch._left)
         {
            _index = branch._left;
            return false;
         }
         else if (branch._right)
         {
            _index = branch._right;
            return true;
         }
         
         throw runtime_error("Past end of file");
         
      }
      
      virtual bool peekBit()
      {
         Branch& branch =
            getBranch(_index);
               
         if (branch._left)
            return false;
         else if (branch._right)
            return true;
               
         throw runtime_error("Past end of file");
      }
      
      virtual Index& operator*() {
         return _index;
      }

  
      void traverse(ostream& out)
      {
         out << '1';
      
         Branch& branch =
            getBranch(_index);
         
         traverse(out, branch._left);
         traverse(out, branch._right);
      }
   
      friend ostream& operator <<
      (ostream& out, Pointer& pointer)
      {
         pointer.traverse(out);
      
         return out;
      }
   
   
      Pointer& operator=(const Index& index)
      {
         if (_isDirty)
            writePage();
            
         _index = index;
         readPage();
         return *this;
      }
   
      Pointer& operator=(const Pointer& rhs)
      { 
         if (_isDirty)
            writePage();
            
         _index = rhs._index;
         readPage();
         return *this;
      }
   
      bool operator == (const Index& rhs)
      {
         return (_index == rhs);
      }
   
      template<class T>
      Pointer& operator <<
      (const T& value)
      {
         PowerEncoding::operator << (value);
         return *this;
      }

      const Index& index() const
      {
         return _index;
      }
      
      bool isDeadEnd()
      {
         Branch& branch = getBranch(_index);
         return branch.isDeadEnd();
      }
      
      Branch& getBranch()
      {
         Branch& branch =
            getBranch(_index);
         return branch;
      }
      
      template<typename Key>
      Pointer operator [] (const Key& key) const
      {
         Pointer start(*this);
         start << key;
         return start;
      }
      
   
   protected:

      void traverse(
         ostream& out,
         Index index
      )
      {
         if (!index)
         {
            out << '0';
            return;
         }
      
         Branch& branch =
            getBranch(index);
      
         out << '1';
      
         traverse(
            out,
            branch._left
         );
      
         traverse(
            out, 
            branch._right
         );

      }

      void first(ostream& out)
      {
         Branch& branch =
            getBranch(_index);
            
         while (!branch.isDeadEnd())
         {
            if (branch._left)
            {
               out << '0';
               _index = branch._left;
            }
            else
            {
               out << '1';
               _index = branch._right;
            }
            
            branch =
               getBranch(_index);
         }
         
      }
      
      Branch& getBranch(const Index& index)
      {

         if (index._pageIndex !=
             _pageIndex) 
         {

            // Page fault
            if (_isDirty)
               writePage();
            
            _pageIndex = index._pageIndex;
            
            readPage();
            
         }
        
         return
            _page
            ._branchPage
            ._branches[
               _index._branchIndex
            ];
            
      }
      
      void writePage()
      {
         _database.writePage(
            _pageIndex,
            _page
         );
         _isDirty = false;
      }
      
      void readPage()
      {
         _database.readPage(
            _pageIndex,
            _page
         );
         _isDirty = false;
      }
      
      
      Database& _database;
      
   public:
      Index _index;
      Page _page;
      File::Size _pageIndex = 0;
      bool _isDirty = false;
   };

   class ReadOnlyPointer :
      public Pointer
   {
   
   public:

      ReadOnlyPointer( Pointer& pointer ) :
        Pointer(pointer)
      {
 
         Branch& branch = getBranch(_index);
         
         _eof = branch.isDeadEnd();
      }
   
      virtual void writeBit(bool bit)
      {
         Branch& branch =
            getBranch(_index);
      
         if (branch.isDeadEnd())
         {
            stringstream s;
            s << "Past eof "
              << _index;
            throw runtime_error(s.str());
         }
         
         Index& index =
            bit ?
               branch._right :
               branch._left;
         
         if (!index)
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