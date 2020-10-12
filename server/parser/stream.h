#ifndef BEE_FISH_PARSER__STREAM_H
#define BEE_FISH_PARSER__STREAM_H

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <optional>
#include <array>
#include <map>
#include <sstream>
#include <unistd.h>
#include <cstring>

#include "match.h"
#include "outbuf.h"

using namespace std;


namespace bee::fish::parser
{
   
   class Match;

   struct Stream:
     // public outbuf,
      public std::iostream
   {
   public:
      Stream() : //outbuf(),
                 std::iostream(new outbuf())
      {
         _index = 0;
         _size = getpagesize();
         _fromString = false;
         _buffer = string(_size, '\0');

      }
      
   protected:
      string _buffer;
      size_t _index = 0;
      size_t _size;
      bool _fromString = false;
      optional<bool> _success = nullopt;
    
      virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
      {
         throw "yes";
         return c;
      }
      
   public:
      
      friend class Match;
      
      class EndLine
      {
      public:
         friend ostream& operator <<
         (ostream& output, const EndLine endl)
         {
            output << "\r\n";
            return output;
         }
      };
      
      static EndLine endl;
      
      void endLine()
      {
         put('\r');
         put('\n');
         writeBuffer();
      }
      
      class Flush
      {
      };
      static Flush flush;
      
      Stream
      (
         const std::string& str
      ) : Stream()
      {
         _buffer = str;
         _size = str.length();
         _index = 0;
         _fromString = true;
         _success = nullopt;
      }
      
   
      Stream& operator =
      (const Stream& source)
      {
         _buffer = source._buffer;
         _size = source._size;
         _index = source._index;
         _fromString = source._fromString;
         _success = source._success;
         
         return *this;
      }
      
      ~Stream()
      {
      }

      
      virtual int get()
      {
         if ( !_fromString &&
              _index == _size )
         {
            
            std::getline(cin, _buffer);
            _index = 0;
            _size = _buffer.size();
            
         }
         
         if (_index == _size)
            return EOF;
         
         char c = _buffer[_index++];
              
         return (int)c;
      }
      
      
      virtual void write(const string& str)
      {
         for (const char c : str) {
            put(c);
         }
      }
      
      virtual ostream& put(int c)
      {
        
         _buffer[_index++] =(int)c;
         
         if (_index == _size) {
            writeBuffer();
         }
         
         return *this;
      }
      
      virtual ostream& write
      (const char* s, size_t n)
      {
         for (size_t i = 0; i < n; i++)
            put(s[i]);
            
         return *this;
      }
   
      virtual bool eof()
      {
         if (_index < _size)
            return false;
            
         if (_fromString)
            return true;
            
         return (feof(stdin));
      }
      
      void writeBuffer()
      {
         fwrite(
            _buffer.data(),
            sizeof(char),
            _index,
            stdout
         );
         fflush(stdout);
         _index = 0;
      }
      
      optional<bool> success()
      {
         return _success;
      }
      
      void setSuccess
      (optional<bool> value)
      {
         _success = value;
      }
      
      Stream& operator <<
      (Match& match);
      
      Stream& operator <<
      (const EndLine endl)
      {
         endLine();
         return *this;
      }
   
   
      Stream& operator <<
      (const Stream::Flush flush)
      {
         writeBuffer();
         return *this;
      }
      
      Stream& operator <<
      (const optional<bool>& value)
      {
         if (value == false)
            put('0');
         else if (value == true)
            put('1');
         else
            put('?');
            
         return *this;
      }
      
      Stream& operator <<
      (const string& str)
      {
         write(str);
         return *this;
      }
   

      Stream& operator <<
      (const char c)
      {
         put(c);
         return *this;
      }

      
   
      Stream& operator <<
      (const char* str)
      {
         write(str);
         return *this;
      }
   
      
      template <typename T>
      Stream& operator<<
      (const T& v)
      {
         ostream::operator<<(v);
         return *this;
      }
     
   };
   
   extern Stream output;
   
}

#endif