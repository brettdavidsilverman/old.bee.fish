#ifndef BEE_FISH_WEB__WEB_REQUEST_H
#define BEE_FISH_WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {

   class WebRequest : public And {
   public:

      class NewLine : public Word {
      public:
         NewLine() : Word("\r\n") {

         }
      };
      
      class Header : public Match
      {
      protected:

         class HeaderNameCharacter : public Not {
         public:
            HeaderNameCharacter() : Not(
               new Or(
                  new BeeFishParser::
                     Character(':'),
                  new BlankChar(),
                  new NewLine()
               )
            )
            {

            }
         };

         class HeaderValueCharacter : public Match {
         public:
            HeaderValueCharacter() : Match(
               new Not(
                  new NewLine()
               )
            )
            {

            }
         };

      public:
         BString _name;
         BString _value;
         
      public:
         Header() : Match()
         {
            Match* colon =
               new And(
                  new Optional(
                     new Blanks
                  ),
                  new BeeFishParser::
                     Character(':'),
                  new Optional(
                     new Blanks()
                  )
               );

            Match*
               headerName =
                  new Repeat<HeaderNameCharacter>();
            
            Match*
               headerValue =
                  new Repeat<HeaderValueCharacter>();

            _match = new And(
               new Capture(
                  headerName,
                  this->_name
               ),
               colon,
               new Capture(
                  headerValue,
                  this->_value
               ),
               new NewLine()
            );
            
         }
         
         virtual ~Header()
         {
         }
      };


      class Headers :
         public Repeat<Header>,
         public map<BString, BString>
      {
      public:
        
         Headers() :
            Repeat<Header>()
         {
         }

         virtual ~Headers()
         {
         }
         
         virtual void matchedItem(Header* header)
         {

            BString lowerName =
               header->_name.toLower();
         
            emplace(
               lowerName,
               header->_value
            );

            Repeat::matchedItem(header);
         }
   
         bool contains(const BString& name) const
         {
            return count(name) > 0;
         }
         
         const BString& operator[] (
            const BString& name
         ) const
         {
            static const BString emptyHeader;

            if (contains(name))
               return map<BString, BString>::at(name);
            else
               return emptyHeader;
         }
   
         friend ostream& operator <<
         (ostream& out, Headers& headers)
         {
            for (auto it = headers.begin();
                      it != headers.end();
                    ++it)
            {
               BString header = it->first;
               BString value = it->second;
               out
                  << header
                  << '\t'
                  << value
                  << endl;
            }
      
            return out;
         }
   
   
   
      };


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
/*
         class HexCharacterSequence : public Repeat<HexCharacter> {
         protected:
            BString _value;
         public:
            HexCharacterSequence() : Repeat<HexCharacter>(1, 4)
            {
            }

            virtual void matchedItem(HexCharacter* item) {
#warning "Find way to join surrogate pairs"
               switch (_matchedCount + 1) {
                  case 1:
                     _character = item->character();
                     break;
                  case 2:
                     _character = (_character << 8) + item->character();
                     break;
                  case 3:
                     _character = (_character << 8) + item->character();
                     break;
                  case 4:
                     _character = (_character << 8) + item->character();
                     _value.push_back(_character);
                     break;
               }
               Repeat<HexCharacter>::matchedItem(item);
            }

            virtual const Char& character() const {
               return _character;
            }

            virtual const BString& value() const {
               return _value;
            }

         };
*/
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
               new HexCharacter()
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
               _value.push_back(item->character());
               Repeat<PathCharacter>::matchedItem(item);
            }

            virtual const BString& value() const {
               return _value;
            }
         };

      public:
         Path* _path;
         Path* _query;
      public:
         
         URL() : Match()
         {
           
            
            _path = new Path();
               
               
            Match* query =
               new Optional(
                  new And(
                     new BeeFishParser::
                        Character('?'),
                     _query = new Path()
                  )
               );
               
            _match = new And(_path, query);

         }
         
         const BString& path() const
         {
            return _path->value();
         }
         
         const BString& query() const
         {
            return _query->value();
         }
         
      };
 
      class FirstLine : public Match
      {
      protected:
         class Method : public Or{
         public:
            Method() : Or(
               new Word("GET"),
               new Word("PUT"),
               new Word("POST"),
               new Word("DELETE"),
               new Word("OPTIONS")
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
      public:
         FirstLine() : Match()
         {
            _match = new And(
               new Capture(
                  new Method(),
                  _method
               ),
               new Blanks(),
               _url = new URL(),
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
      
      FirstLine* _firstLine = nullptr;
      Headers*   _headers   = nullptr;
      BeeFishJSON::Object* _json = nullptr;

      public:

      WebRequest() : And()
      {
      }

      virtual void setup(Parser* parser) {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         _json      = nullptr;

         _inputs = {
            _firstLine,
            _headers,
            new NewLine()
         };

         _firstLine->_onsuccess =
            [this, parser](Match* match) {
               if (method() == "POST") {
                  _json = new BeeFishJSON::Object();
                  _json->setup(parser);
                  _inputs.push_back(_json);
               }
            };

         And::setup(parser);
         
      }
    
      virtual ~WebRequest()
      {
      }
      

      virtual bool hasJSON()
      {
         return _json && _json->matched();
      }
   
      virtual BeeFishJSON::Object& json()
      {
         return *(_json);
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

      const URL& url() const
      {
         return _firstLine->url();
      }
      
      const BString& path() const
      {
         return url().path();
      }
      
      const BString& query() const
      {
         return url().query();
      }

      
      const BString& version() const
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
