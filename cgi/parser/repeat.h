#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"

namespace bee::fish::parser {

class Repeat : public Match {
private:
   Match* _match;
   string _value = "";
   
protected:
   vector<Match*> _items;
   
  
public:

   Repeat(Match* repeat) : Match()
   {
      _match = repeat;
   }
   
   Repeat(const Repeat& source) :
      Match(source)
   {
      _match = source._match->copy();
   }
   
   virtual ~Repeat() {
   
      for (vector<Match*>::iterator
              it = _items.begin();
              it != _items.end();
              ++it)
      {
         Match* item = *it;
         delete item;
      }
      
      delete _match;
   }
   
   virtual optional<bool>
   match(int character)
   {

      optional<bool> matched =
         _match->match(character);
         
      if (matched != false)
         Match::match(character);
         
      if (_match->success() == true) {
      
         _items.push_back(
            _match
         );
         
         addItem(_match);
            
         _match = _match->copy();
           
      }
      else if
         (
            matched == false ||
            character == Match::eof
         )
      {
         _success =
            (_items.size() > 0);
      }
      
      return matched;
      
   }
  
   virtual const vector<Match*>& items() const {
       return _items;
   }
   
   virtual void addItem(Match* match) {
      
   }
   
   virtual void write(ostream& out) const {
      out << "Repeat";
   }
   
   virtual Match* copy() {
      Repeat* copy = new Repeat(*this);
      return copy;
   }
   
};


};

#endif
