#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser {

template<class T>
class Repeat : public Match
{
private:
   T* _match;
   
protected:
   vector<T*> _items;
   
  
public:

   Repeat() : Match()
   {
      _match = NULL;
   }
   
   virtual ~Repeat() {
   
      for (auto
              it = _items.cbegin();
              it != _items.cend();
              ++it)
      {
         delete (*it);
      }
      
      if (_match != NULL) {
         delete _match;
         _match = NULL;
      }
   }
   
   virtual bool match
   (
      int character,
      optional<bool>& success
   )
   {

      if (_match == NULL)
         _match = createItem();
         
      optional<bool>
         childSuccess = nullopt;
         
      bool matched =
         _match->match(
            character, childSuccess
         );
         
      if (matched)
         Match::match(character, success);
         
      if (childSuccess == true) {
      
         _items.push_back(
            _match
         );
         
         this->addItem(_match);
            
         _match = createItem();
           
      }
      else if
         (
            !matched ||
            character == Match::endOfFile
         )
      {
         success = (_items.size() > 0);
         if (success == true)
            onsuccess();
         
      }
      
      return matched;
      
   }
   
   virtual vector<T*>& items() {
       return _items;
   }
   
   virtual T* createItem() {
      return new T();
   }
   
   virtual void addItem(Match* match) {
      
   }
   
   virtual void write(ostream& out)
   {
      out << "Repeat(";
      
      Match::write(out);
      
      out << ":";
      
      for (auto it = _items.cbegin();
                it != _items.cend();)
      {
         out << *(*it);
         
         if (++it != _items.cend())
            out << ",";
     
	      }
       
      out << ")";
   }

};


};

#endif
