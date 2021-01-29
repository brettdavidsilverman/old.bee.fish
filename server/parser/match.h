#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "../id/id.h"

using namespace std;

inline ostream& operator <<
(ostream& out, optional<bool> ok)
{
   if (ok == true)
      out << "true";
   else if (ok == false)
      out << "false";
   else
      out << "?";
         
   return out;
}

using namespace bee::fish::server;

namespace bee::fish::parser {

   class Match {
   protected:

      optional<bool> _result = nullopt;
      Match* _source = NULL;
      
      Match() :
         _source(NULL)
      {
      }
      
   public:
      static const int EndOfFile = -1;
      
      Match(const Match& source) 
      {
         if (source._source)
            _source = source._source->copy();
         else
            _source = source.copy();
      }
      
      virtual Match* copy() const
      {
         if (_source)
         {
            return new Match(*_source);//->copy();
         }
         else
            //return new Match(*this);
            throw runtime_error("Match::copy() with no _source. Derived classes must implement copy()");
      }
      
      virtual ~Match()
      {
         if (_source)
            delete _source;
      }
   
      virtual bool match(int character)
      {
         bool matched = true;
         
         if (_source)
         {
            matched = _source->match(
               character
            );
            
            if (_source->result() == true)
               success();
            else if (_source->result() == false)
               fail();
            
         }
         
         return matched;
      }

      virtual bool read(
         istream& in,
         bool last = true
      )
      {
      
         _result = nullopt;
      
         while (!in.eof())
         {
            int character = in.get();

            if (character == Match::EndOfFile)
               break;
#ifdef DEBUG
            Match::write(cerr, character);
#endif
            match(character);
            
            if (_result != nullopt)
               break;
            
         }

         if ( _result == nullopt &&
            last &&
            in.eof()
         )
         {
#ifdef DEBUG
            Match::write(cerr, Match::EndOfFile);
#endif
            match(Match::EndOfFile);
         
            return _result == true;
         }
         
         return (_result != false);
      }
   
      virtual bool read(const string& str, bool last = true)
      {
      
         istringstream in(str);
      
         return read(in, last);
      
      }
   
      virtual optional<bool> result()
      {
         return _result;
      }
   
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }
 
      friend ostream& operator <<
      (ostream& out, Match& match) 
      {

         match.write(out);
   
         return out;

      }
   
      virtual void write(ostream& out) 
      {
      
         out << "{" 
             << name() 
             << ":"
             << _result
             << "}";
      }
   
      virtual string name() {
         return "Match";
      }
   
      virtual bool isOptional()
      {
         return false;
      }
   
      
   
   public:

   
   
      static void write(ostream& out, int character)
      {
         switch (character) {
         case '\"':
            out << "\\\"";
            break;
         case '\\':
            out << "\\\\";
            break;
         case '\b':
            out << "\\b";
            break;
         case '\f':
            out << "\\f";
            break;
         case '\r':
            out << "\\r";
            break;
         case '\n':
            out << "\\n";
            break;
         case '\t':
            out << "\\t";
            break;
         case Match::EndOfFile:
            out << "-1";
            break;
         default:
            out << (char)character;
         }
      }
   
   };



}

#endif


