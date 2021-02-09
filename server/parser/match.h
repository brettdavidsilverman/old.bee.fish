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
   private:
   
      Match* _match = NULL;
      optional<bool> _result = nullopt;
   
   public:
   
      Match()
      {
      }
      
      Match(Match* copy)
      {
         _match = copy;
      }

   public:
   
      static const int EndOfFile = -1;
      
      Match(const Match& assign)
      {
         copyFromAssign(assign);
      }
      
      Match& operator = (const Match& assign)
      {
         copyFromAssign(assign);
            
         return *this;
      }
     
      void copyFromAssign(const Match& assign)
      {
         if (_match)
            delete _match;

         if (assign._match)
         {
            _match = assign._match->copy();
         }
         else
            _match = assign.copy();
      }
      
      virtual Match* copy() const
      {
         if (_match)
         {
            return _match->copy();
         }
         else
            return new Match();
         
      }
      

      virtual ~Match()
      {
         if (_match)
         {
            delete _match;
            _match = NULL;
         }
      }
     
      virtual optional<bool> read(
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
         
         }
         
         return _result;
      }
   
      virtual optional<bool> read(const string& str, bool last = true)
      {
      
         istringstream in(str);
      
         return read(in, last);
      
      }
   
      virtual optional<bool> result() const
      {
         return _result;
      }

      virtual bool match(int character)
      {
         bool matched = false;
         
         if (_match)
         {
            matched = match(
               _match,
               character
            );
            
         }
         
         return matched;
      }

      virtual bool match(Match* match, int character)
		   {
		      bool matched =
		         match->match(character);
		         
		      if (match->result() == true)
		      {
		         success();
		      }
		      else if (match->result() == false)
		      {
		         fail();
		      }
		         
		      return matched;
		   }
		   
    
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }
 
      virtual void write(ostream& out) const
      {
         if (_match)
            _match->write(out);
      }
   
      virtual void writeResult(ostream& out) const
      {
         out << "<"
             << _result
             << ">";
      }
      
      friend ostream& operator <<
      (ostream& out, const Match& match)
      {
         
         match.write(out);
         
         return out;
      }
   
   protected:
      
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
            out << "{-1}";
            break;
         default:
            out << (char)character;
         }
      }
      
		   
   };



}

#endif


