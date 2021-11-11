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
         
         for (Match* record : _records)
         {
            delete record;
         }
         
      }
      
      virtual void setup()
      {
         MatchPointer<And> OpenBrace =
            *_openBrace and
            ~BlankSpace;
              
         MatchPointer<And> Seperator = 
            ~BlankSpace and
            *_seperator and
            ~BlankSpace;
               
         MatchPointer<And> CloseBrace =
            ~BlankSpace and
            *_closeBrace;
                
         Invoke Item = Invoke(
            _item->copy(),
            [this](Match* item)
            {
               this->matchedSetItem(item);
            }
         );

         
         class SubsequentItems :
            public Repeat
         {
         protected:
            Set* _set;
         public:
            SubsequentItems(Set* set, Match* seperated) :
               Repeat(seperated),
               _set(set)
            {
            }
            
            SubsequentItems(const SubsequentItems& source) :
               Repeat(source),
               _set(source._set)
            {
            }
            
            virtual void matchedItem(Match* item)
            {
               if (_set->_capture)
                  _set->_records.push_back(item);
               else
                  delete item;
            }
            
            virtual Match* copy() const
            {
               return new SubsequentItems(*this);
            }
         };
         
         SubsequentItems subsequentItems(
            this,
            Seperator and Item
         );
         
         MatchPointer set =
            OpenBrace and
            ~(
               Item and
               subsequentItems
             ) and
             CloseBrace;

         _match = set;
         
         _setup = true;
         
      }
      
             
      virtual Match* copy() const
      {
         return new Set(*this);
      }
           

      virtual void matchedSetItem(Match* item)
      {
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

