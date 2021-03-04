#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include <optional>
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace bee::fish::json
{

   class Set : public Match
   {
   public:
      Set( MatchPtr openBrace,
           MatchPtr item,
           MatchPtr seperator,
           MatchPtr closeBrace
      ) : Match()
      {
         MatchPtr OpenBrace =
            openBrace and ~BlankSpace;
         
         MatchPtr Seperator =
            ~BlankSpace and
            seperator and
            ~BlankSpace;
         
         MatchPtr CloseBrace =
            ~BlankSpace and closeBrace;
         
         MatchPtr Item = new
            Invoke(
               item,
               [this](MatchPtr match)
               {
                  cerr << *match << &Set::matchedSetItem;
                  std::invoke(&Set::matchedSetItem, this, match);
               }
            );
            
         MatchPtr Set =
            OpenBrace and
            Optional(
               Item and
               Repeat(Seperator and Item, 0)
            ) and
            CloseBrace;
         
         setMatch(Set);
      }
      
      void setup()
      {
         MatchPtr Item = new
            Invoke(
               new Word("hello"),
               [this](MatchPtr match)
               {
                  cerr << *match << &Set::matchedSetItem;
                  std::invoke(&Set::matchedSetItem, this, match);
               }
            );
            
         MatchPtr Set =
            Character('}') and
            Optional(
               Item and
               Repeat(Character(',') and Item, 0)
            ) and
            Character('}');
         
         setMatch(Set);
         cerr << *Set << endl;
      }

      void callMatched(MatchPtr matched)
      {
         cerr << *this;
         this->matchedSetItem(matched);
      }
      
      Set(const Set& source) : Match(source)
      {
         
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<Set>(*this);
      }
      

      virtual void matchedSetItem(MatchPtr item)
      {
      }
      
   };
      
}

#endif
