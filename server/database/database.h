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
            ++_pageIndex;
            _branchIndex = 0;
         }
         return *this;
      }
      
      operator bool()
      {
         return
            _pageIndex   != 0 ||
            _branchIndex != 0;
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
      read(&_header, sizeof(Header), 1);
      
      if (isNew()) {
         strcpy(_header._version, BEE_FISH_DATABASE_VERSION);
         write(&_header, sizeof(Header), 1);
      }
      else if (strcmp(_header._version, BEE_FISH_DATABASE_VERSION) != 0) {
         std::string error = "Invalid file version.";
         error += " Expecting ";
         error += BEE_FISH_DATABASE_VERSION;
         error += ". Got ";
         error += _header._version;
         throw runtime_error(error);
      }
      
   }

   ~Database()
   {
   
      if (_isDirty)
      {
         *this << _pageIndex << _page;
         _isDirty = false;
      }
   }
   
   struct Branch
   {
      Index _parent;
      Count _count;
      bool  _bit;
      Index _left;
      Index _right;
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
          _pageIndex._pageIndex) 
      {
         *this << index;
         *this >> _page;
      
         if ( index._pageIndex >
             (_header._pageCount - 1) )
            _header._pageCount =
               index._pageIndex + 1;
      }
      
      return
         _page
         ._branchPage
         ._branches[index._branchIndex];
   }
   
   Index& getNextIndex()
   {
      Index& next = ++(_data->_next);
      return next;
   }
   
   File::Size& getPageCount()
   {
      return _header._pageCount;
   }
   
   friend Database& operator <<
   (Database& db, const Index& index)
   {
      Size offset =
         sizeof(Header) +
         sizeof(Page) * index._pageIndex;
         
      db.seek(offset, SEEK_SET);
      
      return db;
   }
   
   friend Database& operator <<
   (Database& db, const Page& page)
   {
      size_t result =
         db.write (
            &page, 
            sizeof(Page),
            1
         );
         
      return db;

   }
   
   friend Database& operator >>
   (Database& db, Page& page)
   {
      size_t result =
         db.read(
            &page,
            sizeof(Page),
            1
         );
      return db;
   }
   
   
private:
   
   struct Header {
      union {
         char buffer[pageSize];
         struct {
            char        _version[256];
            File::Size  _pageCount;
         };
      };
   };
   
   Header _header;
   Page _page;
   Index _pageIndex = {0, 0};
   File::Size _pageCount = 0;
   bool _isDirty = false; 
   const Size _increment;
   

public:
 
   virtual Size resize(
      Size size = 0
   )
   {
      if (size == 0)
         size = _size + _increment;
         
      Size oldSize = _size;
      Size newSize =
         getPageAlignedSize(size);
   
      File::resize(newSize);

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
          << db._data->_version
          << endl
          << db.filePath
          << " "
          << endl
          << "Next: "
          << db._data->_next
          << endl
          << "Page count: "
          << db._pageCount
          << endl
          << "Branches per page: "
          << _branchesPerPage
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