#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		protected:
		   const MatchPtr& _template;
		   Match* _item = NULL;
		public:
		   LoadOnDemand(const MatchPtr& template_) :
		      _template(template_)
		   {
		      
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _template(source._template)
		   {
		   }
		   
		   virtual bool match(int character)
		   {
		      if (!_item)
		         _item = createItem();
		         
		      bool matched =
		         _item->match(character);
		         
		      if (_item->result() == true)
		         success();
		      else if (_item->result() == false)
		         fail();
		     
		      return matched;
		   }
		   		   
		   virtual Match* createItem()
		   {
		      return _template->copy();
		   }
		   
		   virtual ~LoadOnDemand() {
		      if (_item)
		         delete _item;
		   }
		 
		public:
		   virtual Match& item()
		   {
		      return *_item;
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
         
         out << *_template;
         
         out << ")";
      }
      
      virtual optional<bool> result()
      {
         if (_item)
            return _item->result();
         return nullopt;
      }
   };

}

#endif