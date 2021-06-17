#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../power-encoding/power-encoding.h"
#include "../database/path.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace bee::fish::parser;
using namespace bee::fish::database;
using namespace bee::fish::power_encoding;

namespace bee::fish::json {
   
   extern const Label JSON;
  
   class _Array : public Set
   {
   public:
      size_t _size = 0;
      Path<PowerEncoding>* _path = nullptr;
      
   public:
      _Array(Path<PowerEncoding>* path = nullptr) 
      : Set()
      {
         if (path)
            _path = new Path(*path);
      }
      
      _Array(const _Array& source) :
         Set(source)
      {
         if (source._path)
            _path = new Path(*(source._path));
      }
      
      virtual ~_Array()
      {
         if (_path)
            delete _path;
            
         _path = nullptr;
      }
      
      virtual Match* copy() const
      {
         return new _Array(*this);
      }
      
      // Implemented in json.h
      virtual void setup();
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "Array";
         writeResult(out);
         out << endl
             << tabs(tabIndex)
             << "("
             << endl
             << tabs(tabIndex + 1)
             << _size
             << ","
             << endl;
         _match->write(out, tabIndex + 1);
         out << endl
             << tabs(tabIndex)
             << ")";
      }
      
      virtual void matchedSetItem(Match* item)
      {
         ++_size;
         
         if (_path)
            cerr << item->value() << endl;
            
         Set::matchedSetItem(item);
      }
   };
   
   const Label Array = Label(
      "Array",
      new _Array()
   );
      
 
}

#endif