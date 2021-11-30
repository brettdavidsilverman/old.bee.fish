#ifndef BEE_FISH__REQUEST_H
#define BEE_FISH__REQUEST_H

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishDatabase;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishHTTPS {

   class Request : public And {
   public:
   
      class BlankChar : public Or
      {
      public:
         BlankChar() : Or(
            new BeeFishParser::
               Character(' '),
               
            new BeeFishParser::
               Character('\t')
         )
         {
         }

      };

      class Blanks : public Repeat<BlankChar>
      {
      public:
         Blanks() : Repeat()
         {
         }

      };
      
      class NewLine : public Match
      {
      public:
         NewLine() : Match(
            new Or(
               new And (
                  new BeeFishParser::
                     Character('\r'),
                  new Optional(
                     new BeeFishParser::
                        Character('\n')
                  )
               ),
               new BeeFishParser::Character('\n')
            )
         )
         {
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
            return map<BString, BString>::
               at(name);
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

         class SimpleCharacter : public Match {
         public:
            SimpleCharacter() : Match(
               new Not(
                  new Or (
                     new BlankChar,
                     new BeeFishParser::
                        Character('\r'),
                     new BeeFishParser::
                        Character('\n') ,
                     new BeeFishParser::
                        Character('?'),
                     new BeeFishParser::
                        Character('%')
                  )
               )
            )
            {

            }
         };

         class HexCharacter : public Capture {
         public:
            HexCharacter() : Capture(
               new Or (
                  new Range('a', 'f'),
                  new Range('A', 'F'),
                  new Range('0', '9')
               )
            )
            {

            }
         };

         class EscapedCharacter : public And {
         protected:
            HexCharacter* _hex1;
            HexCharacter* _hex2;
            Match*        _escape;
            Char         _character;
         public:
            EscapedCharacter() : And(
               new BeeFishParser::
                  Character('%'),
               new Or(
                  new And(
                     _hex1 = new HexCharacter(),
                     _hex2 = new HexCharacter()
                  ),
                  _escape = new BeeFishParser::Character('%')
               )
            )
            {

            }
         
            virtual void success() {
               if (_escape->matched())
                  _character = '%';
               else {
                  std::stringstream stream;
                  stream << std::hex << _hex1->value() << _hex2->value();
                  uint32_t u32;
                  stream >> u32;
                  _character = Char(u32);
               }
            }

            virtual const Char& character() const {
               return _character;
            }
         };
         
         class PathCharacter : public Or {
         public:
            PathCharacter() : Or(
               new SimpleCharacter(),
               new EscapedCharacter()
            )
            {

            }

            virtual const Char& character() const {
               return item().character();
            }

         };

         template <class T>
         class String : public Repeat<T> {
         protected:
            BString _value;

         public:
            String() : Repeat<T>() {

            }

            virtual void matchedItem(T* item) {
               _value.push_back(item->character());
               Repeat<T>::matchedItem(item);
            }

            virtual const BString& value() const {
               return _value;
            }
         };

      public:
         String<PathCharacter>* _path;
         String<PathCharacter>* _query;
      public:
         
         URL() : Match()
         {
           
            
            _path = new String<PathCharacter>();
               
               
            Match* query =
               new Optional(
                  new And(
                     new BeeFishParser::
                        Character('?'),
                     _query = new String<PathCharacter>()                  )
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
      Optional*  _body      = nullptr;
      JSON*      _json      = nullptr;

      public:

      Request(map<BString, ObjectFunction> objectFunctions = {}) : And()
      {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         
         _inputs = {
            _firstLine,
            _headers,
            new NewLine(),
            _body = new Optional(
               _json = new JSON(objectFunctions)
            )
         };

      }
    
      virtual ~Request()
      {
      }
      
      map<BString, ObjectFunction>& objectFunctions() {
         return _json->objectFunctions();
      }

      virtual bool hasBody()
      {
         return _body->matched();
      }
      
      virtual bool hasJSON()
      {
         return _json->matched();
      }
   
      virtual JSON& json()
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
