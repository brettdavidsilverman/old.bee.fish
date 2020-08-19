#ifndef BEE_FISH_PARSER__LAZY_LOAD_H
#define BEE_FISH_PARSER__LAZY_LOAD_H

#include "match.h"

namespace bee::fish::parser {

template<class T>
class LazyLoad : public Match
{
private:
   T* _item;

public:
   LazyLoad() : Match() {
      _item = NULL;
   }
   
   virtual bool match
   (
      int character,
      optional<bool>& success
   )
   {
      T& _item = item();
      
      optional<bool> childSuccess = nullopt;
      
      bool matched =
         _item.match(
            character, childSuccess
         );
      
      if (matched)
         Match::match(
            character, childSuccess
         );
      
      if (childSuccess == true) {
         success = true;
         onsuccess();
      }
      else if (childSuccess == false)
         success = false;
         
      return matched;
   }
   
   virtual T* createItem()
   {
      return new T();
   }
   
   virtual ~LazyLoad() {
      if (_item)
      {
         delete _item;
         _item = NULL;
      }
   }
   
   virtual void onsuccess()
   {
      Match::onsuccess();
      if (_item) 
      {
         delete _item;
         _item = NULL;
      }
      
   }
 
   virtual T& item()
   {
      if (!_item) {
         _item = createItem();
      }
      return *_item;
   }
   

   
   virtual string name()
   {
      return "LazyLoad";
   }
   
};

}

#endif