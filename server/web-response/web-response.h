#ifndef BEE_FISH_WEB__WEB_RESPONSE_H
#define BEE_FISH_WEB__WEB_RESPONSE_H

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

   class WebResponse;

   class ContentLength : public Match {
   protected:
      size_t _contentCount = 0;
      WebResponse* _webResponse = nullptr;
   public:

      ContentLength()
      {
      }

      void setWebResponse(WebResponse* webResponse) {
         _webResponse = webResponse;
      }

      virtual bool matchCharacter(const Char& character);

   };
   
   class JSONWebResponseBody :
         public BeeFishJSON::Object
   {
   protected:
      WebResponse* _webResponse = nullptr;

   public:
      JSONWebResponseBody() {
            
      }

      void setWebResponse(WebResponse* webResponse) {
         _webResponse = webResponse;
      }
   };


   class WebResponse : public Match {
   public:
      class StatusLine;
      class Headers;
   protected:
      StatusLine* _statusLine;
      Headers* _headers;
      Match* _body;
      size_t _contentLength = -1;
   public:
      WebResponse(Match* body) {
         _match = new And(
            _statusLine = new StatusLine(),
            _headers = new Headers(),
            new Invoke(
               new CRLF(),
               [this](Match*) {
                  if (this->_headers->has("content-length")) {
                     const BString& contentLength = (*(this->_headers))["content-length"];
                     this->_contentLength = atoi(contentLength.c_str());
                  }
               }
            ),
            _body = body
         );
      }
      
      Headers* headers() {
         return _headers;
      }

      StatusLine* statusLine() {
         return _statusLine;
      }

      Match* body() {
         return _body;
      }

      size_t contentLength() {
         return _contentLength;
      }

   public:

      class CRLF : public Word {
      public:
         CRLF() : Word("\r\n") {

         }
      };

      class StatusLine : public And {
      public:
         class StatusCode;

      protected:
         StatusCode* _statusCode;

      public:
         StatusLine() : And(
            new HTTPVersion(),
            new Blanks(),
            _statusCode = new StatusCode(),
            new Blanks(),
            new ReasonPhrase(),
            new CRLF()

         )
         {

         }

      public:
         StatusCode* statusCode() {
            return _statusCode;
         }
      public:

         class HTTPVersion : public And {
         public:
            HTTPVersion() : And(
               new Word("HTTP/1."),
               new Or(
                  new BeeFishParser::Character('0'),
                  new BeeFishParser::Character('1')
               )
            )
            {

            }
         };

         class StatusCode : public Match {
         protected:
            Capture* _value;

         public:
            StatusCode() : Match(
               _value = new Capture(
                  new Repeat<Digit>(3, 3)
               )
            ) 
            {

            }

         public:
            int value() {
               if (_value->matched())
                  return atoi(_value->value());
               else
                  return -1;
            }

         public:
            class Digit : public Range {
            public:
               Digit() : Range('0', '9') {

               }
            };
         };


         class ReasonPhrase : public And {
         public:
            ReasonPhrase() : And(
               new Repeat<ReasonPhrase::Character>()
            )
            {

            }
         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               ) 
               {
                  
               }
            };
         };
         

      };

      class Header : public And {
      protected:
         Capture* _name;
         Capture* _value;
      public:
         Header() : And(
            _name = new Capture(new Name()),
            new Optional(new Blanks()),
            new BeeFishParser::Character(':'),
            new Optional(new Blanks()),
            _value = new Capture(new Value()),
            new CRLF()
         )
         {

         }

      public:
         const BString& name() {
            return _name->value();
         }

         const BString& value() {
            return _value->value();
         }

      public:
         class Name : public And {
         public:
            Name() : And(
               new Repeat<Name::Character>()
            )
            {

            }
         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character(':'),
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               )
               {

               }
            };

         };

         class Value : public Match {
         public:
            Value() : Match(
               new Repeat<Value::Character>()
            )
            {
            }

         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               )
               {

               }
            };

         };

      };

      class Headers : 
         public Repeat<Header>,
         public std::map<BString, BString>  {

      public:
		   virtual void matchedItem(Header *match) {
            const BString& name = match->name();
            const BString lowerName = name.toLower();
            const BString& value = match->value();
            insert(std::pair<BString, BString>(lowerName, value));
            Repeat<Header>::matchedItem(match);
         }

         bool has(const BString& name) {
            return (count(name) > 0);
         }
      };

      
   };

   inline bool ContentLength::matchCharacter(const Char& character) {

      ++_contentCount;

      size_t contentLength = _webResponse->contentLength();   

      if ( contentLength != -1 && _contentCount >= contentLength )
         _result = true;

      return true;
   }




};

#endif
