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

   class Set :
      public Match
   {
   
   public:
      
      Match* _openBrace;
      Match* _item;
      Match* _seperator;
      Match* _closeBrace;
      
      bool   _capture;
      
      vector<Match*> _records;
      
   public:
      Set() : Match(),
         _openBrace(nullptr),
         _item(nullptr),
         _seperator(nullptr),
         _closeBrace(nullptr),
         _capture(false)
      {
      }
      
      Set(
           Match* openBrace,
           Match* item,
           Match* seperator,
           Match* closeBrace,
           bool   capture
      ) : Match(),
         _openBrace(openBrace),
         _item(item),
         _seperator(seperator),
         _closeBrace(closeBrace),
         _capture(capture)
      {
         
           
      }
        
      Set(const Set& source) :
         Match(),
         _openBrace(source._openBrace->copy()),
         _item(source._item->copy()),
         _seperator(source._seperator->copy()),
         _closeBrace(source._closeBrace->copy()),
         _capture(source._capture)
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
         And* openBrace = new And(
            _openBrace->copy(),
            new Optional(
               BlankSpace.copy()
            )
         );
              
         And* seperator = new And(
            new Optional(
               BlankSpace.copy()
             ),
            _seperator->copy(),
            new Optional(
               BlankSpace.copy()
            )
         );
               
         And* closeBrace = new And(
            new Optional(
               BlankSpace.copy()
            ),
            _closeBrace->copy()
         );
                
         Invoke* item = new Invoke(
            _item->copy(),
            [this](Match* item)
            {
               this->matchedSetItem(item);
            }
         );

         class SubsequentItems : public Repeat {
         private:
            vector<Match*> _items;
            bool _capture;
         public:
            SubsequentItems(Match* seperatedItem, bool capture = true) : Repeat(
               seperatedItem
            ),
               _capture(capture)
            {

            }

            ~SubsequentItems() {
               for (auto it : _items)
               {
                  Match* match = it;
                  delete match;
               }   
            }

            virtual void matchedItem(Match *match) {
               if (_capture)
                  _items.push_back(match);
               else
                  Repeat::matchedItem(match);
            }


         };

         _match =
            new And(
               openBrace,
               new Optional(
                  new And(
                     item->copy(),
                     new SubsequentItems(
                        new And(seperator, item->copy()),
                        _capture
                     ) 
                  )
               ),
               closeBrace
            );

         delete item;

         _setup = true;
         
      }
      
             
      virtual Match* copy() const
      {
         return new Set(*this);
      }
           

      virtual void matchedSetItem(Match* item)
      {
         if (_capture)
            _records.push_back(item);
      }
      
      virtual const vector<BeeFishBString::Character>& value() const
      {
         return _match->value();
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "Set";
         writeResult(out);
         out << endl
             << tabs(tabIndex)
             << "("
             << endl
             << tabs(tabIndex + 1)
             << _capture << ","
             << endl;
         _openBrace->write(out, tabIndex + 1);
         out << ","
             << endl;
         _item->write(out, tabIndex + 1);
         out << ","
             << endl;
         _seperator->write(out, tabIndex + 1);
         out << ","
             << endl;
         _closeBrace->write(out, tabIndex + 1);
         out << endl
             << tabs(tabIndex)
             << ")";
      }
   
   };
         
}

#endif

