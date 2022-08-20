#ifndef BEE_FISH__JSON_OUT_STREAM_H
#define BEE_FISH__JSON_OUT_STREAM_H

#include <stack>
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"
#include "b-script-parser.h"

   
namespace BeeFishBScript {

   using namespace BeeFishBString;
   
   
     
   class JSONOutStream
   {
   private:
      typedef std::map<size_t, BString> Keys;

      struct StackItem {
         Path _path;
         ObjectPointer _object;
         Keys _keys;

         StackItem(Path path, ObjectPointer object) :
            _path(path),
            _object(object),
            _keys()
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
      
      JSONOutStream& operator >> (
         Variable& variable
      )
      {
         variable = this->read();
         return *this;
      }
      
      
      virtual Variable read() {
         BeeFishJSON::Type type;
         float temp;
         _path >> temp;
         type = (BeeFishJSON::Type)temp;
         cerr << "Read type:" << type << endl;
         switch (type) {
            case UNDEFINED:
               return undefined;
               break;
            case __NULL:
               return nullptr;
               break;
            case BOOLEAN: {
               bool value;
               _path.getData(value);
               return value;
            }
            case NUMBER: {
               double value;
               _path.getData(value);
               return value;
            }
            case STRING: {
               BeeFishBString::BString value;
               _path.getData(value);
               return value;
            }
            case ARRAY:
               throw std::logic_error("ARRAY not implemented yet");
               break;
            case OBJECT: {
               Keys keys;
               ObjectPointer object = std::make_shared<Object>();
               _stack.push(
                  StackItem(_path, object)
               );
               _path = _path['K'];
               break;
            }
            default:
               throw std::logic_error("Invalid json type");
         }
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