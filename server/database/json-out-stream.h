#ifndef BEE_FISH__JSON_IN_STREAM_H
#define BEE_FISH__JSON_IN_STREAM_H

#include <stack>
#include "json-parser.h"
#include "json.h"
#include "../database/path.h"

   
namespace BeeFishJSON {

   using namespace BeeFishBString;
   typedef BeeFishDatabase::Path<BeeFishPowerEncoding::PowerEncoding> Path;
   
   
     
   class JSONInStream :
      public JSONParser
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
      JSON _json;
      Path _path;
      std::stack<StackItem> _stack;
   public:
      JSONInStream(
         Path& path
      ) :
         JSONParser(_json),
         _path(path)
      {
      }
      
      friend
      JSONInStream& operator << (
         JSONInStream& stream,
         const BString& json
      )
      {
         stream.read(json);
         return stream;
      }
      
      friend
      JSONInStream& operator << (
         JSONInStream& stream,
         const char* json
      )
      {
         stream << BString(json);
         return stream;

      }
      
      virtual void onbeginobject(Match* match)
      {
         cout << "Begin Object" << endl;
         _stack.push(StackItem(_path));
         _path <<
            (float)BeeFishJSON::Type::OBJECT;
      }

      virtual void onendobject(Match* match) {
         cout << "End Object" << endl;
         _path = _stack.top()._path;
         _stack.pop();
      }

      virtual void onbeginarray(Match* match) {
      }

      virtual void onarrayvalue(Match* match) {
      }

      virtual void onendarray(Match* match) {
      }

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
      
      

     
   };
   
   
}

#endif