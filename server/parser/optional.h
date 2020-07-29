#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

using namespace std;

class Optional : public Match {
protected:
   Match* _match;
   static const string _defaultValue;
   
public:
   Optional(Match* match)
      : Match()
   {
      _match = match;
   }
   
   virtual ~Optional() {
      if (_match) {
         delete _match;
         _match = NULL;
      }
   }
   
   virtual bool match(int character) {
   
      bool matched =
         _match->match(character);
         
      std::optional<bool>& _success =
         _match->success();
         
      if (_success == true) {
         success() = true;
      } 
      else if (_success == false) {
         success() = true;
      }
      else if (character == Match::eof) {
         success() = true;
      }
      return matched;
   }
   
   virtual const string& value() const {
   
      if (_match->success() == true)
         return _match->value();
      
      return _defaultValue;
   }

   
   virtual void write(ostream& out) const
   {
      out << "Optional(";
      Match::write(out);
      out << *_match;
      out << ")";
   }
   
};

template<class T>
class _Optional : public Optional
{
public:
   _Optional() : Optional(NULL)
   {
      _match = NULL;
   }
   
   virtual ~_Optional() 
   {
      if (_match) {
         delete _match;
         _match = NULL;
      }
   }
   
   virtual bool match(int character)
   {
      if (_match == NULL)
         _match = new T();
         
      return Optional::match(character);
     
   }
   
   virtual const string& value() const {
      if (_match == NULL)
         return _defaultValue;
         
      return _match->value();
   }
 
};

}

#endif
