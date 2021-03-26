#ifndef BEE_FISH__REQUEST_H
#define BEE_FISH__REQUEST_H

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json.h"

using namespace bee::fish::parser;
using namespace bee::fish::json;

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
      protected:
         const MatchPointer<And> Colon =
            ~Blanks() and
            Character(':') and
            ~Blanks();

         const MatchPointer<Not>
            HeaderNameCharacter =
               not (
                  Character(':') or
                  BlankChar() or
                  NewLine()
               );

         const MatchPointer<Repeat>
            HeaderName =
               new Repeat(
                  HeaderNameCharacter
               );

         const MatchPointer<Not>
            HeaderValueCharacter =
               not NewLine();

         const MatchPointer<Repeat>
            HeaderValue =
               new Repeat(
                  HeaderValueCharacter
              );

      public:
         BString _name;
         BString _value;
         
      public:
         Header() : Match()
         {
            _match = new And(
               new Capture(
                  HeaderName,
                  this->_name
               ),
               Colon.copy(),
               new Capture(
                  HeaderValue,
                  this->_value
               ),
               new NewLine()
            );
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
         {}

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
   
         bool contains(const BString& name) 
         {
            return count(name) > 0;
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
         const MatchPointer<Or> Method =
            new Or(
               new Word("GET"),
               new Word("PUT"),
               new Word("POST"),
               new Word("DELETE"),
               new Word("OPTIONS")
            );

         const MatchPointer<And> Version =
            Word("HTTP/1.") and
            Range('0', '9');

         const MatchPointer<Not> PathCharacter =
            not (
               BlankChar() or
               Character('\r') or
               Character('\n')
            );
   
         const Repeat Path =
            Repeat(PathCharacter);


      public:
         BString _method;
         BString _path;
         BString _version;
      public:
         FirstLine() : Match()
         {
            _match = new And(
               new Capture(
                  Method.copy(),
                  _method
               ),
               new Blanks(),
               new Capture(
                  Path.copy(),
                  _path
               ),
               new Blanks(),
               new Capture(
                  Version.copy(),
                  _version
               ),
               new NewLine()
            );
         }
      
      };
   
      class Body :
         public bee::fish::json::_Object
      {
      protected:
         unsigned long _contentLength;
   
      public:
         Body() : _Object()
         {
            _contentLength = 0;
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
   
         virtual bool match(
            const Char& character
         )
         {
            ++_contentLength;
      
           return _Object::match(character);
         }
   
         virtual unsigned long contentLength()
         {
            return _contentLength;
         }
   

      };

   
      FirstLine* _firstLine = new FirstLine();
      Headers*   _headers = new Headers();
      Body*      _body = new Body();
      
      Optional*   _optionalBody =
         new Optional(_body);
      
      long _contentLength;
           
      Request() : Match()
      {
         _match = new
            And(
               _firstLine,
               _headers,
               new NewLine(),
               new Optional(
                  new NewLine()
               ),
               _optionalBody
            );
         _contentLength = -1;
      }
    
      virtual bool match(const Char& character)
      {
       
         bool matched = Match::match(character);
         
         unsigned long contentLength =
            Request::contentLength();
            
         unsigned long bodyContentLength =
            _body->contentLength();
      
         if (_result == nullopt)
         {
            if ( contentLength ==
                 bodyContentLength )
               success();
         }
         
         return matched;
         
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
  
      virtual long contentLength()
      {
         if (_contentLength != -1)
            return _contentLength;
         
         if ( _headers->contains(
                 "content-length"
              ) )
         {
            string length =
               (*_headers)["content-length"];
            _contentLength =
               atol(length.c_str());
         }
 
         return _contentLength;
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
      
   };
   

};

#endif
