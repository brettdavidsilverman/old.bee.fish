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

   class WebResponseBody : public BeeFishBString::BStream
   {
   protected:
      WebResponse* _webResponse = nullptr;
   public:

      WebResponseBody();

      void setWebResponse(WebResponse* webResponse) {
         _webResponse = webResponse;
      }

   };

   class ContentLength : 
      public Match,
      public WebResponseBody 
   {
   protected:
      size_t _contentCount = 0;
      size_t _contentLength = -1;
   public:

      ContentLength(size_t contentLength) :
         _contentLength(contentLength)
      {
      }

      virtual bool matchCharacter(const Char& character) {

         ++_contentCount;

         if ( _contentCount >= _contentLength )
            _result = true;

         push_back((uint8_t)character);

         return true;
      }

      virtual void success() {
         flush();
         Match::success();
      }


   };
   
   class JSONWebResponseBody :
         public BeeFishJSON::Object,
         public WebResponseBody
   {

   public:
      JSONWebResponseBody() {
            
      }

      virtual bool matchCharacter(const Char& character) {

         bool matched = BeeFishJSON::Object::matchCharacter(character);

         if (matched)
            *this << character;

         return true;
      }

      virtual void success() {
         flush();
         Match::success();
      }

   };


   class WebResponse : 
      public And,
      public BeeFishBString::BStream {
   public:
      class StatusLine;
      class Headers;
      typedef std::function<void(const Data& data)> OnData;
   protected:
      StatusLine* _statusLine;
      Headers* _headers;
      Match* _body = nullptr;
      size_t _contentLength = -1;
      bool _authenticated = false;
      OnData _ondata = nullptr;
   public:
      WebResponse() : And(
            _statusLine = new StatusLine(),
            _headers = new Headers(),
            new Invoke(
               new CRLF(),
               [this](Match*) {
                  this->createBody();
               }
            )
         )
      {
      }

      virtual void ondata(const BeeFishBString::Data& data) {
         if (_ondata) {
            if (_statusLine->statusCode()->value() == 200) {
               _ondata(data);
            }
         }
      }

      void setOnData(OnData ondata) {
         _ondata = ondata;
      }

      void createBody() {

         if (_headers->has("content-type")) {
            const BString& contentType = _headers->at("content-type");

            if (contentType.startsWith("application/json")) {
               JSONWebResponseBody* body = new JSONWebResponseBody();
               body->setOnKeyValue(
                  [this](const BString& key, BeeFishJSON::JSON& value) {
                     if (key == "authenticated") {
                        this->_authenticated = (value.value() == "true");
                     }
                  }
               );
               body->setWebResponse(this);
               _body = body;
            }
         }

         if (_body == nullptr && _headers->has("content-length")) {
            const BString& contentLength = _headers->at("content-length");
            _contentLength = atoi(contentLength.c_str());
            ContentLength* body = new ContentLength(_contentLength);
            body->setWebResponse(this);
            _body = body;
            _parser->setDataBytes(this->_contentLength);
         }

         if (_body) {
            // Adding new body to end of inputs
            And::push_back(_body);
         }

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

      bool authenticated() {
         return _authenticated;
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


   inline WebResponseBody::WebResponseBody() {
      _onbuffer = [this](const BeeFishBString::Data& data) {
         this->_webResponse->ondata(data);
      };
   }



};

#endif
