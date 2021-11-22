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
               BeeFishParser::
                  Character('\r') and
               ~BeeFishParser::
                  Character('\n')
            ) or
            BeeFishParser::Character('\n')
         )
         {
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
      /*
      class NewLine : public Word
      {
      public:
         NewLine() : Word("\r\n")
         {
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
      */
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
               BeeFishParser::
                  Character(':') and
               ~Blanks();

            MatchPointer
               HeaderNameCharacter =
                  not (
                     BeeFishParser::
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
         MatchPointer<Capture> _path;
         MatchPointer<Capture> _query;
      public:
         
         URL() : Match()
         {
           
            MatchPointer SimpleCharacter =
               not (
                  BlankChar() or
                  BeeFishParser::
                     Character('\r') or
                  BeeFishParser::
                     Character('\n') or
                  BeeFishParser::
                     Character('?')
               );
   
            MatchPointer HexCharacter =
               Range('a', 'f') or
               Range('A', 'F') or
               Range('0', '9');
               
            MatchPointer EscapedCharacter =
               BeeFishParser::
                  Character('%') and
               HexCharacter.copy() and
               HexCharacter;
               
            MatchPointer PathCharacter =
               SimpleCharacter or
               EscapedCharacter;
               
            MatchPointer Path =
               Repeat(PathCharacter.copy());
               
            _path = Capture(
               Path.get()
            );
               
            MatchPointer Query =
               Optional(
                  BeeFishParser::
                     Character('?') and
                  Repeat(PathCharacter)
               );
               
            _query = Capture(
               Query.get()
            );
            
            _match = _path and _query;
         }
         
         URL(const URL& source) : URL()
         {
         }
         
         virtual Match* copy() const
         {
            return new URL(*this);
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


            _match = new And(
               new Capture(
                  Method.get(),
                  _method
               ),
               new Blanks(),
               _url = new URL(),
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
