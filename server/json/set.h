#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace BeeFishJSON
{

   template<class OpenBrace, class Item, class Seperator, class CloseBrace>
   class Set : public Match
   {
   
   protected:
      Match* _match;

   public:
      Set() : Match()
      {
         _match = nullptr;
      }
            
      virtual ~Set()
      {
         if (_match)
            delete _match;
      }
      
      virtual void setup()
      {
         Match* openBrace = new And(
            new OpenBrace(),
            new Optional(
               new BlankSpace()
            )
         );
              
         class _Seperator : public And {
         public:
            _Seperator() : And(
               new Optional(
                  new BlankSpace()
               ),
               new Seperator(),
               new Optional(
                  new BlankSpace()
               )
            )
            {

            }
         };

         Match* closeBrace = new And(
            new Optional(
               new BlankSpace()
            ),
            new CloseBrace()
         );

         class InvokeItem : public Invoke {
      
         public:
            InvokeItem(Set* set) : Invoke(
               new Item(),
               [set](Match* match) {
                  set->matchedSetItem((Item*)match);
               }
            ) 
            {
            }
         };

         class SubsequentItem : public And {
         public:
            
            SubsequentItem() : And() {
               throw std::runtime_error("SubsequentItem construted without a set");
            }

            SubsequentItem(Set* set) : And(
               new _Seperator(), 
               new InvokeItem(set)
            )
            {

            }
         };

         class SubsequentItems : public Repeat<SubsequentItem> {
         protected:
            Set* _set;
         public:
            SubsequentItems() : Repeat<SubsequentItem>(0) {
               throw std::runtime_error("SubsequentItems constructed without set");
            }

            SubsequentItems(Set* set) : Repeat<SubsequentItem>(0) 
            {
               _set = set;
            }

            virtual ~SubsequentItems() {
            }

            virtual SubsequentItem* createItem() {
               return new SubsequentItem(_set);
            }
         };

         _match =
            new And(
               openBrace,
               new Optional(
                  new And(
                     new InvokeItem(this),
                     new SubsequentItems(this)
                  )
               ),
               closeBrace
            );

         Match::setup();

      }
      
             
      virtual void matchedSetItem(Item* item)
      {
      }
      
      virtual const BString& value() const
      {
         return _match->value();
      }

      virtual bool match(const Char& character) {
         bool matched = _match->matchCharacter(character);
         if (_match->_result == true)
            success();
         else if (_match->_result == false)
            fail();

         return matched;
      }
      
   };
         
}

#endif

