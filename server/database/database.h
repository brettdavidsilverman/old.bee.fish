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
#include <list>

#include "file.h"
#include "config.h"
#include "version.h"
#include "index.h"
#include "page.h"
#include "branch.h"

using namespace std;

namespace bee::fish::database {

   class Pointer;
   class CachedPage;
   class PageCache;
   
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
         const Size incrementSize = 1000 * 1000
      ) :
         File(
            filePath,
            initialSize
         ),
         _incrementSize(incrementSize)
      {
      
         if (isNew())
         {
            initializeHeader();
         }
    
         initializePageCache();
         
         checkHeader();
      
         _pageCount =
            floor(_size / PAGE_SIZE);
            
         _isDirty = false;
      }
      
   private:

      virtual void initializeHeader()
      {
         HeaderPage header;
         memset(&header, '\0', sizeof(HeaderPage));
         header._pageSize = PAGE_SIZE;
         strcpy(header._version, BEE_FISH_DATABASE_VERSION);
         header._nextIndex = Index::root;
         seek(0);
         write(&header, 1, sizeof(HeaderPage));
      }
      
      virtual void checkHeader()
      {
         HeaderPage header;
         readHeader(header);
         _nextIndex = header._nextIndex;
         checkHeaderVersion(header);
         checkHeaderPageSize(header);
      }
      
      virtual void readHeader(HeaderPage& header) const
      {
         seek(0);
         read(&header, 1, sizeof(HeaderPage));
      }
      
      virtual void checkHeaderVersion(HeaderPage& header)
      {
         if (strcmp(header._version, BEE_FISH_DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Expecting ";
            error += BEE_FISH_DATABASE_VERSION;
            error += ". Got ";
            error += header._version;
            throw runtime_error(error);
         }

      }
      
      virtual void checkHeaderPageSize(HeaderPage& header)
      {
               
         if (header._pageSize != PAGE_SIZE)
         {
            std::string error = "Invalid file page size.";
            error += " Expecting ";
            error += to_string(PAGE_SIZE);
            error += ". Got ";
            error += to_string(header._pageSize);
            throw runtime_error(error);
         }

      }
      
      virtual void initializePageCache();
      
   public:

      ~Database();
   
      
   
      Index getNextIndex()
      {
      
         _mutex.lock();

         Index next =
            ++_nextIndex;
            
         _mutex.unlock();
         
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
   
      friend class PageCache;
      friend class Path;
      
      CachedPage* operator [] (File::Size pageIndex);
      
      void readPage(
         File::Size pageIndex,
         Page* page
      )
      {

         _readWriteMutex.lock();
         
         Size offset =
            PAGE_SIZE * pageIndex;
         
         seek(offset);
      
         read(
            page,
            1,
            PAGE_SIZE
         );
         
         _readWriteMutex.unlock();
      }

      void writePage(
         File::Size pageIndex,
         const Page* page
      )
      {
      
         _readWriteMutex.lock();
         
         Size offset =
            PAGE_SIZE * pageIndex;
         
         seek(offset);
      
         write(
            page,
            1,
            PAGE_SIZE
         );
         
         _readWriteMutex.unlock();
         
      }
      
      void writeHeader(HeaderPage& header) 
      {
         seek(0);
         write(&header, 1, sizeof(HeaderPage));
         _isDirty = false;
      }
      
      
     
      PageCache* _pageCache;
      
      list<File::Size> _pageList;
      Index _nextIndex;
      bool _isDirty = false; 
      File::Size _pageCount = 0;
      const File::Size _incrementSize;
      const File::Size _maxPageCache = 250000;
      boost::mutex _mutex;
      boost::mutex _readWriteMutex;
   public:
      
      virtual Size resize(
         Size size = 0
      )
      {
         if (size == 0)
            size = _size + _incrementSize;
     
         /*
         Size newSize =
            getPageAlignedSize(size);
         */
         
         File::resize(size);

         _pageCount =
            floor(_size / PAGE_SIZE);
         
         return _size;

      }
   
   protected:
   /*
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
   */
      friend ostream& operator <<
      (ostream& out, const Database& db)
      {
         HeaderPage header;
         db.readHeader(header);
         
         out << "Database: " 
             << header._version
             << endl
             << "Filename: "
             << db.filePath
             << endl
             << "Next: "
             << header._nextIndex
             << endl
             << "Page Size: "
             << header._pageSize
             << endl
             << "Page count: "
             << db._pageCount
             << endl
             << "Size: "
             << db._size
             << endl;
          
         return out;
      }

   };

}

#include "page-cache.h"

namespace bee::fish::database
{
   inline void Database::initializePageCache()
   {
      _pageCache = new PageCache(this);
   }
   
   inline Database::~Database()
   {

      
      if (_isDirty)
      {
         HeaderPage header;
         readHeader(header);
         header._nextIndex = _nextIndex;
         writeHeader(header);
         _isDirty = false;
      }
#ifdef DEBUG
      cerr << *this;
#endif
      delete _pageCache;
   }
   
   inline CachedPage* Database::operator [] (File::Size pageIndex)
   {
      return (*_pageCache)[pageIndex];
   }
}

#endif