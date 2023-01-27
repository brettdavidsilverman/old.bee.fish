#ifndef BEE_FISH_WEB__WEB_RESPONSE_H
#define BEE_FISH_WEB__WEB_RESPONSE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#ifdef SERVER
#include "../database/path.h"
#endif
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
            new NewLine()

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
               if (_value->matched()) {
                  std::string value = _value->value().str();
                  return atoi(value.c_str());                  
               }
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

   protected:
      StatusLine* _statusLine;
      BeeFishWeb::Headers* _headers;
      Body* _body = nullptr;
      size_t _contentLength = -1;
      bool _authenticated = false;
      BStream::OnBuffer _ondata = nullptr;
      bool _parseJSON;
   public:
      WebResponse(bool parseJSON = true) : And() {
         _parseJSON = parseJSON;
      }

      virtual void setup(Parser* parser) {
         _inputs = {
            _statusLine = new StatusLine(),
            _headers = new BeeFishWeb::Headers(),
            new Invoke(
               new NewLine(),
               [this](Match*) {
                  this->createBody();
               }
            )
         };

         And::setup(parser);

      }

      virtual void flush() {
         if (_body)
            _body->flush();
      }

      void setOnData(BStream::OnBuffer ondata) {
         _ondata = ondata;
         if (_body)
            _body->setOnBuffer(_ondata);
      }

      virtual void createBody() {

         if ( (*_headers)["content-length"] == "0" ) {
            // No Body, result is true
            _result = true;
         }
         else if ( _headers->contains("content-length") ||
                  (*_headers)["content-type"].startsWith("application/json") )
         {
            _body = new Body(_parseJSON);
            _body->setup(_parser, _headers);
            _body->setOnBuffer(_ondata);
            
            And::push_back(_body);
         }

      }
      
      BeeFishWeb::Headers* headers() {
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
            new NewLine()
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
