#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <chrono>
#include <bitset>
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
  
      inline static unsigned long _matchInstanceCount = 0;

      optional<bool> _result;
   protected:
      Match* _match;
   public:
   
      Match() :
         _result(nullopt),
         _match(NULL)
      {
        ++_matchInstanceCount;
      }
      
      Match(Match* copy) :
         _result(nullopt),
         _match(copy)
      {
         ++_matchInstanceCount;
      }

   public:
   
      static const int EndOfFile = -1;
  
      Match(const Match& assign) : Match()
      {
         copyFromAssign(assign);
      }
      
      Match& operator = (const Match& assign)
      {
         copyFromAssign(assign);
            
         return *this;
      }
     
      virtual void copyFromAssign(const Match& assign)
      {
         if (_match)
            delete _match;

         _match = assign.copy();
      }
      
      virtual Match* copy() const
      {
         if (_match)
         {
            return _match->copy();
         }
         else
            return new Match(*this);
         
      }
      

      virtual ~Match()
      {
         if (_match)
         {
            delete _match;
            _match = NULL;
         }
         --_matchInstanceCount;
      }
     
      unsigned long now()
      {
         return
            std::chrono::duration_cast
            <std::chrono::milliseconds>
            (
               std::chrono::system_clock
                  ::now()
                     .time_since_epoch()
            ).count();
      }
      
      virtual optional<bool> read(
         istream& in,
         bool last = true
      )
      {
         _result = nullopt;
         
#ifdef TIME
         unsigned long readCount = 0;
         
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         
         
         while (!in.eof())
         {
            int character = in.get();

            if (character == Match::EndOfFile)
               break;
#ifdef TIME
            if (++readCount % 1000 == 0)
            {
               unsigned long time =
                  now() - start;
                  
               cout << readCount << "\t" << _matchInstanceCount << "\t" << time << endl;
               start = now();
            }
            
#endif

#ifdef DEBUG
           // bitset<8> bits(character);
           // cerr << bits << ",";
           // cerr << (char)character;
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
            matched = _match->match(
               character
            );
            
            if (_match->result() == true)
               success();
            else if (_match->result() == false)
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
 
      virtual const Match& match() const
      {
         if (_match)
            return *_match;
         else
            return *this;
      }
      
      virtual void write(ostream& out) const
      {
         if (_match)
            _match->write(out);
         else
         {
            out << "Match";
            writeResult(out);
            out << "()";
         }
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
   
   public:
      
      template<class stream>
      static void write(stream& out, int character)
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
            if (character <= 128)
               out << (char)character;
            else
               out << (wchar_t)character;
         }
      }
      
		   
   };



}

#endif


