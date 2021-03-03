#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		public:
		   const MatchPtr& _template;
		   
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
		      if (!_match)
		         _match = createItem();
		         
		      return Match::match(character, *_match);
		   }
		   		   
		   virtual MatchPtr createItem()
		   {
		      return _template->copy();
		   }
		   
		public:
			   
			  virtual MatchPtrBase copy() const
      {
         return make_shared<LoadOnDemand>(*this);
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