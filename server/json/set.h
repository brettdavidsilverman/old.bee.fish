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
      Match* _openBrace;
      Match* _item;
      Match* _seperator;
      Match* _closeBrace;
   public:
       
      
      Set( Match* openBrace,
           Match* item,
           Match* seperator,
           Match* closeBrace
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
      
      virtual ~Set()
      {
         delete _openBrace;
         delete _item;
         delete _seperator;
         delete _closeBrace;
      }
      
      virtual void setup()
      {

         And OpenBrace = And(
            _openBrace->copy(),
            new Optional(BlankSpace.copy())
         );
              
         And Seperator = And(
            new Optional(BlankSpace.copy()),
            _seperator->copy(),
            new Optional(BlankSpace.copy())
         );
               
         And CloseBrace = And(
            new Optional(BlankSpace.copy()),
            _closeBrace->copy()
         );
                
         Invoke Item = Invoke(
            _item->copy(),
            [this](Match* item)
            {
               this->matchedSetItem(item);
            }
         );
            
         Match* _item1 = Item.copy();
         Match* _item2 = Item.copy();
         
         Match* _set = new And(
            OpenBrace.copy(),
            new Optional(
               new And(
                  _item1,
                  new Repeat(
                     new And(
                        Seperator.copy(),
                        _item2
                     ),
                     0
                  )
               )
            ),
            CloseBrace.copy()
         );
            
         _match = _set;
         
         Invoke::setup();
      }
      
             
      virtual Match* copy() const
      {
         return new Set(*this);
      }
           

      virtual void matchedSetItem(Match* item)
      {
         
      }
      
         
   };
         
}

#endif

