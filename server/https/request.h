#ifndef BEE_FISH__REQUEST_H
#define BEE_FISH__REQUEST_H

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json.h"
#include "../database/path.h"

using namespace bee::fish::parser;
using namespace bee::fish::json;
using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
      
namespace bee::fish::https {

   class Request : public Match {
   public:
   
      class BlankChar : public Or
      {
      public:
         BlankChar() : Or(
            new Character(' '),
            new Character('\t')
         )
         {
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
      };
      
      class NewLine : public Match
      {
      public:
         NewLine() : Match(
            (
               Character('\r') and
               ~Character('\n')
            ) or
            Character('\n')
         )
         {
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
            MatchPointer Colon =
               ~Blanks() and
               Character(':') and
               ~Blanks();

            MatchPointer
               HeaderNameCharacter =
                  not (
                     Character(':') or
                     BlankChar() or
                     NewLine()
                  );

            MatchPointer
               HeaderName =
                  new Repeat(
                     HeaderNameCharacter
                  );

            MatchPointer
               HeaderValueCharacter =
                  not NewLine();

            MatchPointer
               HeaderValue =
                  new Repeat(
                     HeaderValueCharacter
                 );


            _match = new And(
               new Capture(
                  HeaderName,
                  this->_name
               ),
               Colon.get(),
               new Capture(
                  HeaderValue,
                  this->_value
               ),
               new NewLine()
            );
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
            if (contains(name))
               return map<BString, BString>::at(name);
            return bee::fish::b_string::Null;
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


      class FirstLine : public Match
      {
      public:
         BString _method;
         BString _path;
         BString _version;
      public:
         FirstLine() : Match()
         {
            MatchPointer Method =
               new Or(
                  new Word("GET"),
                  new Word("PUT"),
                  new Word("POST"),
                  new Word("DELETE"),
                  new Word("OPTIONS")
               );

            MatchPointer Version =
               Word("HTTP/1.") and
               Range('0', '9');

            MatchPointer PathCharacter =
               not (
                  BlankChar() or
                  Character('\r') or
                  Character('\n')
               );
   
            MatchPointer Path = 
               new Repeat(PathCharacter);

            _match = new And(
               new Capture(
                  Method.get(),
                  _method
               ),
               new Blanks(),
               new Capture(
                  Path.get(),
                  _path
               ),
               new Blanks(),
               new Capture(
                  Version.get(),
                  _version
               ),
               new NewLine()
            );
         }
         
         virtual ~FirstLine()
         {
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
      class Body :
         public _Object
      {
      public:
         Body()
         {
            _capture = true;
         }
   
         virtual ~Body()
         {
         }
   
   
         _JSON& token()
         {
            _JSON* json = (*this)["token"];
            return *json;
         }
   
         const BString& method()
         {
            return (*this)["method"]->value();
         }
   
         const BString& key()
         {
            return (*this)["key"]->value();
         }
   
         const BString& value()
         {
            return (*this)["value"]->value();
         }
   
         bool valueIsNull()
         {
            if (!contains("value"))
               return true;
            else
               return (*this)["value"]->isNull();
         }
   

      };
      
      FirstLine* _firstLine;
      Headers*   _headers;
      Optional*  _optionalBody;
      Body*      _body;
      
      Request() :
         Match()
      {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         _body = new Body();
         _optionalBody =
            new Optional(_body);

         _match = new
            And(
               _firstLine,
               _headers,
               new NewLine(),
               //_optionalBody,
              // new Optional(
                  new NewLine()
              // )
            );
            
      }
    
      virtual ~Request()
      {
      }
   
      virtual bool hasBody()
      {
         return _optionalBody->matched();
      }
   
      virtual Body& body()
      {
         return *_body;
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
      
      const BString& path() const
      {
         return _firstLine->_path;
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
               vector<BString> nameValue =
                  cookie.trim().split('=');
            
               if (nameValue[0].trim() == cookieName)
               {
                  return (nameValue[1].trim());

               }
            }
         }
         
         return "";
      }
      
   };
   

};

#endif
