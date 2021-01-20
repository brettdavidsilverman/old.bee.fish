#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		template<class T>
		class LoadOnDemand : public Match
		{
		private:
		   T* _item;
		
		public:
		   LoadOnDemand() : Match() {
		      _item = NULL;
		   }
		   
		   virtual bool match(int character)
		   {
		      T& _item = item();
		      
		      bool matched =
		         _item.match(character);
		      
		      if (matched)
		         Match::match(character);
		      
		      if (_item.result() == true) {
		         success();
		      }
		      else if (_item.result() == false) {
		         fail();
		      }
		         
		      return matched;
		   }
		   
		   virtual T* createItem()
		   {
		      return new T();
		   }
		   
		   virtual ~LoadOnDemand() {
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
		   
		   virtual T* itemPtr()
		   {
		      if (!_item) {
		         _item = createItem();
		      }
		      return _item;
		   }
		   
		   virtual string name()
		   {
		      return "LoadOnDemand";
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) 
      {
         _item = NULL;
      }
			   
      virtual Match* copy() const
      {
         return new LoadOnDemand(*this);
      }
			   
   };

}

#endif