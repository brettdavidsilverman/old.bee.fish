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
      size_t _byteCount = 0;
      bool _setup = false;
      vector<Match*> _inputs;
      deque<Char::Value> _buffer;
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
      
      virtual ~Match()
      {
      
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
         istream& input
      )
      {
         _result = nullopt;
         
         setup();
         
#ifdef TIME
         unsigned long readCount = 0;
         
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         
         Char::Value value = 0;
         Char character;
         
         while (!input.eof())
         {
            size_t bytesRead = 
               UTF8Character::read(
                  input, value, _buffer
               );
 
            if ((int)value == -1)
               break;
               
            if (bytesRead)
            {
               _byteCount += bytesRead;
               _character = value;
#ifdef DEBUG
               _character.writeEscaped(cerr);
#endif
               match(_character);
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
         
         return _result;
      }
   
      virtual optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
   
      virtual bool matched() const
      {
         if (_match)
            return _match->matched();
            
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
         if (_match)
         {
            _match->_capture |= _capture;
            _match->setup();
         }
         
         
         for (auto input : _inputs)
         {
            if (input)
            {
               input->_capture |= _capture;
               input->setup();
            }
         }
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
      
      virtual const Char& character() const
      {
         return _character;
      }
      
      virtual Match& item()
      {
         if (_match)
            return _match->item();
            
         return *this;
      }
      
      virtual const Match& item() const
      {
         if (_match)
            return _match->item();
            
         return *this;
      }
      
      virtual const BString& value() const
      {
         return item()._value;
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
         {
            out << endl
                << tabs
                << "("
                << endl;
                _match->write(out, tabIndex + 1);
            out << endl
                << tabs
                << ")";
         }
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


