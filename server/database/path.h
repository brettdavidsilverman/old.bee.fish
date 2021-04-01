#ifndef BEE_FISH_DATABASE__PATH_H
#define BEE_FISH_DATABASE__PATH_H

#include <optional>
#include <iostream>
#include <atomic>
#include "../power-encoding/power-encoding.h"
#include "../b-string/string.h"
#include "file.h"
#include "branch.h"
#include "database.h"

using namespace std;
using namespace bee::fish::power_encoding;
using namespace bee::fish::b_string;

namespace bee::fish::database {


   template<class Encoding = PowerEncoding>
   class Path :
      public Encoding
   {
   public:
      typedef Encoding _Encoding;
   public:
      Database& _database;
      Index     _index;
      BString    _trail;
      
   public:
   
      Path( Database& database,
            Index index = Branch::Root ) :
         Encoding(),
         _database(database),
         _index(index)
      {
      }
   
      Path(const Path& source) :
         Encoding(),
         _database(source._database),
         _index(source._index),
         _trail(source._trail)
         
      {
         
      }
  /*
      template <class T>
      Path& operator <<
      (const T& object)
      {

         Encoding::operator << (object);
         
#ifdef DEBUG
         cerr << endl;
#endif
         return *this;
      }
     */
      Path& operator <<
      (const BString& str)
      {
         writeBit(true);
         
         for (const Char& character : str)
         {
            writeBit(true);
            *this << character;
         }
         
         writeBit(false);
         
         return *this;
      }
      
      Path& operator >>
      (BString& bstring)
      {
         bool bit = readBit();
         if (!bit)
            throw runtime_error("Expected 'true' bit");
         
         bstring.clear();
      
         Char character;
         
         while (readBit())
         {
            *this >> character;
            bstring.push_back(character);
         }
         
         return *this;
         
      }
   
      Path& operator <<
      (const char* object)
      {
      
         return Path::operator <<
            (BString(object));
         
      }

      Size getDataSize()
      {
       
         Database::Data* data =
            getData();
        
         if (data != NULL)
            return data->_size;
         
         return 0;
      }
      
      Database::Data* getData()
      {
         Branch& branch =
            _database.getBranch(_index);
            
         if (branch._dataIndex)
         {
            Database::Data* data =
               _database.getData(
                  branch._dataIndex
               );
            
            return data;
         }
         
         return NULL;
      }
      
      Database::Data* setData(const void* source, Size size)
      {

         Index dataIndex = 
            _database.allocate(size);
               
         
         Branch& branch =
            _database.getBranch(_index);
         
         branch._dataIndex = dataIndex;
         
         Database::Data* data =
            _database.getData(
               branch._dataIndex
            );
            
         memcpy(data->data(), source, size);
         
         return data;
      }
      
      Branch& getBranch()
      {
         return
            _database.getBranch(_index);
      }
      
      void deleteData()
      {
         Branch& branch =
            _database.getBranch(_index);
         branch._dataIndex = 0;
      }
      
      void clear()
      {
         Branch& branch =
            _database.getBranch(_index);
         branch._left = 0;
         branch._right = 0;
      }
      
      const BString& trail()
      {
         return _trail;
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
            
            _trail.push_back('1');
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
            
            _trail.push_back('0');
         }
         
      
      }
      
      virtual bool readBit()
      {
         Branch& branch =
            _database.getBranch(_index);
            
         if (branch._left)
         {
            _trail.push_back('0');
            _index = branch._left;
            return false;
         }
         else if (branch._right)
         {
            _trail.push_back('1');
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
      
      Path& operator=(const Path& rhs)
      { 
         _index = rhs._index;
         _trail = rhs._trail;
         return *this;
      }
   
      bool operator == (const Path& rhs)
      {
         return (_index == rhs._index);
      }
   
      operator bool ()
      {
         return (_index > Branch::Root);
      }

      template<typename T>
      Path operator [] (const T& key)
      {
         Path path(*this);
         
         path << key;
         
         return path;
      }
      
      Path operator [] (const char* key)
      {
         return Path::operator[] 
            (BString(key));
      }
      
      const Index& index() const
      {
         return _index;
      }
      
      virtual bool isDeadEnd()
      {
         Branch& branch =
            _database.getBranch(_index);
            
         return branch.isDeadEnd();
      }
      
      template<typename T>
      Path contains(const T& object)
      {
         Contains check(_database, _index);
         return check.contains(object);
      }
      
      Database& database()
      {
         return _database;
      }
      
      
   
   
   protected:
      
      static bool readBit(istream& in)
      {
         char c;
         in >> c;
         bool bit = (c != '0');
         return bit;
      }
      
      friend istream& operator >>
      (istream& in, Path& path)
      {
         if (Path::readBit(in))
            path.input(in);
         return in;
      }
      
      void input(istream& in)
      {
         Index bookmark = _index;
         
         // Write the left branch
         if (Path::readBit(in))
         {
            writeBit(false);
            input(in);
            _index = bookmark;
         }
            
         // Write the right branch
         if (Path::readBit(in))
         {
            writeBit(true);
            input(in);
            _index = bookmark;
         }
      }
      
      friend ostream& operator <<
      (ostream& out, Path& path)
      {
         Path::writeBit(out, true);
         path.output(out);
      
         return out;
      }
      
      virtual void output(ostream& out)
      {
         output(out, _index);
      }
   
      static void writeBit(ostream& out, bool bit)
      {
         if (bit)
            out << '1';
         else
            out << '0';
      }
      
      void output(ostream& out, Index index)
      {
         Branch branch =
            _database.getBranch(index);
         
         if (branch._left)
         {
            // Write the left branch
            Path::writeBit(out, true);
            output(out, branch._left);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         if (branch._right)
         {
            // Write the right branch
            Path::writeBit(out, true);
            output(out, branch._right);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         return;
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
      
   protected:
      
      class Contains :
         public Encoding
      {
      protected:
         bool _isDeadEnd;
         Database& _database;
         Index _index;
      public:
         Contains
         (
            Database& database,
            Index index
         ) :
            _database(database),
            _index(index)
         {
            Branch& branch =
               _database.getBranch(_index);
               
            _isDeadEnd = branch.isDeadEnd();
         }
         
         virtual void writeBit(bool bit)
         {
            if (_isDeadEnd)
               return;
            
            Branch& branch =
               _database.getBranch(_index);
            
            if (!bit && branch._left)
               _index = branch._left;
            else if (bit && branch._right)
               _index = branch._right;
            else
               _isDeadEnd = true;
         }
         
         virtual bool readBit()
         {
            throw logic_error("readBit() not implemented.");
         }
         
         virtual bool peekBit()
         {
            throw logic_error("peekBit() not implemented.");
         }
         
         virtual bool isDeadEnd()
         {
            return _isDeadEnd;
         }
         
         template <class T>
         Path<Encoding> contains
         (const T& object)
         {

            Encoding* encoding = this;
            
            *encoding << object;
         
            if (!_isDeadEnd)
            {
               return
                  Path<Encoding>
                  ( _database, _index );
            }
            else
               return Path<Encoding>
                  ( _database );
         }
         
      };
      
      
   };

   
}

#endif