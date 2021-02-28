#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		public:
		   const MatchPtr& _template;
		   MatchPtr _item;
		   
		public:
		   LoadOnDemand(const MatchPtr& template_) :
		      _template(template_)
		   {
		      
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _template(source._template)
		   {
		   }
		   
		   virtual bool match(Char character)
		   {
		      if (!_item)
		         _item = createItem();
		         
		      return Match::match(character, *_item);
		   }
		   		   
		   virtual MatchPtr createItem()
		   {
		      return _template->copy();
		   }
		   
		public:
			   
			  virtual MatchPtr copy() const
      {
         return new LoadOnDemand(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "LoadOnDemand";
         
         writeResult(out);
         
         out << "(";
         
         out << *_template;
         
         out << ")";
      }
      

   };

}

#endif