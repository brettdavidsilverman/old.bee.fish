#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <sys/file.h>
#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <pthread.h>
#include "file.h"
#include "memoryFile.h"

using namespace std;

const string databasePath =
   "database.b2";

const off_t databaseSize =
      100000000;
      
class DB :
   public MemoryFile,
   public ostream,
   public istream
{

protected:
   off_t _pointer = 0;
   
   off_t  _offset;
   off_t* _count;
  
   
   struct _Branch {
      off_t _left;
      off_t _right;
   };
   
   typedef struct _Branch Branch;
   
   Branch* _branch;
   Branch* _branches;
   
public:

   DB(const string path = "database.b2",
      off_t size = 0,
      off_t start = 0
     ) :
        MemoryFile(path, size)
   {
      _offset = 0;
      
      _count = (off_t*)(
         (char*)_memory + _offset
      );
      
      _offset += sizeof(_count);
      
      _branches = (Branch*)(
         (char*)_memory + _offset
      );
      
      _branch = _branches + start;

   }
 
   virtual ~DB() {

   }
   
   virtual void log(ostream& stream) const {
      stream 
           << "_size:    "
           << _size
           << endl
           << "_count:   "
           << *_count
           << endl
           << "length:   "
           << length()
           << endl
           << "_pointer: "
           << _pointer
           << endl;
   }

   DB& operator >> (char& c) {
   
      c = getChar();
      
      return *this;
   }
   
   DB& operator >> (string& string) {
      char c = 0;
      
      while (
         _branch->_left || 
         _branch->_right )
      {
         c = getChar();
         string.push_back(c);
      }
      setstate(std::ios_base::eofbit);
      return *this;
   }
   
   DB& operator << (const string str)
   {
      putString(str);
      return *this;
   }
   
   DB& operator << (const char* str)
   {
      const string& string = str;
      putString(string);
      return *this;
   }
   
   DB& operator << (const char c)
   {
      putChar(c);
      return *this;
   }
   
   void sync() {
      MemoryFile::sync();
   }
   
   void setPointer(off_t pointer) {
      _pointer = pointer;
      _branch = _branches + pointer;
   }

   off_t getPointer() {
      return _pointer;
   }

   off_t length() const {
      return
         _offset        +
         *_count        *
         sizeof(Branch);
   }
   
protected:

   inline void writeBit(bool bit) {
      
      ostream& output = cout;
      
      if (bit == 0) {
         // Chose left branch
         off_t& left = _branch->_left;
         
         if (left == 0) {
            // I chose the path
            // less travelled
            
            left = ++(*_count);
         }
            
         _branch = _branches + left;
         _pointer = left;
      }
      else {
         // Chose right branch
         off_t& right = _branch->_right;
         
         if (right == 0) {
            // I chose the path
            // less travelled
            right = ++(*_count);
         }
            
         _branch =  _branches + right;
         _pointer = right;
      }
      
   }

   inline void putChar(const char c) {
      bool bit;
     
      for (int bitMask = 0b10000000;
           bitMask != 0;
           bitMask >>= 1) {
         bit = c & bitMask;
         
         writeBit(bit);
      }

      
   }
   
   void putString(const string& str) {

      off_t length = str.length();
      
      for (int i = 0; i < length; ++i) {
         char c = str[i];
         putChar(c);
      }
      if ( _branch->_left == 0 &&
           _branch->_right == 0 )
         setstate(std::ios_base::eofbit);

   }
   
   inline char getChar() {
      char c = 0;
      
      for (int i = 0; i < CHAR_BIT; i++)
      {
         c <<= 1;
         if (_branch->_left) {
            c &= 0b11111110;
            _pointer = _branch->_left;
         }
         else if (_branch->_right) {
            c |= 0b00000001;
            _pointer = _branch->_right;
         }
         else {
            cerr << "End Of File" << endl;
            throw "End of file";
         }
         
         _branch = _branches + _pointer;
            
      }
      
      return c;
   }
   
 
};

ostream& operator << (ostream& stream, const DB& db) {
   db.log(stream);
   return stream;
}

int main(int argc, char* argv[], char* envp[]) {

//   cout << "HTTP/1.1 200 OK" << endl;

      
   cout << "Content-type: text/plain"
        << endl
        << endl;
        
   
    //*stderr = *stdout;
   
   DB db(databasePath, databaseSize);
   
   off_t folder = 0;
   off_t next = 0;
   
   char* pathInfo = getenv("PATH_INFO");
   if (pathInfo) {
      db << pathInfo;
      folder = db.getPointer();
   }
   
   // Query string
   char* queryString = getenv("QUERY_STRING");
   if (queryString) {
      db.setPointer(folder);
      db << queryString;
      next = db.getPointer();
   }
   else if (argc > 1) {
      // Command line arguments
      for (int i = 1; i < argc; ++i) {
         char* arg = argv[i];

         db << arg;
      }
      next = db.getPointer();
   }
   else {
      // Standard input
      std::string line;
      do
      {
         getline(cin, line);
         db.setPointer(folder);
         db << line;
      }
      while (
         line.length() > 0 && 
         cin.eof() == false
      );
   }
   
   // get the result
   std::string nextString;
   db.setPointer(next);
   db >> nextString;

   cout << nextString << endl;
   cout << db.length() << endl;
   
   return 0;
  
}


