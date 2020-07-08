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
      if (inputs().size() == 0) {
         setSuccess(false);
      }
      
   }

   virtual bool match(char character) {
      
      optional<bool> success;
      bool matched;
      
      vector<Match*>& inputs =
         Match::inputs();
      
      do {
         Match* item =
            inputs[_index];

         matched =
            item->match(character);
    
         success = item->success();
            
         if (success == true) {
            _matches.push_back(item);
         
            if (++_index  ==
                inputs.size())
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
   
   virtual void readEnd() {
      
      vector<Match*>& inputs =
         Match::inputs();
      
      while(_index < inputs.size()) {
            
         Match* item =
            inputs[_index];

         item->readEnd();
         
         optional<bool> success =
            item->success();
            
         if (success == true) {
            _matches.push_back(item);
            ++_index;
         }
         else {
            if (success == false)
               setSuccess(false);
            break;
         }
         
      }
      
      if (success() == nullopt &&
          _index == inputs.size())
         setSuccess(true);
      
   }
   
   virtual const vector<Match*>& items() const {
      return _matches;
   }
   
   virtual const string value() const {
      return Match::word(items());
   }
};

};
