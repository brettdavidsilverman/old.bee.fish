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

   class Request : public Match {
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

         virtual Match* copy() const {
            return new BlankChar();
         }

      };

      class Blanks : public Repeat
      {
      public:
         Blanks() : Repeat(
            new BlankChar()
         )
         {
         }

         virtual Match* copy() const {
            return new Blanks();
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
   
         virtual Match* copy() const {
            return new NewLine();
         }

         virtual void write(
            ostream& out,
            size_t tabIndex = 0
         ) const
         {
            out << tabs(tabIndex) 
                << "NewLine";
            writeResult(out);
            out << "()";
         }
         
      };

      class Header : public Match
      {
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
               headerNameCharacter =
                  new Not (
                     new Or(
                        new BeeFishParser::
                           Character(':'),
                        new BlankChar(),
                        new NewLine()
                     )
                  );

            
            Match*
               headerName =
                  new Repeat(
                     headerNameCharacter->copy()
                  );
            
            Match*
               headerValueCharacter =
                  new Not(
                     new NewLine()
                  );

            Match*
               headerValue =
                  new Repeat(
                     headerValueCharacter->copy()
                 );

            _match = new And(
               new Capture(
                  headerName->copy(),
                  this->_name
               ),
               colon->copy(),
               new Capture(
                  headerValue->copy(),
                  this->_value
               ),
               new NewLine()
            );

            delete colon;
            delete headerNameCharacter;
            delete headerName;
            delete headerValueCharacter;
            delete headerValue;
            
         }
         
         virtual ~Header()
         {
         }
         
         virtual void write(
            ostream& out,
            size_t tabIndex = 0
         ) const
         {
            out << tabs(tabIndex) 
                << "Header";
            writeResult(out);
            if (_result == true)
            {
               out << "(\"";
               _name.writeEscaped(out);
               out << "\", \"";
               _value.writeEscaped(out);
               out << "\")";
            }
            else
               out << "()";
         }
         
         virtual Match* copy() const
         {
            return new Header();
         }
      
      };


      class Headers :
         public Repeat,
         public map<BString, BString>
      {
      public:
        
         Headers() :
            Repeat(new Header())
         {
         }

         virtual ~Headers()
         {
         }
         
         virtual void matchedItem(Match* match)
         {

            Header* header =
               static_cast<Header*>(match);
    
            BString lowerName =
               header->_name.toLower();
         
            emplace(
               lowerName,
               header->_value
            );

            Repeat::matchedItem(match);
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
         Capture* _path;
         Capture* _query;
      public:
         
         URL() : Match()
         {
           
            Match* simpleCharacter =
               new Not(
                  new Or (
                     new BlankChar,
                     new BeeFishParser::
                        Character('\r'),
                     new BeeFishParser::
                        Character('\n') ,
                     new BeeFishParser::
                        Character('?')
                  )
               );
            
            Match* hexCharacter =
               new Or (
                  new Range('a', 'f'),
                  new Range('A', 'F'),
                  new Range('0', '9')
               );
                           
            Match* escapedCharacter =
               new And(
                  new BeeFishParser::
                     Character('%'),
                  hexCharacter->copy(),
                  hexCharacter->copy()
               );
               
            Match* pathCharacter =
               new Or(
                  simpleCharacter->copy(),
                  escapedCharacter->copy()
               );
            
            Match* path =
               new Repeat(pathCharacter->copy());
               
            _path = new Capture(
               path->copy()
            );
               
            Match* query =
               new Optional(
                  new And(
                     new BeeFishParser::
                        Character('?'),
                     new Repeat(pathCharacter->copy())
                  )
               );
               
            _query = new Capture(
               query->copy()
            );
            
            _match = new And(_path, _query);

            delete simpleCharacter;
            delete hexCharacter;
            delete escapedCharacter;
            delete pathCharacter;
            delete path;
            delete query;
         }
         
         URL(const URL& source) : URL()
         {
         }
         
         virtual Match* copy() const
         {
            return new URL();
         }
         
         const BString& path() const
         {
            return _path->value();
         }
         
         const BString& query() const
         {
            return _query->value();
         }
         
         virtual void write(
            ostream& out,
            size_t tabIndex = 0
         ) const
         {
            out << tabs(tabIndex) 
                << "URL";
            writeResult(out);
            out << endl
                << tabs(tabIndex)
                << "(";
            _match->write(out, tabIndex + 1);
            out << tabs(tabIndex) 
                << ")";
         }
      };
 
      class FirstLine : public Match
      {
      public:
         BString _method;
         URL*    _url;
         BString _version;
      public:
         FirstLine() : Match()
         {
            Match* method =
               new Or(
                  new Word("GET"),
                  new Word("PUT"),
                  new Word("POST"),
                  new Word("DELETE"),
                  new Word("OPTIONS")
               );

            Match* version = new And(
               new Word("HTTP/1."),
               new Range('0', '9')
            );


            _match = new And(
               new Capture(
                  method->copy(),
                  _method
               ),
               new Blanks(),
               _url = new URL(),
               new Blanks(),
               new Capture(
                  version->copy(),
                  _version
               ),
               new NewLine()
            );

            delete method;
            delete version;
         }
         
         virtual ~FirstLine()
         {
         }
         
         virtual Match* copy() const
         {
            return new FirstLine();
         }

         const URL& url() const
         {
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
      _JSON*     _json      = nullptr;
      Optional*  _body      = nullptr;
      Request() :
         Match()
      {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         _body = new Optional(
            _json = new _JSON()
         );
#warning "JSON->_capture could use up all memory. Need to stream this."    
         _json->_capture = true;

         _match = new
            And(
               _firstLine,
               _headers,
               new NewLine(),
               _body
            );

      }
    
      virtual ~Request()
      {
      }
      
      virtual bool hasBody()
      {
         return _body->matched();
      }
      
      virtual bool hasJSON()
      {
         return _json->matched();
      }
   
      virtual _JSON& json()
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
