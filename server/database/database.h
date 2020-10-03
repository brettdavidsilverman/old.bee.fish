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
#include "config.h"
#include "version.h"
#include "index.h"
#include "page.h"
#include "branch.h"


using namespace std;

namespace bee::fish::database {

   class Pointer;

   // Store [left, right] branch elements.
   // A zero is stored if the branch
   // hasnt been visited yet.
   // The _next points to the furthest element.
   class Database : 
      public File
   {
      
   public:
   
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
      
         if (isNew())
         {
            memset(&_header, '\0', sizeof(_header));
            _header._pageSize = PAGE_SIZE;
            strcpy(_header._version, BEE_FISH_DATABASE_VERSION);
            seek(0, SEEK_SET);
            write(&_header, 1, sizeof(Header));
         }
    
         seek(0, SEEK_SET);
         read(&_header, 1, sizeof(Header));
      
         if (strcmp(_header._version, BEE_FISH_DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Expecting ";
            error += BEE_FISH_DATABASE_VERSION;
            error += ". Got ";
            error += _header._version;
            throw runtime_error(error);
         }
         
         if (_header._pageSize != PAGE_SIZE)
         {
            std::string error = "Invalid file page size.";
            error += " Expecting ";
            error += to_string(PAGE_SIZE);
            error += ". Got ";
            error += to_string(_header._pageSize);
            throw runtime_error(error);
         }
         
      
         _pageCount =
            _size / PAGE_SIZE;
         _isDirty = false;
         _pageIndex = -1;
         readPage(0);
      }

      ~Database()
      {
   
         if (_isDirty)
         {
            savePage();
         }
         seek(0);
         write(&_header, 1, sizeof(Header));
      }
   
      Branch& getBranch(const Index& index)
      {
         if (index._pageIndex !=
             _pageIndex) 
         {
            // Page fault
            readPage(index._pageIndex);
            
         }
         
         Branch& branch =
            _page
            ._branchPage
            ._branches[
               index._branchIndex
            ];
            
         return branch;
      }
   
      Index& getNextIndex()
      {

         Index& next =
            ++(_header._next);
            
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
   
   private:
   
      void readPage(File::Size pageIndex)
      {
         if ( pageIndex ==
              _pageIndex )
            return;
         
         if (_isDirty) 
         {
            savePage();
         }
      
         _pageIndex = pageIndex;
      
         Size offset =
            PAGE_SIZE * (_pageIndex + 1);
         
         seek(offset);
      
         read(
            &_page,
            1,
            PAGE_SIZE
         );
      
      }
   
      void savePage()
      {
         Size offset =
            PAGE_SIZE * (_pageIndex + 1);
         
         seek(offset);
      
         write(
            &_page,
            1,
            PAGE_SIZE
         );
         
         _isDirty = false;

      }
   
      struct Header {
         // Page aligned header
         union {
            char buffer[PAGE_SIZE];
            struct {
               char   _version[256];
               unsigned short   _pageSize;
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
            _size / PAGE_SIZE;
         
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
            newValue % PAGE_SIZE;
   
         if (mod != 0)
            newValue += (
               PAGE_SIZE - mod
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
             << "Page Size: "
             << db._header._pageSize
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

}

#endif