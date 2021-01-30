#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		private:
		   const Match& _template;
		   Match* _match;
		public:
		   LoadOnDemand(const Match& source) :
		      Match(),
		      _template(source),
		      _match(NULL)
		   {
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      Match(),
		      _template(source._template),
		      _match(NULL)
		   {
		   }
		   
		   virtual bool match(int character)
		   {
		      Match& _match = item();
		      
		      bool matched =
		         _match.match(character);
		      
		      if (matched)
		         Match::match(character);
		      
		      if (_match.result() == true) {
		         success();
		      }
		      else if (_match.result() == false) {
		         fail();
		      }
		         
		      return matched;
		   }
		   
		   virtual Match* createItem()
		   {
		      Match* item = _template.copy();
		      return item;
		   }
		   
		   virtual ~LoadOnDemand() {
		      if (_match)
		      {
		         delete _match;
		         _match = NULL;
		      }
		   }
		 
		   virtual Match& item()
		   {
		      if (!_match)
		      {
		         _match = createItem();
		      }
		      return *_match;
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