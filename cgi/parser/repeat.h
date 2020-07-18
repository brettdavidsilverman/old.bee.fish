#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser {

class Repeat : public Match {
private:
   Match* _match;
   
protected:
   vector<Match*> _items;
   
  
public:

   Repeat(Match* match) : Match()
   {
      _match = match;
   }
   
   Repeat(const Match& match) : Match()
   {
      _match = match.copy();
   }
   
   Repeat(const Repeat& source) :
      Match()
   {
      _match = source._match->copy();
   }
   
   virtual ~Repeat() {
   
      for (auto
              it = _items.cbegin();
              it != _items.cend();
              ++it)
      {
         delete (*it);
      }
      
      delete _match;
   }
   
   virtual bool
   match(int character)
   {

      bool matched =
         _match->match(character);
         
      if (matched)
         Match::match(character);
         
      if (_match->success() == true) {
      
         _items.push_back(
            _match
         );
         
         this->add_item(_match);
            
         _match = _match->copy();
           
      }
      else if
         (
            !matched ||
            character == Match::eof
         )
      {
         set_success(
            (_items.size() > 0)
         );
         
      }
      
      return matched;
      
   }
  
   virtual const vector<Match*>& items() const {
       return _items;
   }
   
   virtual void add_item(Match* match) {
      
   }
   
   
   virtual Match* copy() const {
      Repeat* copy = new Repeat(*this);
      return copy;
   }
   
   virtual void write(ostream& out) const
   {
      out << "Repeat(";
      
      Match::write(out);
      for (auto it = _items.cbegin();
                it != _items.cend();
                )
      {
         out << (*it)->value();
         ++it;
         if (it != _items.cend())
            out << ",";
	      }
       
      out << ")";
   }

};


};

#endif
