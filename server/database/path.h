#ifndef BEE_FISH_DATABASE__PATH_H
#define BEE_FISH_DATABASE__PATH_H

#include <optional>
#include <iostream>
#include <atomic>
#include "../power-encoding/power-encoding.h"
#include "file.h"
#include "branch.h"
#include "database.h"

//#undef DEBUG

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

   class Path :
      public PowerEncoding
   {
   protected:
      Database& _database;
      Index     _index;
   public:
   
      Path( Database& database,
            Index index = Branch::Root ) :
         PowerEncoding(),
         _database(database),
         _index(index)
      {
      }
   
      Path(const Path& source) :
         PowerEncoding(),
         _index(source._index),
         _database(source._database)
      {
         
      }
   
      
      template <class T>
      Path& operator <<
      (const T& object)
      {
         PowerEncoding::operator << (object);
         
#ifdef DEBUG
         cerr << endl;
#endif
         return *this;
      }
      
      virtual void writeBit(bool bit)
      {
      
#ifdef DEBUG
         cerr << (bit ? '1' : '0');
#endif

         Branch& branch =
            _database.getBranch(_index);
            
         if (bit)
         {
            if (!branch._right)
            {
               branch._right = 
                  _database.getNextIndex();
               
#ifdef DEBUG
               cerr << '+';
#endif
            }
#ifdef DEBUG
            else
               cerr << '=';
#endif
               
            _index = branch._right;
         }
         else
         {
            if (!branch._left)
            {
               branch._left = 
                  _database.getNextIndex();
#ifdef DEBUG
               cerr << '+';
#endif
            }
#ifdef DEBUG
            else 
               cerr << '=';
#endif
            _index = branch._left;
         }
         
      }
      
      virtual bool readBit()
      {
         Branch& branch =
            _database.getBranch(_index);
            
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
         Branch& branch =
            _database.getBranch(_index);
               
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
   

      template<typename T>
      Path operator [] (const T& key)
      {
         Path path(*this);
         
         path << key;
         
         return path;
      }
      
      const Index& index() const
      {
         return _index;
      }
      
      bool isDeadEnd()
      {
         Branch& branch =
            _database.getBranch(_index);
            
         return branch.isDeadEnd();
      }
      
      
      Database& database()
      {
         return _database;
      }
      
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
        
         Branch branch =
            _database.getBranch(index);
            
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
         Branch branch =
            _database.getBranch(_index);
            
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
            
            branch = 
               _database.getBranch(_index);
         }
         
      }
      
      
      
      
      
   };

}

#endif