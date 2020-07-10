#include "match.h"
#include <vector>

using namespace std;

namespace Bee::Fish::Parser {

class And : public Match {
private:
   size_t _index = 0;
   vector<Match*> _matches;
public:

   template<typename... T>
   And(T*... input) :
      Match{ input... }
   {
      if (_inputs.size() == 0) {
         setSuccess(false);
      }
      
   }

   virtual bool match(int character) {
      
      optional<bool> success;
      bool matched;
      
      do {
         Match* item =
            _inputs[_index];

         matched =
            item->match(character);
    
         success = item->success();
            
         if (success == true) {
            _matches.push_back(item);
         
            if (++_index  ==
                _inputs.size())
            {
               setSuccess(true);
               break;
            }
         }
         else if (success
                  == false)
         {
            setSuccess(false);
         }
         
      } while(success == true
              && !matched);
         
      return matched;
   }
   
   virtual const vector<Match*>& items() const {
      return _matches;
   }
   
   virtual const string value() const {
      return Match::word(items());
   }
};

};
