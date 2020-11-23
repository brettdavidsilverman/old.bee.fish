#ifndef BEE_FISH_DATABASE__DATABASE_H
#define BEE_FISH_DATABASE__DATABASE_H

#include <sys/mman.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <atomic>
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
#include "quick-lock.h"

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
         atomic<Index>  _nextIndex;
      };
    
      struct Data
      {
         Header _header;
         Branch _tree[];
      };
      
      typedef map<Index, QuickLock>
         BranchLocks;
      
      struct Shared
      {
         mutex   _mutex;
         atomic<unsigned long> _count = 0;
         BranchLocks _branchLocks;
      };
      
      Size  _incrementSize;
      Size  _size;
      Data* _data;
      Branch* _tree;
      Size    _branchCount;
      
      inline static
      map<string, Shared> _sharedPerFile;
      Shared&             _shared;
      BranchLocks&        _branchLocks;
      
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
         _shared(
            _sharedPerFile[_fullPath]
         ),
         _branchLocks(
            _shared._branchLocks
         )
      {
      
         _shared._mutex.lock();
            
         ++_shared._count;
         
         mapFile();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
         _shared._mutex.unlock();
      }
      
      Database(const Database& source) :
         File(source),
         _incrementSize(source._incrementSize),
         _size(source._size),
         _shared(source._shared),
         _branchLocks(source._branchLocks)
      {
         _shared._mutex.lock();
         ++_shared._count;
         mapFile();
         _shared._mutex.unlock();
      }
      
      ~Database()
      {
         cerr << "lock...";
         
         _shared._mutex.lock();
            
         if (--_shared._count == 0)
         {
            cerr << "clearing branch locks...";
            _branchLocks.clear();
            _sharedPerFile.erase(_fullPath);
         }
         
         cerr << "mumps...";
         
         munmap(_data, _size);
         
         _shared._mutex.unlock();
         
         cerr << "...unlock ok" << endl;
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
      
         Index next =
            ++(_data->_header._nextIndex);
         
         return next;
      }
  
      inline Branch getBranch(const Index& index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
        // cerr << "r(" << index << ')';
            
         return _tree[index];
      }
      
      inline void setBranch(const Index& index, const Branch& branch)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
         
        // cerr << "w(" << index << ')';
         
         _tree[index] |= branch;
      }
      
      inline void lockBranch(Index index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
         
         _branchLocks[index].lock();
         /*
         _shared._lock.lock();
         
         QuickLock& branchLock =
            _branchLocks[index];
            
         _shared._lock.unlock();
         
         branchLock.lock();
         */
      }
      
      inline void unlockBranch(Index index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
        
         _branchLocks.erase(index);
         /*
         
         _shared._lock.lock();
        // _branchLocks[index].wait();
         _branchLocks[index].unlock();
         _branchLocks.erase(index);
         
         _shared._lock.unlock();
         */
      }

      inline void waitBranch(Index index)
      {
         _branchLocks[index].wait();
      }
      
      virtual Size resize(
         Size size = 0
      )
      {
      
        // _shared._lock.lock();
            
        // cerr << "Resize ";
         
         if (size == 0)
            size = File::size() +
                   _incrementSize;
         
 
         size = getPageAlignedSize(size);
            
         if (size <= _size)
            size = _size;
         else
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
            
        // _shared._lock.unlock();
         
       //  cerr << " ok " << _data << endl;
         
         
         
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
             << db._fullPath
             << endl
             << "Next: "
             << (unsigned long long)(db._data->_header._nextIndex)
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