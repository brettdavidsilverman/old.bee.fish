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

#include "../b-string/string.h"

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

using namespace bee::fish::b_string;

namespace bee::fish::parser {

   class Match;

   class Match {
   protected:
   
      
   public:
      inline static unsigned long _matchInstanceCount = 0;
      optional<bool> _result = nullopt;
      Match* _match = nullptr;
      BString _value;
      bool _capture = false;
      Char _character = -1;
      bool _setup = false;
      vector<Match*> _inputs;
      
   public:
   
      Match(Match* match)
      {
         _match = match;
      }
      
      template<typename ...T>
      Match(T*... inputs) :
         _inputs{inputs...}
      {
         if (_inputs.size() == 1)
         {
            _match = _inputs[0];
            _inputs.clear();
            return;
         }
         
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
         
         _capture = source._capture;
         
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
         {
            delete _match;
            _match = nullptr;
         }
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
                  input, character._character
               );
               
            if (next)
            {
               _character = character;
               
               if (character == BString::EndOfFile)
               {
                  break;
               }
#ifdef DEBUG
               character.writeEscaped(cerr);
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
            cerr << "{-1}";
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
         return (_result == true);
      }

   public:

      virtual bool match(const Char& character)
      {
         if (!_setup)
            setup();
         
         if (!_match) 
         {
            throw runtime_error("Null match pointer");
         }
         
         return match(character, *_match);
      }
      
      virtual void setup()
      {
         _setup = true;
      }
      
      virtual bool match(
         const Char& character,
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
      
      virtual const BString& value() const
      {
         return _value;
      }
      
      virtual const Char& character() const
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
      
      virtual void capture(const Char& character)
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


