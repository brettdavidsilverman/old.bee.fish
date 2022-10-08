#ifndef BEE_FISH_WEB__WEB_REQUEST_H
#define BEE_FISH_WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"
#include "headers.h"
#include "content-length.h"
#include "body.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {


   class WebRequest : public And {
   public:

      class URL : public Match
      {
      public:

         class Hex : public Capture {
         public:
            Hex() : Capture(
               new Or (
                  new Range('a', 'f'),
                  new Range('A', 'F'),
                  new Range('0', '9')
               )
            )
            {

            }
            
         };

         class HexCharacter : public And {
         protected:
            Hex* _hex1;
            Hex* _hex2;
         public:
            HexCharacter() : And(
               new BeeFishParser::Character('%'),
               _hex1 = new Hex(),
               _hex2 = new Hex()
            )
            {

            }

            virtual void success() {

               std::stringstream stream;
               stream << std::hex;
               stream << (uint8_t)_hex1->character() << (uint8_t)_hex2->character();

               uint32_t u32;
               stream >> u32;
               _character = Char(u32);
            }


         };

         class UnicodeCharacter : public And {
         protected:
            Hex* _hex1;
            Hex* _hex2;
            Hex* _hex3;
            Hex* _hex4;

         public:
            UnicodeCharacter() : And(
               new BeeFishParser::Character('%'),
               new BeeFishParser::Character('u'),
               _hex1 = new Hex(),
               _hex2 = new Hex(),
               _hex3 = new Hex(),
               _hex4 = new Hex()
            )
            {

            }

            virtual void success() {

               std::stringstream stream;
               stream << std::hex;
               stream 
                  << (uint8_t)_hex1->character()
                  << (uint8_t)_hex2->character()
                  << (uint8_t)_hex3->character()
                  << (uint8_t)_hex4->character();

               uint32_t u32;
               stream >> u32;
               _character = Char(u32);
            }

         };

         class PathCharacter : public Or {
         public:
            PathCharacter() : Or (
               new Range('0', '9'),
               new Range('a', 'z'),
               new Range('A', 'Z'),
               new BeeFishParser::Character('+'),
               new BeeFishParser::Character('.'),
               new BeeFishParser::Character('='),
               new BeeFishParser::Character('&'),
               new BeeFishParser::Character('-'),
               new BeeFishParser::Character('_'),
               new BeeFishParser::Character('/'),
               new HexCharacter(),
               new UnicodeCharacter()
            )
            {

            }

            virtual const Char& character() const {
               return item().character();
            }


         };


         class Path : public Repeat<PathCharacter> {
         protected:
            BString _value;

         public:
            Path() : Repeat() {

            }

            virtual void matchedItem(PathCharacter* item) {
               _value.push_back((uint32_t)item->character());
               Repeat<PathCharacter>::matchedItem(item);
            }

            virtual BString value(){
               return _value;
            }

            virtual BString value() const {
               return _value;
            }
         };

         class Query : 
            public Path,
            public std::map<BString, BString>
         {
         public:
            Query() : Path() {

            }

            virtual bool contains(const BString& key) {
               return count(key) > 0;
            }
            
            virtual void success() {
               Path::success();
               vector<BString> keyValuePairs = _value.split('&');
               for (const BString& pair : keyValuePairs) {
                  BString equals("=");
                  
                  auto itEquals = std::find_first_of(
                     pair.begin(), pair.end(),
                     equals.begin(), equals.end()
                  );

                  if (itEquals != pair.end()) {
                     size_t posEquals = itEquals - pair.begin();
                     BString key = pair.substr(0, posEquals);
                     BString value = pair.substr(posEquals + 1);
                     emplace(key, value);
                  }
               }
            }
            
         };       

      public:
         Path* _path;
         Query* _query;
      public:
         
         URL() : Match()
         {
           
            
            _path = new Path();
               
               
            Match* query =
               new Optional(
                  new And(
                     new BeeFishParser::
                        Character('?'),
                     _query = new Query()
                  )
               );
               
            _match = new And(_path, query);

         }
         
         const BString path() const
         {
            return _path->value();
         }
         
         const Query& query() const
         {
            return (*_query);
         }
         
         Query& query()
         {
            return (*_query);
         }

      };
 
      class FirstLine : public Match
      {
      protected:
         class Method : public Or{
         public:
            Method() : Or(
               new Word("GET"),
//               new Word("PUT"),
               new Word("POST"),
               new Word("DELETE")
//               new Word("OPTIONS")
            )
            {

            }

         };

         class Version : public And {
         public:
            Version() : And(
               new Word("HTTP/1."),
               new Range('0', '9')
            )
            {

            }

         };

      public:
         BString _method;
         URL*    _url;
         BString _version;
         BString _fullURL;
      public:
         FirstLine() : Match()
         {
            _match = new And(
               new Capture(
                  new Method(),
                  _method
               ),
               new Blanks(),
               new Capture(
                  _url = new URL(),
                  _fullURL
               ),
               new Blanks(),
               new Capture(
                  new Version(),
                  _version
               ),
               new NewLine()
            );

         }
         
         virtual ~FirstLine()
         {
         }
         
         const URL& url() const {
            return *_url;
         }
      
         URL& url() {
            return *_url;
         }

         const BString& fullURL() const {
            return _fullURL;
         }

      };

   /*
      class ObjectPath :
         public _Object,
         protected Path<PowerEncoding>
      {
      
      public:
         ObjectPath(const Path& start) :
            _Object(),
            Path(start)
         {
         }
         
         ObjectPath(const ObjectPath& source) :
            _Object(),
            Path(source)
         {
         }
         
         virtual Match* copy() const
         {
            return new ObjectPath(*this);
         }
         
            
         virtual _JSON* operator[] (const BString& key)
         {
            Path next = Path::operator [] (key);
            _JSON* json = _Object::operator[] (key);
            return json;
         }
         
         virtual _JSON* operator[] (const char* key)
         {
            _JSON* json = ObjectPath::operator[] (BString(key));
            return json;
         }
         
         virtual bool contains(const BString& key)
         {
            bool value = _Object::contains(key);
            return value;
         }
         
         virtual bool contains(const char* key)
         {
            bool value = ObjectPath::contains(BString(key));
            return value;
         }
         
         void emplace(
            const BString& key,
            _JSON* value
         )
         {
            Path::operator [] (key);
            _Object::emplace(key, value);
         }
         
      protected:
         
      };
      */
      
      FirstLine*           _firstLine = nullptr;
      BeeFishWeb::Headers* _headers   = nullptr;
      Body*                _body = nullptr;
      size_t               _contentLength = 0;
      BStream::OnBuffer    _ondata = nullptr;

   public:

      WebRequest() : And()
      {
      }

      virtual void setup(Parser* parser) {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         _body      = nullptr;

         _inputs = {
            _firstLine,
            _headers,
            new NewLine()
         };

         _firstLine->setup(parser);
         _headers->setup(parser);

         _headers->_onsuccess = 
            [this, parser](Match* match) {
               if (  method() == "POST") {
                  // Currently we only handle json or image/jpeg
                  _body = new Body();
                  _body->setup(parser, _headers);
                  _body->setOnData(_ondata);
                  _inputs.push_back(_body);
               }

            };

         And::setup(parser);
         
      }

      virtual void flush() {
         if (_body) {
            _body->flush();
         }
      }
    
      virtual ~WebRequest()
      {
      }
      

      virtual bool hasJSON()
      {
         return _body && _body->hasJSON();
      }
   
      virtual BeeFishJSON::Object& json()
      {
         return *(_body->_json);
      }

      virtual BStream& body(){
         if (_body && _body->_contentLength)
            return *(_body->_contentLength);
         else
            throw std::runtime_error("ContentLength body not setup");
      }

      void setOnData(BStream::OnBuffer ondata) {
         _ondata = ondata;
      }

      
      Headers& headers()
      {
         return *_headers;
      }
      
      const Headers& headers() const
      {
         return *_headers;
      }
      
      const BString& method() const
      {
         return _firstLine->_method;
      }
      
      const FirstLine& firstLine() const
      {
         return *_firstLine;
      }

      FirstLine& firstLine()
      {
         return *_firstLine;
      }

      const URL& url() const
      {
         return _firstLine->url();
      }
      
      URL& url()
      {
         return _firstLine->url();
      }

      BString fullURL() const
      {
         return _firstLine->fullURL();
      }

      BString path() const
      {
         return url().path();
      }
      
      const URL::Query& queryObject() const
      {
         return url().query();
      }

      URL::Query& queryObject()
      {
         return url().query();
      }

      BString query() const {
         return url().query().value();
      }
      
      BString version() const
      {
         return _firstLine->_version;
      }
      
      BString getCookie(
         const BString& cookieName
      ) const
      {
         const Headers& headers =
            this->headers();
      
         if (headers.contains("cookie"))
         {
            const BString& cookieHeader = headers["cookie"];
            vector<BString> cookies = cookieHeader.split(';');
            for (BString cookie : cookies)
            {
               vector<BString> pair =
                  cookie.trim().split('=');
            
               if ( pair.size() == 2 && 
                    pair[0].trim() == cookieName )
               {
                  return (pair[1].trim());

               }
            }
         }
         
         return "";
      }

   };
   

};

#endif
