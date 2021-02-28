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
   typedef shared_ptr<Match> MatchPtrBase;
   
   class Match {
   protected:
      BString _value;
      
      inline static unsigned long _matchInstanceCount = 0;
   public:
   
      optional<bool> _result;
      MatchPtrBase _item;
   public:
   
      Match() :
         _result(nullopt)
      {
        ++_matchInstanceCount;
      }
      
      Match(MatchPtrBase item) : Match()
      {
         _item = item;
         ++_matchInstanceCount;
      }
      
   public:
   
      static const Char EndOfFile = -1;
  
      virtual Match* copy() const = 0;
      
      virtual ~Match()
      {
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
            if (getNext(input, character))
            {
               if (character == Match::EndOfFile)
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
            cerr << "{" << Match::EndOfFile << "}";
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
   
   protected:
      
      bool getNext(
         istream& input,
         Char& character
      )
      {
         int nextChar;
         UTF8Character utf8;
         
         while ( !input.eof() )
         {
            nextChar = input.get();
            
            if ((Char)nextChar == Match::EndOfFile)
            {
               character = Match::EndOfFile;
               return true;
            }
               
            utf8.match(nextChar);
            
            if (utf8._result != nullopt)
               break;
         }
      
         if (utf8._result == true)
         {
            character = utf8._character;
            return true;
         }
         else
            return false;
      }
   
   public:
      virtual optional<bool> result() const
      {
         return _result;
      }

      virtual bool match(Char character)
      {
         if (!_item)
            return false;
            
         bool matched = _item->match(character);
         
         if (_item->_result == true)
            success();
         else if (_item->_result == false)
            fail();
         
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
 
      virtual Match& item()
      {
         return *_item;
      }

      virtual BString& value()
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
      
      friend ostream& operator <<
      (ostream& out, const Match& match)
      {
         
         match.write(out);
         
         return out;
      }
 
		   
   };

   


}

#endif


