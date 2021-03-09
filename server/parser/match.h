#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <chrono>
#include <bitset>
#include <memory>

#include "b-string.h"

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

namespace bee::fish::parser {

   class Match;
   class MatchPtr;
   typedef shared_ptr<Match> MatchPtrBase;
   
   class Match {
   public:
   
      optional<bool> _result = nullopt;
      inline static unsigned long _matchInstanceCount = 0;

      MatchPtrBase _match;
      BString _value;
      bool _capture = true;
      
   public:
   
      Match()
      {
        ++_matchInstanceCount;
        _match = nullptr;
      }
      
      Match(const Match& source)
      {
         if (source._match)
            _match = source._match->copy();
         else
            _match = nullptr;
         ++_matchInstanceCount;
      }
      
      Match(MatchPtrBase match)
      {
         _match = match;
         ++_matchInstanceCount;
      }
      
      
   public:
  
      virtual MatchPtrBase copy() const
      {
         return make_shared<Match>(*this);
      };
      
      virtual ~Match()
      {
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
         istream& input,
         bool last = true
      )
      {
         _result = nullopt;
         
#ifdef TIME
         unsigned long readCount = 0;
         
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         
         Char character = 0;
         
         while (!input.eof())
         {
            bool next = 
               UTF8Character::read(
                  input, character
               );
               
            if (next)
            {
               if (character == BString::EndOfFile)
               {
                  break;
               }
#ifdef DEBUG   
               BString::writeEscaped(cerr, character);
#endif
               bool matched = false;
               while (!matched && _result == nullopt)
                  matched = match(character);
            }
            else
               _result = false;
#ifdef TIME
            if (++readCount % 1000 == 0)
            {
               unsigned long time =
                  now() - start;
                  
               cout << readCount << "\t" << _matchInstanceCount << "\t" << time << endl;
               start = now();
            }
#endif
            if (_result != nullopt)
               break;
            
         }

         if ( _result == nullopt &&
              last &&
              input.eof()
         )
         {
#ifdef DEBUG
            BString::writeEscaped(cerr, BString::EndOfFile);
#endif
            match(BString::EndOfFile);
         
         }

         return _result;
      }
   
      virtual optional<bool> read(const string& str, bool last = true)
      {
      
         istringstream in(str);
      
         return read(in, last);
      
      }
   
      bool matched() const
      {
         return _result == true;
      }
      
   protected:
      

   
   public:
      virtual optional<bool> result() const
      {
         return _result;
      }

      virtual bool match(Char character)
      {
         if (!_match)
            setup();
            
         return match(character, *_match);
      }
      
      virtual void setup()
      {
      }
      
      virtual bool match(
         Char character,
         Match& item
       )
      {
         bool matched =
            item.match(character);
        
         if (matched)
            capture(character);
            
         if (item._result == true)
            success();
         else if (item._result == false)
            fail();
               

         return matched;
      }

      void setMatch(MatchPtrBase match)
      {
         _match = match;
      }
      
      MatchPtrBase match() const
      {
         return _match;
      }
      
      virtual MatchPtrBase item() const
      {
         return _match;
      }
      
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }

      virtual BString value() const
      {
         return _value;
      }
      
      virtual void write(ostream& out) const
      {
         out << typeid(*this).name();
         writeResult(out);
         if (_match)
            out << "(" << *_match << ")";
         else
            out << "()";
      }
   
      virtual void writeResult(ostream& out) const
      {
         out << "<"
             << _result
             << ">";
      }
      
      virtual void capture(Char character)
      {
         if (_capture)
            _value.push_back(character);
      }
      
      friend ostream& operator <<
      (ostream& out, const Match& match)
      {
         
         match.write(out);
         
         return out;
      }
 
		   
   };

   


}

#endif


