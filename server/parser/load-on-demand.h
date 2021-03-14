#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		public:
		   Match*& _template;
		   
		public:
		   LoadOnDemand(Match*& template_) :
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
		   		   
		   virtual Match* createItem()
		   {
		      return _template->copy();
		   }
		   
		public:
			   
			  virtual Match* copy() const
      {
         return new LoadOnDemand(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << "LoadOnDemand";
         
         writeResult(out);
         
         out << "(";
         
         out << *_template;
         
         out << ")";
      }
      

   };

}

#endif