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
   vector<Match*> _items;
   size_t _matchedCount = 0;
  
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
         Match* child = *it;
         if (child)
         {
            delete child;
         }
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
      
      if (childSuccess == true)
      {
      
         addItem(_match);
         
         _match = createItem();
         
         ++_matchedCount;
      }
      
      else if (
            (childSuccess == false) ||
            (!matched) ||
            (character == Match::endOfFile)
         )
      {
         if (_matchedCount > 0)
         {
            success = true;
            onsuccess();
         }
         else
         {
            success = false;
            onfail();
         }
         
      }
      
      return matched;
      
   }
   
   virtual vector<Match*>& items() {
       return _items;
   }
   
   virtual T* createItem() {
      return new T();
   }
   
   virtual void addItem(Match* match) {
      
      items().push_back(
         match
      );
      
   }
   
   
   virtual string name()
   {
      return
         "Repeat" + 
         to_string(_matchedCount);
   }
   

};


};

#endif
