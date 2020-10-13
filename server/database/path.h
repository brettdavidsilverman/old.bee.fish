#ifndef BEE_FISH_DATABASE__PATH_H
#define BEE_FISH_DATABASE__PATH_H

#include <optional>
#include <iostream>
#include "../power-encoding/power-encoding.h"
#include "file.h"
#include "database.h"
#include "page-cache.h"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

   class Path :
      public PowerEncoding
   {

   protected:
      Database&   _database;
      Index       _index;
      CachedPage* _cachedPage;
      
   public:
   
      Path( Database& database,
               Index index = Index::root ) :
         PowerEncoding(),
         _database(database),
         _index(index),
         _cachedPage(NULL)
      {
         _cachedPage =
            _database[_index._pageIndex];
      }
   
      Path(const Path& source) :
         PowerEncoding(),
         _database(source._database),
         _index(source._index),
         _cachedPage(source._cachedPage)
      {
      }
   
      virtual ~Path()
      {
      }
      
      virtual void writeBit(bool bit)
      {
      
#ifdef DEBUG
         cerr << (bit ? '1' : '0');
         cerr << _index;
#endif

         Branch& branch =
            getBranch(
               _index
            );
      
         if (bit)
         {
            if (!branch._right)
            {
               branch._right = 
                  _database.getNextIndex();
               _cachedPage->_isDirty = true;
#ifdef DEBUG
               cerr << '+';
#endif
            }
#ifdef DEBUG
            else
               cerr << '=';
#endif
               
            _index = branch._right;
         }
         else
         {
            if (!branch._left)
            {
               branch._left = 
                  _database.getNextIndex();
               _cachedPage->_isDirty = true;
#ifdef DEBUG
               cerr << '+';
#endif
            }
#ifdef DEBUG
            else
               cerr << '=';
#endif
            _index = branch._left;
         }
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
      (ostream& out, Path& pointer)
      {
         pointer.traverse(out);
      
         return out;
      }
   
   
      Path& operator=(const Index& index)
      {
         _index = index;
         _cachedPage =
            _database[_index._pageIndex];
         return *this;
      }
   
      Path& operator=(const Path& rhs)
      { 
         _index = rhs._index;
         _cachedPage =
            _database[_index._pageIndex];
         return *this;
      }
   
      bool operator == (const Index& rhs)
      {
         return (_index == rhs);
      }
   
      Path& operator <<
      (const bool& value)
      {
         writeBit(value);
         return *this;
      }
      
      template<class T>
      Path& operator <<
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
      Path operator [] (const Key& key) const
      {
         Path start(*this);
         start << key;
         return start;
      }
      
      Path& remove()
      {
         throw runtime_error("Not implemented");
         Branch& branch = getBranch();
         
         return *this;
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
             _cachedPage->_pageIndex) 
         {
#ifdef DEBUG
            // Page fault
            cerr << "{Page Fault: "
                 << index._pageIndex
                 << "}";
#endif
            _cachedPage =
               _database[index._pageIndex];
            
         }
        
         return
            _cachedPage
               ->_page
               ->_branchPage
               ._branches[
                  _index._branchIndex
               ];
            
      }
      
      
   };

   class ReadOnlyPath :
      public Path
   {
   
   public:

      ReadOnlyPath( Path& pointer ) :
        Path(pointer)
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
   
      ReadOnlyPath& operator=
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