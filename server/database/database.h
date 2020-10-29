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
      struct Data
      {
         Index _nextIndex;
         bool  _isDirty;
         Size  _incrementSize;
         Size  _size;
         mutex _mutex;
         map<Index, mutex> _branchLocks;
      };
      
      inline static map<string, Data> _databases;
      Data& _data;
      

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
         _data(_databases[_fullPath])
      {
      
         _data._incrementSize = incrementSize;
         _data._size = fileSize();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
      
         _data._isDirty = false;
      }
      
     
      ~Database()
      {
         _data._mutex.lock();
         
         save();
                  
         _data._branchLocks.clear();
         
         _data._mutex.unlock();
         
      }
      
      void save()
      {
         if (_data._isDirty)
         {
            HeaderPage header;
            readHeader(header);
            header._nextIndex =
               _data._nextIndex;
            writeHeader(header);
            _data._isDirty = false;
         }
      }
   
      Database(const Database& source) :
         File(source),
         _data(source._data)
      {
      }
      
   private:

      virtual void initializeHeader()
      {
         HeaderPage header;
         memset(&header, '\0', sizeof(HeaderPage));
         header._pageSize = PAGE_SIZE;
         strcpy(header._version, BEE_FISH_DATABASE_VERSION);
         header._nextIndex = IndexRoot;
         seek(0);
         write(&header, 1, sizeof(HeaderPage));
      }
      
      virtual void checkHeader()
      {
         HeaderPage header;
         readHeader(header);
         _data._nextIndex = header._nextIndex;
         checkHeaderVersion(header);
         checkHeaderPageSize(header);
      }
      
      virtual void readHeader(HeaderPage& header) const
      {
         File::seek(0);
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
      
   public:
      
   
      Index getNextIndex()
      {
      
         _data._mutex.lock();

         Index next =
            ++_data._nextIndex;
         
         _data._isDirty = true;
         
         if ( next * sizeof(Branch) > _data._size )
            resize();
            
         _data._mutex.unlock();
         
         return next;
      }
  
   
   public:
   
      
      void readBranch(
         const Index& index,
         Branch& branch
      )
      {
         
         Size offset =
            sizeof(Branch) * index;
            
         seek(offset);
      
         read(
            &branch,
            1,
            sizeof(Branch)
         );
        
      }
 
      void writeBranch(
         const Index& index,
         Branch& branch
      )
      {
      
         Size offset =
            sizeof(Branch) * index;
         
         seek(offset);
      
         Branch old;
         readBranch(index, old);
         
         branch |= old;
         
         seek(offset);
         
         write(
            &branch,
            1,
            sizeof(Branch)
         );
         
      }
            
      void writeHeader(HeaderPage& header) 
      {
         seek(0);
         write(&header, 1, sizeof(HeaderPage));
         _data._isDirty = false;
      }

      void lockBranch(Index index)
      {
         _data._mutex.lock();
         _data._branchLocks[index].lock();
         _data._mutex.unlock();
      }
      
      void unlockBranch(Index index)
      {
         _data._mutex.lock();
         _data._branchLocks[index].unlock();
         _data._branchLocks.erase(index);
         _data._mutex.unlock();
      }

      virtual Size resize(
         Size size = 0
      )
      {
 
         if (size == 0)
            size =
               File::size() +
               _data._incrementSize;
     
         /*
         Size newSize =
            getPageAlignedSize(size);
         */
         
         if (size <= _data._size)
         {
            return _data._size;
         }
         
         _data._size = File::resize(size);
         
         return _data._size;

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
             << db._fullPath
             << endl
             << "Next: "
             << db._data._nextIndex
             << endl
             << "Page Size: "
             << header._pageSize
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