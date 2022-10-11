#ifndef BEE_FISH_WEB__WEB_RESPONSE_H
#define BEE_FISH_WEB__WEB_RESPONSE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"
#include "../web-request/headers.h"
#include "../web-request/content-length.h"
#include "../web-request/body.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {

   class JSONWebResponseBody :
         public BeeFishJSON::Object,
         public BeeFishBString::BStream
   {

   public:
      JSONWebResponseBody() {
            
      }

      virtual bool matchCharacter(const Char& character) {

         bool matched = BeeFishJSON::Object::matchCharacter(character);

         if (matched) {
            (*this) << character;
         }

         return matched;
      }

   };


   class WebResponse : 
      public And
   {

    public:
      class StatusLine;
   protected:
      StatusLine* _statusLine;
      Headers* _headers;
      Body* _body = nullptr;
      size_t _contentLength = -1;
      bool _authenticated = false;
      BStream::OnBuffer _ondata = nullptr;
   public:
      WebResponse() : 
         And(
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
         //if (_ondata && _statusLine->statusCode()->intValue() == 200)
         if (_ondata && _body)
            _ondata(data);
      }

      virtual void flush() {
         if (_body && _body->_contentLength)
            _body->_contentLength->flush();
      }

      void setOnData(BStream::OnBuffer ondata) {
         _ondata = ondata;
      }

      void createBody() {

         if ((*_headers)["content-length"] != "0") {

            _body = new Body();
            _body->setup(_parser, _headers);

            _body->setOnData(_ondata);
            
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
            int intValue() {
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
         BString name() {
            return _name->value();
         }

         virtual BString value() {
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

      
   };


};

#endif
