#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser {

		class LoadOnDemand : public Match
		{
		public:
		   const Match& _template;
		   
		public:
		   LoadOnDemand(const Match& template_) :
		      _template(template_)
		   {
		      
		   }
		   
		   LoadOnDemand(const LoadOnDemand& source) :
			  Match(),
		      _template(source._template)
		   {
		   }
		   
		   virtual void setup()
		   {
		      if (!_match)
		         _match = createItem();
		         
         _setup = true;
		      
		   }
		   		   
		   virtual Match* createItem()
		   {
		      Match* match = _template.copy();
		      return match;
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
         
         out << endl;
         
         out << tabs(tabIndex) << "(" << endl;
         
         _template.write(out, tabIndex + 1);
         
         out << endl;
         
         out << tabs(tabIndex) << ")";
      }
      

   };

}

#endif