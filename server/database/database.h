#ifndef BEE_FISH_DATABASE__DATABASE_H
#define BEE_FISH_DATABASE__DATABASE_H

#include <sys/mman.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <mutex>
#include <shared_mutex>
#include <string.h>
#include <condition_variable>

#include "file.h"
#include "version.h"

using namespace std;

namespace bee::fish::database {

class Pointer;

// Store a memory mapped _array of
// [left, right] elements.
// The pointer points to the next elememt
// in the _array.
// A zero is stored if the branch
// hasnt been visited yet.
// The _next points to the furthest element.
class Database : 
   public File {
public:

   static const Size pageSize = 4096;
   
   struct Index
   {
      File::Size _pageIndex;
      File::Size _branchIndex;

      Index& operator++()
      {
         if (++_branchIndex == _branchesPerPage)
         {
            _branchIndex = 0;
            ++_pageIndex;
         }
         return *this;
      }

      bool operator !()
      {
         return
             (_pageIndex   == 0 &&
              _branchIndex == 0);
      }
      
      operator bool()
      {
         return
            (_pageIndex   != 0 ||
             _branchIndex != 0);
      }
      
      bool operator == (const Index& rhs)
      {
         return
            _pageIndex
               == rhs._pageIndex &&
            _branchIndex
               == rhs._branchIndex;
      }
      
      friend ostream& operator <<
      (ostream& out, const Index& index)
      {
         out << "{"
             << index._pageIndex
             << ","
             << index._branchIndex
             << "}";
             
          return out;
      }
   };
   
   typedef unsigned long long Count;
   
   Database(
      const string& filePath,
      const Size initialSize = 1000 * 1000,
      const Size increment = 1000 * 1000
   ) :
      File(
         filePath,
         getPageAlignedSize(
            initialSize,
            getpagesize()
         )
      ),
      _increment(increment)
   {
      read(&_header, 1, sizeof(Header));
      
      if (isNew()) {
         strcpy(_header._version, BEE_FISH_DATABASE_VERSION);
         write(&_header, 1, sizeof(Header));
      }
      else if (strcmp(_header._version, BEE_FISH_DATABASE_VERSION) != 0) {
         std::string error = "Invalid file version.";
         error += " Expecting ";
         error += BEE_FISH_DATABASE_VERSION;
         error += ". Got ";
         error += _header._version;
         throw runtime_error(error);
      }
      
      _pageCount =
         _size / pageSize;
         
      _pageIndex = 0;
      _isDirty = false;
      *this >> _page;
   }

   ~Database()
   {
   
      if (_isDirty)
      {
         *this << _page;
         _isDirty = false;
      }
      seek(0, SEEK_SET);
      write(&_header, 1, sizeof(Header));
   }
   
   struct Branch
   {
      Index _parent;
      Count _count;
      bool  _bit;
      Index _left;
      Index _right;
      /*
      Branch& operator --()
      {
         Index parent = _parent;
         --_count;
         if (_count == 0)
         {
            memset(this, 0, sizeof(Branch));
         }
         return --_parent;
      }
      */
   };
   
   static const File::Size
      _branchesPerPage =
         pageSize / sizeof(Branch);
     
   struct BranchPage
   {
      Branch _branches[
         _branchesPerPage
      ];
   };
   
   struct FreePage
   {
      Index _nextFreePage;
   };
   
   union Page
   {
      FreePage   _freePage;
      BranchPage _branchPage;
   };
   
   
   
   Branch& getBranch(Index& index)
   {
      if (index._pageIndex !=
          _pageIndex) 
      {
         // Page fault
       //  cerr << "Page fault " << index._pageIndex << endl;
         *this << index;
      
      }
      
      return
         _page
         ._branchPage
         ._branches[index._branchIndex];
   }
   
   Index& getNextIndex()
   {
      Index& next = ++(_header._next);
      _isDirty = true;
     
      if ( next._pageIndex >=
           _pageCount - 1 )
         resize();

      return next;
   }
   
   File::Size& getPageCount()
   {
      return _pageCount;
   }
   
   friend Database& operator <<
   (Database& db, const Index& index)
   {
      if ( index._pageIndex ==
           db._pageIndex )
         return db;
         
      if (db._isDirty) 
      {
         db << db._page;
         db._isDirty = false;
      }
      
      db._pageIndex = index._pageIndex;
      
      db >> db._page;
      
      return db;
   }
   
   friend Database& operator <<
   (Database& db, const Page& page)
   {
      Size offset =
         pageSize * (db._pageIndex + 1);
         
      db.seek(offset, SEEK_SET);
      
      db.write(
         &page,
         1,
         pageSize
      );
         
      return db;

   }
   
   friend Database& operator >>
   (Database& db, Page& page)
   {
      Size offset =
         pageSize * (db._pageIndex + 1);
         
      db.seek(offset, SEEK_SET);
      
      db.read(
         &page,
         1,
         pageSize
      );
      
      return db;
   }
   
   
private:
   
   struct Header {
      // Page aligned header
      union {
         char buffer[pageSize];
         struct {
            char   _version[256];
            Index  _next;
         };
      };
   };
   
   Header _header;
   Page _page;
   File::Size _pageIndex = 0;
   File::Size _pageCount = 0;
  
   const Size _increment;
   

public:
   bool _isDirty = false; 
   virtual Size resize(
      Size size = 0
   )
   {
      if (size == 0)
         size = _size + _increment;
     
      Size newSize =
         getPageAlignedSize(size);
   
      File::resize(newSize);

      _pageCount =
         _size / pageSize;
         
      return _size;

   }
   
protected:
   static Size getPageAlignedSize
   (
      const Size value,
      const Size minimum = 0
   )
   {

      if (value == 0)
         return 0;
   
      Size newValue = value;
      if (newValue < minimum)
         newValue = minimum;
      
      Size mod =
         newValue % pageSize;
   
      if (mod != 0)
         newValue += (
            pageSize - mod
         );
   
      return newValue;

   }
   
   friend ostream& operator <<
   (ostream& out, const Database& db)
   {
      out << "Database: " 
          << db._header._version
          << endl
          << db.filePath
          << " "
          << endl
          << "Next: "
          << db._header._next
          << endl
          << "Page count: "
          << db._pageCount
          << endl
          << "Page index: "
          << db._pageIndex
          << endl
          << "Size: "
          << db._size
          << endl;
          
      return out;
   }

};

typedef Database::Index Index;
typedef Database::Branch Branch;

}

#endif