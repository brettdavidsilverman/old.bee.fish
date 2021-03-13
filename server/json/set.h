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

   class Set :
      public Invoke
   {
   public:
      MatchPtr _openBrace;
      MatchPtr _item;
      MatchPtr _seperator;
      MatchPtr _closeBrace;
   public:
       
      
      Set( MatchPtr openBrace,
           MatchPtr item,
           MatchPtr seperator,
           MatchPtr closeBrace
      ) : Invoke(),
         _openBrace(openBrace),
         _item(item),
         _seperator(seperator),
         _closeBrace(closeBrace)
      {
         
           
      }
        
      Set(const Set& source) :
         _openBrace(source._openBrace->copy()),
         _item(source._item->copy()),
         _seperator(source._seperator->copy()),
         _closeBrace(source._closeBrace->copy())
      {
      }
      
      virtual void setup()
      {

         MatchPtr OpenBrace =
            _openBrace and ~BlankSpace;
              
         MatchPtr Seperator =
            ~BlankSpace and
            _seperator and
            ~BlankSpace;
               
         MatchPtr CloseBrace =
            ~BlankSpace and _closeBrace;
                

         MatchPtr Item = make_shared<Invoke>(
            _item,
            [this](MatchPtr item)
            {
               this->matchedSetItem(item);
            }
         );
            
         MatchPtr Item1 = Item->copy();
         MatchPtr Item2 = Item->copy();
         
         MatchPtr _Set =
            OpenBrace and
            ~(
               Item1 and
               Repeat(
                  Seperator and
                  Item2,
                  0
               )
            ) and
            CloseBrace;
            
         setMatch(_Set);
         
         Invoke::setup();
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

