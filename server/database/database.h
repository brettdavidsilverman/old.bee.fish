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
      };
    
      struct Data
      {
         Header _header;
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
      Data* _data;
      Branch* _tree;
      Size    _branchCount;
      Shared& _shared;
      inline static mutex _mutex;
      inline static map<string, Shared>
         _sharedMap;

   public:
   
      Database(
         const string& filePath,
         const Size initialSize = 1000 * 1000,
         const Size incrementSize = 1000 * 1000
      ) :
         File(
            filePath,
            getPageAlignedSize(initialSize)
         ),
         _incrementSize(incrementSize),
         _shared(_sharedMap[_fullPath])
      {
      
         const std::lock_guard<std::mutex>
            lock(_mutex);
            
         ++_shared._count;
         
         mapFile();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
         
      }
      
      Database(const Database& source) :
         File(source),
         _incrementSize(source._incrementSize),
         _size(source._size),
         _shared(source._shared)
      {
         ++_shared._count;
         mapFile();
      }
      
      ~Database()
      {
         const std::lock_guard<std::mutex>
            lock(_mutex);
            
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
            _sharedMap.erase(_fullPath);
         }
         
         munmap(_data, _size);
      }
      
      
   private:

      virtual void initializeHeader()
      {
         Header& header = _data->_header;
         memset(&header, '\0', sizeof(Header));
         strcpy(header._version, BEE_FISH_DATABASE_VERSION);
         header._nextIndex = Branch::Root;
      }
      
      virtual void checkHeader()
      {
         if (strcmp(_data->_header._version, BEE_FISH_DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Program version ";
            error += BEE_FISH_DATABASE_VERSION;
            error += ". File version ";
            error += _data->_header._version;
            throw runtime_error(error);
         }

      }
      
      virtual void mapFile()
      {
         _size = fileSize();
         
         _data = (Data*)
            mmap(
               NULL,
               _size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fileNumber,
               0
            );
            
         _tree = _data->_tree;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
      }
         
      
   public:
      
   
      inline Index getNextIndex()
      {
      
         const std::lock_guard<std::mutex>
            lock(_shared._nextIndexMutex);

         Index next =
            ++(_data->_header._nextIndex);
         
         return next;
      }
  
   
      inline  Branch& getBranch(const Size& index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
         //cerr << index << endl;
         return _tree[index];
      }
      
      inline void lockBranch(Index index)
      {
         const std::lock_guard<std::mutex>
            lock(_shared._mutex);
         _shared._branchLocks[index].lock();
         
      }
      
      inline void unlockBranch(Index index)
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
         
        // cerr << "Resizing " << size;
         
         size = File::resize(size);
         
         _data = (Data*)
            mremap(
               _data,
               _size,
               size,
               MREMAP_MAYMOVE
            );
            
         _tree = _data->_tree;
         
         _size = size;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
            
      //   cerr << " ok " << _data << endl;
         
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
             << db._data->_header._nextIndex
             << endl
             << "Branch size: "
             << sizeof(Branch)
             << endl
             << "Size: "
             << db.size()
             << endl;
          
         return out;
      }

   };

}


#endif