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
      }
      
      Match(const Match& source);
      
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

         
         Char character;
         
         while (!input.eof())
         {
            bool read = 
               UTF8Character::read(
                  input, character
               );
               
            if (read)
            {
               if (character == BString::EndOfFile)
               {
                  break;
               }
#ifdef DEBUG   
               BString::writeEscaped(cerr, character);
#endif
               match(character);
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
   
      bool matched()
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
         if (_match)
            return match(character, *_match);
         else
            return false;
      }
      
      virtual bool match(
         Char character,
         Match& item
       )
      {
         bool matched =
            item.match(character);
         
         if (item._result == true)
            success();
         else if (item._result == false)
            fail();
               
         if (matched)
            capture(character);
            
         return matched;
      }

      void setMatch(MatchPtrBase match)
      {
         _match = match;
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
         out << "Match";
         writeResult(out);
         out << "()";
      }
   
      virtual void writeResult(ostream& out) const
      {
         out << "<"
             << _result
             << ">";
      }
      
      void capture(Char character)
      {
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


