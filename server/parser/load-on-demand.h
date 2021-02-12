#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		protected:
		   const Match& _template;
		   Match* _item = NULL;
		public:
		   LoadOnDemand(const Match& tem) :
		      _template(tem)
		   {
		      
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _template(source._template)
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
		      if (_item)
		      {
		         delete _item;
		         _item = NULL;
		      }
		   }
		 
		public:
		   virtual Match& item()
		   {
		      return *itemPtr();
		   }
		   
		protected:
		   virtual Match* itemPtr()
		   {
		      if (!_item)
		      {
		         _item = createItem();
		      }
		      return _item;
		   }
		   
		public:
			   
			  virtual Match* copy() const
      {
         Match* copy = 
            new LoadOnDemand(*this);
         return copy;
         
      }
      
      virtual void write(ostream& out) const
      {
         out << "LoadOnDemand";
         
         writeResult(out);
         
         out << "(";
         
         out << _template;
         
         out << ")";
      }
      
      virtual optional<bool> result()
      {
         return itemPtr()->result();
      }
   };

}

#endif