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
#include <map>

#include "file.h"
#include "config.h"
#include "version.h"
#include "index.h"
#include "page.h"
#include "branch.h"

using namespace std;

namespace bee::fish::database {
   
   // Store [left, right] branch elements.
   // A zero is stored if the branch
   // hasnt been visited yet.
   // The _next points to the furthest element.
   class Database :
      public File
   {
   private:
      
      struct Header
      {
         char   _version[256];
         Index  _nextIndex;
         Branch _tree[];
      };
   
      struct Shared
      {
         Size  _count = 0;
         mutex _nextIndexMutex;
         mutex _mutex;
         map<Index, mutex> _branchLocks;
      };
      
      Size  _incrementSize;
      Size  _size;
      Header* _header;
      inline static map<string, Shared> _sharedMap;
      Shared& _shared;
      friend class Path;

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
         _shared(_sharedMap[_fullPath])
      {
      
         const std::lock_guard<std::mutex>
            lock(_shared._mutex);
            
         ++_shared._count;
         _incrementSize = incrementSize;
         
         mapFile();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
         
      }
      
      Database(const Database& source) :
         File(source),
         _shared(source._shared),
         _incrementSize(source._incrementSize),
         _size(source._size)
      {
         ++_shared._count;
         mapFile();
      }
      
      ~Database()
      {
         const std::lock_guard<std::mutex>
            lock(_shared._mutex);
            
         if (--_shared._count == 0)
         {
            for ( auto
                  it  = _shared._branchLocks.begin();
                  it != _shared._branchLocks.end();
                ++it )
            {
               it->second.unlock();
            }
         
            _shared._branchLocks.clear();
         }
         
         munmap(_header, _size);
      }
      
   private:

      virtual void initializeHeader()
      {
         memset(_header, '\0', sizeof(Header));
         strcpy(_header->_version, BEE_FISH_DATABASE_VERSION);
         _header->_nextIndex = Branch::Root;
      }
      
      virtual void checkHeader()
      {
         if (strcmp(_header->_version, BEE_FISH_DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Program version ";
            error += BEE_FISH_DATABASE_VERSION;
            error += ". File version ";
            error += _header->_version;
            throw runtime_error(error);
         }

      }
      
      virtual void mapFile()
      {
         _size = fileSize();
         
         _header = (Header*)
            mmap(
               NULL,
               _size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fileNumber,
               0
            );
      }
         
      
   public:
      
   
      Index getNextIndex()
      {
      
         const std::lock_guard<std::mutex>
            lock(_shared._nextIndexMutex);

         Index next =
            ++_header->_nextIndex;
            
         return next;
      }
  
   
   public:
   
      inline Branch& getBranch(
         const Index& index
      )
      {
         return _header->_tree[index];
      }
      
      void lockBranch(Index index)
      {
         
         _shared._branchLocks[index].lock();
         
      }
      
      void unlockBranch(Index index)
      {
          const std::lock_guard<std::mutex>
            lock(_shared._mutex);

         _shared._branchLocks[index].unlock();
         _shared._branchLocks.erase(index);
      }

      virtual Size resize(
         Size size = 0
      )
      {
      
         const std::lock_guard<std::mutex>
            lock(_shared._mutex);
            
         if (size == 0)
            size = _size +
                   _incrementSize;
         
 
         size = getPageAlignedSize(size);
        
         if (size <= _size)
            return _size;
         
         cerr << "Resizing " << size;
         
         size = File::resize(size);
         
         _header = (Header*)
            mremap(
               _header,
               _size,
               size,
               MREMAP_MAYMOVE
            );
            
            
         _size = size;
         
         cerr << " ok " << endl;
         
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
             << BEE_FISH_DATABASE_VERSION
             << endl
             << "Filename: "
             << db._filePath
             << endl
             << "Next: "
             << db._header->_nextIndex
             << endl
             << "Branch size: "
             << sizeof(Branch)
             << endl
             << "Size: "
             << db._size
             << endl;
          
         return out;
      }

   };

}


#endif