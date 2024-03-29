#ifndef BEE_FISH_DATABASE__DATABASE_H
#define BEE_FISH_DATABASE__DATABASE_H

#include "config.h"

#include <sys/mman.h>

#include <string.h>
#include <map>
#include <mutex>

#include "file.h"
#include "version.h"
#include "index.h"
#include "branch.h"

using namespace std;

namespace BeeFishDatabase {
   
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
    
      struct Tree
      {
         Header _header;
         Branch _root[];
      };
      
      Size  _incrementSize;
      Size  _size;
      Tree* _tree;
      Branch* _root;
      Index* _nextIndex;
      Size    _branchCount;

   public:
      std::mutex _lock;
   
      Database(
         BString filePath,
         const Size initialSize = 1000 * 1000,
         const Size incrementSize = 1000 * 1000
      ) :
         File(
            filePath,
            getPageAlignedSize(initialSize)
         ),
         _incrementSize(incrementSize)
      {
         
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
         _size(source._size)
      {
         mapFile();
      }
      
      ~Database()
      {
         munmap(_tree, _size);
      }
      
      
   private:

      virtual void initializeHeader()
      {
         Header& header = _tree->_header;
         //memset(&header, '\0', sizeof(Header));
         strcpy(header._version, BEE_FISH_DATABASE_VERSION);
         header._nextIndex = Branch::Root;
      }
      
      virtual void checkHeader()
      {
         if (strcmp(_tree->_header._version, BEE_FISH_DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Program version ";
            error += BEE_FISH_DATABASE_VERSION;
            error += ". File version ";
            error += _tree->_header._version;
            throw runtime_error(error);
         }

      }
      
      virtual void mapFile()
      {
         _size = fileSize();
         
         _tree = (Tree*)
            mmap(
               NULL,
               _size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fileNumber,
               0
            );
            
         _root = _tree->_root;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
            
         _nextIndex = 
           &(_tree->_header._nextIndex);
      }
         
      
   public:
      
   
      struct Data
      {
         Size  _size;
         char _bytes[];
         void* getData()
         {
            return &(_bytes[0]);
         }
         
         Size getSize()
         {
            return _size;
         }
         
      };
      
      inline Index getNextIndex(const Index& parent = 0)
      {
         Index nextIndex =
            ++(*_nextIndex);
         
         if (parent > 0) {

            Branch& nextBranch =
               getBranch(nextIndex);

            nextBranch._parent = parent;
         }

         return nextIndex;
      }
  
      inline Index allocate(Size byteSize)
      {
         Size size = sizeof(Size) + byteSize;
  
         Index branchCount =
            ceil(size / sizeof(Branch));
            
         Index dataIndex = getNextIndex();
         
         (*_nextIndex) += (branchCount);
         
         // Check for resize
         if ( *_nextIndex >= _branchCount )
         {
            resize(_size + byteSize);
         }
        
         Database::Data* data = getData(dataIndex);
         
         data->_size = byteSize;
         
         return dataIndex;
            
      }
      
      inline Branch& getBranch(const Index& index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
         
         return _root[index];
      }
      
      inline Database::Data* getData(const Index& dataIndex)
      {
         if (dataIndex == 0)
            return nullptr;
         
         Database::Data* data =
            (Database::Data*)
               (&(_root[dataIndex]));
            
         return data;
      }
 
      virtual Size resize(
         Size size = 0
      )
      {
            
         
         if (size == 0)
            size = File::size() +
                   _incrementSize;
         
 
         size = getPageAlignedSize(size);
            
         if (size <= _size)
            size = _size;
         else
            size = File::resize(size);
         
         _tree = (Tree*)
            mremap(
               _tree,
               _size,
               size,
               MREMAP_MAYMOVE
            );
            
         _root = _tree->_root;
         
         _size = size;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
            
         _nextIndex = 
           &(_tree->_header._nextIndex);
           
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
             << db._tree->_header._version
             << endl
             << "Filename: "
             << db._filename
             << endl
             << "Next: "
             << (unsigned long long)(db._tree->_header._nextIndex)
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


