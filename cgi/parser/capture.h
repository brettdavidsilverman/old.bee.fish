#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H
#include <ostream>
#include <vector>
#include <experimental/optional>
#include <map>

#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class Capture : public Match {
   protected:
      Match* _match;
      
   public:
      Capture( const string& name,
               Match* match )
         : Match()
      {
         _name = name;
         _match = match;
         insert_input_into_map(this);
      }
      
      Capture(const Capture& source) :
         Match(source)
      {
         _name = source._name;
         _match = source._match->copy();
         insert_input_into_map(this);
      }
 
      
      Capture(const string& name, 
              const Match& source) :
         Match(source)
      {
         _name = name;
         _match = source.copy();
         insert_input_into_map(this);
      }
      
      virtual Match* copy() const {
         Capture* copy =
            new Capture(*this);
         return copy;
      }
   
      virtual ~Capture() {
         delete _match;
      }
   
      virtual bool match(int character)
      {
         bool matched =
            _match->match(character);
         
         if ( matched &&
              character != Match::eof )
         {
            _value += (char)character;
         }
            
         return matched;
      }
   
      virtual optional<bool> success() const
      {
         return _match->success();
      }
      
      virtual const string& value() const
      {
         return _value;
      }
      
      virtual vector<Match*>& inputs() const
      {
         return _match->inputs();
      }
      
      virtual void set_success(
         optional<bool> value
      )
      {
         _match->set_success(value);
      }
      
      virtual void write(ostream& out) const
      {
       
         out << "Capture("
             << "\"" << name() << "\","
             << success() 
             << ",";
         for (auto pair : _capture)
             out << pair.first << ",";
        // out << value();
         out << *_match;
         out << ")";
      }
      
      
   };
      
 
};

#endif
