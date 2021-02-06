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
		      _template(source),
		      _match(NULL)
		   {
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _template(source._template),
		      _match(NULL)
		   {
		   }
		   
		   virtual bool match(int character)
		   {
		      Match* item = itemPtr();
		      bool matched =
		         Match::match(item, character);
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
		 
		   virtual Match* itemPtr()
		   {
		      if (!_match)
		      {
		         _match = createItem();
		      }
		      return _match;
		   }
		   
			   
			  virtual Match* copy() const
      {
         return new LoadOnDemand(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "LoadOnDemand";
         
         writeResult(out);
         
         out << "(";
         
         out << _template;
         
         out << ")";
      }
   };

}

#endif