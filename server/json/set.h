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
      public Match
   {
   
   public:
      bool   _capture;
      Match* _openBrace;
      Match* _item;
      Match* _seperator;
      Match* _closeBrace;
      vector<Match*> _records;
   public:
      
      Set( bool   capture,
           Match* openBrace,
           Match* item,
           Match* seperator,
           Match* closeBrace
      ) : Match(),
         _capture(capture),
         _openBrace(openBrace),
         _item(item),
         _seperator(seperator),
         _closeBrace(closeBrace)
      {
         
           
      }
        
      Set(const Set& source) :
         _capture(source._capture),
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
         
         for (Match* record : _records)
         {
         }
      }
      
      virtual void setup()
      {

         MatchPointer OpenBrace =
            *_openBrace and
            ~BlankSpace;
              
         MatchPointer Seperator = 
            ~BlankSpace and
            *_seperator and
            ~BlankSpace;
               
         MatchPointer CloseBrace =
            ~BlankSpace and
            *_closeBrace;
                
         Invoke Item = Invoke(
            _item->copy(),
            [this](Match* item)
            {
               this->matchedSetItem(item);
            }
         );

         Repeat subsequentItems(
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
         cerr << *item << endl;
         if (_capture)
            _records.push_back(item);
         else
            delete item;
      }
      
      virtual const BString& value() const
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

