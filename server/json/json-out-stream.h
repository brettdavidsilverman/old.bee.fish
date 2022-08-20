#ifndef BEE_FISH__JSON_OUT_STREAM_H
#define BEE_FISH__JSON_OUT_STREAM_H

#include <stack>
#include "json-parser.h"
#include "json.h"
#include "../database/path.h"

   
namespace BeeFishJSON {

   using namespace BeeFishBString;
   typedef BeeFishDatabase::Path<BeeFishPowerEncoding::PowerEncoding> Path;
   
   
     
   class JSONOutStream
   {
   private:
      struct StackItem {
         Path _path;
         size_t _nextIndex;
         StackItem(Path path) :
            _path(path),
            _nextIndex(0)
         {
         }
      };
      
   protected:
      Path _path;
      std::stack<StackItem> _stack;
   public:
      JSONOutStream(
         Path& path
      ) :
         _path(path)
      {
      }
      
      friend
      JSONOutStream& operator << (
         JSONOutStream& stream,
         const BString& json
      )
      {
         json = stream.read();
         return stream;
      }
      
      
      virtual BString read() {
         BeeFishJSON::Type type;
         _path >> type;
         cerr << "Read type:" << type << endl;
         
         return "";
      }
      /*
      virtual void onobjectvalue(const BString& key, const JSON* value) {
         cout << "Object Value" << endl;
         _path = _stack.top()._path;
         Path bookmark = _path;
         
         _path << 'K' << key <<
            _stack.top()._nextIndex;
            
         _path = bookmark;
         _path << 'V' <<
            _stack.top()._nextIndex++;
      }

      virtual void onvalue(JSON* json) {
         cout << "Value" << endl;
         BeeFishJSON::Type type = json->type();
         _path << (float)type;
         switch (type) {
            case UNDEFINED:
               break;
            case __NULL:
               break;
            case BOOLEAN:
               _path.setData(
                  json->value() == "true"
               );
               break;
            case NUMBER:
               _path.setData(
                  atof(json->value().c_str())
               );
               break;
            case STRING:
               _path.setData(json->value());
               break;
            case ARRAY:
               throw std::logic_error("ARRAY not implemented yet");
               break;
            case OBJECT:
               cout << "Object" << endl;
               break;
            default:
               throw std::logic_error("Invalid json");
            
         }
      }
      
      */

     
   };
   
   
}

#endif