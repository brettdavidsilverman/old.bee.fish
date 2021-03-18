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

#ifdef DEBUG
#define TIME
#endif

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

   class Match {
   public:
   
      optional<bool> _result = nullopt;
      inline static unsigned long _matchInstanceCount = 0;

      Match* _match = nullptr;
      BString _value;
      bool _capture = true;
      Char _character = -1;
      
      vector<Match*> _inputs;
      
   public:
   
      Match(Match* match) : _match(match)
      {
         ++_matchInstanceCount;
      }
      
      template<typename ...T>
      Match(T*... inputs) :
         _inputs{inputs...}
      {
         ++_matchInstanceCount;
      }

      Match(const Match& source)
      {
      
         for (auto it = source._inputs.begin();
                   it != source._inputs.end();
                 ++it)
         {
            Match* match = *it;
            if (match)
            {
               Match* copy = match->copy();
               _inputs.push_back(copy);
            }
         }
         
         if (source._match)
            _match = source._match->copy();
            
         ++_matchInstanceCount;
      
      }
      
      virtual ~Match() {
      
         for ( auto
               it = _inputs.cbegin();
               it != _inputs.cend();
             ++it )
         {
            Match* child = *it;
            if (child)
            {
               delete child;
            }
         }
         
         if (_match)
            delete _match;
            
         --_matchInstanceCount;
      }
      
   public:
  
      virtual Match* copy() const 
      {
         return new Match(*this);
      };
     
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
               _character = character;
               
               if (character == BString::EndOfFile)
               {
                  break;
               }
#ifdef DEBUG   
              // UTF8Character::write(cerr, character);
               
               //BString::writeEscaped(cerr, character);
#endif
               match(character);
            }
            else
            {
               fail();
               break;
            }
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
/*
#ifdef DEBUG
         if (_result == false)
         {
            cerr << (int)_character << endl;
            cerr << *this << endl;
         }
#endif
*/
         return _result;
      }
   
      virtual optional<bool> read(const string& str, bool last = true)
      {
      
         istringstream in(str);
      
         return read(in, last);
      
      }
   
      bool matched() const
      {
         return (_result == true);
      }
      
      virtual Match* getMatch()
      {
         if (_match)
            return _match->getMatch();
        
         return this;
      }
   
   public:

      virtual bool match(Char character)
      {
         if (!_match)
            setup();
         
         if (!_match) 
         {
            throw runtime_error("Invalid match pointer");
         }
         
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
         if (_match)
            return _match->value();
            
         return _value;
      }
      
      virtual Char character() const
      {
         return _character;
      }
      
      BString tabs(size_t tabIndex) const
      {
         BString tabs =
            std::string(
               tabIndex * 3,
               ' '
            );
            
         return tabs;
              
      }
      
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         BString tabs = Match::tabs(tabIndex);
         
         out << tabs
             << typeid(*this).name();
             
         writeResult(out);
         
         if (_match)
            out << "(" << *_match << ")";
         else
         {
            out << endl;
            out << tabs << "(" << endl;
            writeInputs(out, tabIndex + 1);
            out << tabs << ")";
         }
      }
      
      virtual void writeInputs(
         ostream& out,
         size_t tabIndex
      ) const
      {
         for ( auto it = _inputs.cbegin();
                    it != _inputs.cend();
                  ++it )
         {
            Match* match = *it;
            if (match)
               match->write(out, tabIndex);
            else
               out << tabs(tabIndex) << "NULL";
            if (it + 1 != _inputs.cend())
               out << ",";
            out << endl;
         }
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


