#include "match.h"
#include <vector>

using namespace std;

namespace Bee::Fish::Parser {

class And : public Match {
private:
   vector<Match>::iterator _index;

public:

   template<typename... T>
   And(const T&... input) :
      Match{ input... }
   {
      cout << "And::And("
           << _inputs.size() 
           << ")"
           << endl;
      
      if (_inputs.size() == 0) {
         setSuccess(true);
      }
      else
         _index = _inputs.begin();
   }

   virtual bool match(int character) {
      
      optional<bool> success;
      bool matched;
      
      Match& item = *_index;

      matched =
         item.match(character);
    
      success = item.success();
            
      if (success == true) {
         if (++_index  == _inputs.end())
            setSuccess(true);
      }
      else if (success == false)
         setSuccess(false);
         
      return matched;
   }
   
   virtual const vector<Match>& items() const {
      return _inputs;
   }
   
};

};
