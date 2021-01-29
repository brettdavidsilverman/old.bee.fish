#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		private:
		   const Match& _source;
		   Match* _item;
		public:
		   LoadOnDemand(const Match& source) :
		      Match(),
		      _source(source),
		      _item(NULL)
		   {
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _source(source._source),
		      _item(NULL)
		   {
		   }
		   
		   virtual bool match(int character)
		   {
		      Match& _item = item();
		      
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
		   
		   virtual Match* createItem()
		   {
		      Match* item = _source.copy();
		      return item;
		   }
		   
		   virtual ~LoadOnDemand() {
		      if (_item)
		      {
		         delete _item;
		         _item = NULL;
		      }
		   }
		 
		   virtual Match& item()
		   {
		      if (!_item) {
		         _item = createItem();
		      }
		      return *_item;
		   }
		   
		   virtual Match* itemPtr()
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
			   
			  virtual Match* copy() const
      {
         return new LoadOnDemand(*this);
      }
   };

}

#endif