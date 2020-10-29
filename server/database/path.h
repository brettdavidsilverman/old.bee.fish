#ifndef BEE_FISH_DATABASE__PATH_H
#define BEE_FISH_DATABASE__PATH_H

#include <optional>
#include <iostream>
#include "../power-encoding/power-encoding.h"
#include "file.h"
#include "database.h"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

   class Path :
      public PowerEncoding
   {
   protected:
      Index _index;
      Index _lockedIndex;
      Database _database;
   public:
   
      Path( Database& database,
               Index index = IndexRoot ) :
         PowerEncoding(),
         _index(index),
         _lockedIndex(0),
         _database(database)
      {
      }
   
      Path(const Path& source) :
         PowerEncoding(),
         _index(source._index),
         _lockedIndex(0),
         _database(source._database)
      {
         
      }
   
      virtual ~Path()
      {
         unlock();
      }
      
      virtual void writeBit(bool bit)
      {
      
#ifdef DEBUG
  //       cerr << (bit ? '1' : '0');
#endif

         Branch branch;
         getBranch(
            _index,
            branch
         );
      
         bool isDirty = false;
         
         Index parent = _index;
         
         if (bit)
         {
            if (!branch._right)
            {
               lock();
               branch._right = 
                  _database.getNextIndex();
               
               isDirty = true;
#ifdef DEBUG
    //           cerr << '+';
#endif
            }
#ifdef DEBUG
      //      else
     //          cerr << '=';
#endif
               
            _index = branch._right;
         }
         else
         {
            if (!branch._left)
            {
               lock();
               branch._left = 
                  _database.getNextIndex();
               
               isDirty = true;
#ifdef DEBUG
      //         cerr << '+';
#endif
            }
#ifdef DEBUG
     //       else
     //          cerr << '=';
#endif
            _index = branch._left;
         }
         
         
         if (isDirty)
         {
            _database.writeBranch(parent, branch);
            
            Branch child;
            child._left   = 0;
            child._right  = 0;
            child._parent = parent;
            
            _database.writeBranch(_index, child);
         }
         
      }
     
      virtual void lock()
      {
         if (!_lockedIndex)
         {
            _lockedIndex = _index;
            _database.lockBranch(
               _lockedIndex
            );
         }

      }
      
      virtual void unlock()
      {
         if (_lockedIndex)
         {
            _database.unlockBranch(_lockedIndex);
            _lockedIndex = 0;
         }
      }
      
      virtual bool readBit()
      {
         Branch branch;
         getBranch(_index, branch);
            
         if (branch._left)
         {
            _index = branch._left;
            return false;
         }
         else if (branch._right)
         {
            _index = branch._right;
            return true;
         }
         
         throw runtime_error("Past end of file");
         
      }
      
      virtual bool peekBit()
      {
         Branch branch;
         getBranch(_index, branch);
               
         if (branch._left)
            return false;
         else if (branch._right)
            return true;
               
         throw runtime_error("Past end of file");
      }
      
      virtual Index& operator*() {
         return _index;
      }

      Path& operator=(const Index& index)
      {
         _index = index;
         return *this;
      }
   
      Path& operator=(const Path& rhs)
      { 
         _index = rhs._index;
         return *this;
      }
   
      bool operator == (const Index& rhs)
      {
         return (_index == rhs);
      }
   
      
      template<class T>
      Path& operator <<
      (const T& value)
      {
         PowerEncoding::operator << (value);
         
         unlock();
         
         return *this;
      }

      const Index& index() const
      {
         return _index;
      }
      
      bool isDeadEnd()
      {
         Branch branch;
         getBranch(_index, branch);
         return branch.isDeadEnd();
      }
      
      template<typename Key>
      Path operator [] (const Key& key) const
      {
         Path start(*this);
         start << key;
         return start;
      }
      /*
      Path& remove()
      {
         throw runtime_error("Not implemented");
         Branch& branch = getBranch();
         
         return *this;
      }
      */
      
      virtual void traverse(ostream& out)
      {
         out << '1';
         traverse(out, _index);
    
      }
   
      friend ostream& operator <<
      (ostream& out, Path& path)
      {
         path.traverse(out);
      
         return out;
      }
   
   
   protected:

      void traverse(
         ostream& out,
         Index index
      )
      {
      
         /*
         Branch branch =
            getBranch(index);
         
         bool lastWasLeft = false;
         
         while (1)
         {
            // go least (bottom left)
            while(branch._left || branch._right)
            {
               if (branch._left)
               {
                  out << '1';
                  branch =
                     getBranch(branch._left);
                  lastWasLeft = true;
               }
               else
               {
                  out << '0';

                  branch =
                     getBranch(branch._right);
                  lastWasLeft = false;
               }
               
            }
            
            // go next least (up and right)
            while(branch._parent)
            {
               if (branch._right &&
                   lastWasLeft)
               {
                  out << '1';
                  
                  // go right
                  branch =
                     getBranch(branch._right);
                  
                  lastWasLeft = false;
                  break;
               }
               
               out << '0';
               
               // go up
               branch = 
                  getBranch(branch._parent);
            }
            
            if (!branch._parent)
               break;
         }
         
         return;
         */
        
         Branch branch;
         getBranch(index, branch);
            
         if (branch._left)
         {
            out << '1';
            traverse(
               out,
               branch._left
            );
         }
         else
            out << '0';
      
         if (branch._right)
         {
            out << '1';
            traverse(
               out, 
               branch._right
            );
         }
         else
            out << '0';
        

      }

      void first(ostream& out)
      {
         Branch branch;
         getBranch(_index, branch);
            
         while (!branch.isDeadEnd())
         {
            if (branch._left)
            {
               out << '0';
               _index = branch._left;
            }
            else
            {
               out << '1';
               _index = branch._right;
            }
            
            getBranch(_index, branch);
         }
         
      }
      
      void getBranch(const Index& index, Branch& branch)
      {
         
         _database.readBranch(index, branch);
      }
      
      
   };

   class ReadOnlyPath :
      public Path
   {
   
   public:

      ReadOnlyPath (Database& database) :
         Path(database)
      {
         Branch branch;
         getBranch(_index, branch);
         
         _eof = branch.isDeadEnd();
      }
      
      ReadOnlyPath( Path& path ) :
        Path(path)
      {
 
         Branch branch;
         getBranch(_index, branch);
         
         _eof = branch.isDeadEnd();
      }
   
      virtual void writeBit(bool bit)
      {
         Branch branch;
         getBranch(_index, branch);
         
         Index& index =
            bit ?
               branch._right :
               branch._left;
         
         if (!index)
         {
            _eof = true;
            throw runtime_error("End of file");
         }
         else
            _index = index;
         
      
      }
   
      ReadOnlyPath& operator=
      (const Index& index)
      {
         _index = index;
         return *this;
      }
   
      virtual bool eof()
      {
         return _eof;
      }
   protected:

      bool _eof;
   };

}

#endif