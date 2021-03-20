#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match {
   protected:
      bool _matched = false;
   
   public:
      Optional(Match* match)
      {
         _match = match;
      }
     
      Optional(const Optional& source) :
         Match(source)
      {
      }
      
		   virtual bool match(Char character)
		   {
		     
		      bool matched =
		         _match->match(character);
		      
		      bool succeeded = false;
		      
		      if (_match->result() == true)
		      {
		      
		         _matched = true;
		         succeeded = true;
		         
		      } 
		      else if (_match->result() == false)
		      {
		         matched = false;
		         succeeded = true;
		      }
		      else if (character == BString::EndOfFile)
		      {
		         succeeded = true;
		      }
		      
		      if (matched)
		         capture(character);
		         
		      if (succeeded)
		         success();
		         
		      return matched;
		   }
      
      virtual Match* copy() const
      {
         return new Optional(*this);
      }
   
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << "Optional";
         
         writeResult(out);
         out << endl;
         out << tabs(tabIndex) << "(" << endl;
         _match->write(out, tabIndex + 1);
         out << endl;
         out << tabs(tabIndex) << ")";
      }
      
   };


}

#endif
