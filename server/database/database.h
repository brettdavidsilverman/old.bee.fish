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
    
         checkHeader();
      
         _branchCount =
            floor(_size / sizeof(Branch));
            
         _isDirty = false;
      }
     
      ~Database()
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
      
   public:
      
   
      Index getNextIndex()
      {
      
         _mutex.lock();

         Index next =
            ++_nextIndex;
            
         _mutex.unlock();
         
         _isDirty = true;
         
         if ( next >=
            _branchCount )
            resize();
         
         return next;
      }
  
   
   private:
   
      friend class Path;
      
      void readBranch(
         const Index& index,
         Branch* branch
      )
      {

         _readWriteMutex.lock();
         
         Size offset =
            sizeof(Branch) * index;
         
         seek(offset);
      
         read(
            branch,
            1,
            sizeof(Branch)
         );
         
         _readWriteMutex.unlock();
      }

      void writeBranch(
         const Index& index,
         const Branch* branch
      )
      {
      
         _readWriteMutex.lock();
         
         Size offset =
            sizeof(Branch) * index;
         
         seek(offset);
      
         write(
            branch,
            1,
            sizeof(Branch)
         );
         
         _readWriteMutex.unlock();
         
      }
      
      void writeHeader(HeaderPage& header) 
      {
         seek(0);
         write(&header, 1, sizeof(HeaderPage));
         _isDirty = false;
      }
      
      
     
      Index _nextIndex;
      bool _isDirty = false; 
      File::Size _branchCount = 0;
      const File::Size _incrementSize;

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

         _branchCount =
            floor((_size - PAGE_SIZE) /
                  sizeof(Branch));
         
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
             << "Branch count: "
             << db._branchCount
             << endl
             << "Size: "
             << db._size
             << endl;
          
         return out;
      }

   };

}


#endif