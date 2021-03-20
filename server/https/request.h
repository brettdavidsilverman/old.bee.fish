#ifndef BEE_FISH_SERVER__REQUEST_H
#define BEE_FISH_SERVER__REQUEST_H

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json.h"

using namespace bee::fish::parser;
using namespace bee::fish::json;

namespace bee::fish::server {

   const MatchPointer BlankChar =
      Character(' ') or
      Character('\t');

   const Repeat Blanks =
      Repeat(BlankChar);

   const MatchPointer NewLine =
      (
         Character('\r') and
         ~Character('\n')
      ) or
      Character('\n');

   const MatchPointer Base64Char =
      Range('0', '9') or
      Range('a', 'z') or
      Range('A', 'Z') or
      Character('+') or
      Character('/');
   
   const MatchPointer Base64 =
      Repeat(Base64Char) and
      ~Character('=') and
      ~Character('=');
   
   const MatchPointer Colon =
      ~Blanks and
      Character(':') and
      ~Blanks;

   const MatchPointer HeaderNameCharacter =
      not (
         Character(':') or
         BlankChar or
         NewLine
      );

   const Repeat HeaderName(
      HeaderNameCharacter
   );

   const MatchPointer HeaderValueCharacter =
      Not(NewLine);


   const Repeat HeaderValue(
      HeaderValueCharacter
   );

   class Header : public Match
   {
   public:
      BString _name;
      BString _value;
   public:
      Header() : Match()
      {
         _match =
            Capture(
               HeaderName.copy(),
               _name
            ) and
            Colon and
            Capture(
               HeaderValue.copy(),
               _value
            ) and
            NewLine;
      }
      
   };


   class Headers :
      public Repeat,
      public map<BString, BString>
   {
   public:
      Headers() : Repeat(new Header())
      {}

      virtual void matchedItem(Match* match) {
    
         Header* header =
            static_cast<Header*>(match);
   
         BString lowerName =
            header->_name.toLower();
         
         emplace(lowerName, header->_value);

         Repeat::matchedItem(match);
      }
   
      bool contains(const BString& name) 
      {
         return count(name) > 0;
      }
   
      friend ostream& operator << (ostream& out, Headers& headers)
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

   const MatchPointer Version =
      Word("HTTP/1.") and
      Range('0', '9');

   const MatchPointer PathCharacter =
      not (
         BlankChar or
         Character('\r') or
         Character('\n')
      );
   
   const Repeat Path(PathCharacter);

   const MatchPointer Method =
      new Or(
         new Word("GET"),
         new Word("PUT"),
         new Word("POST"),
         new Word("DELETE"),
         new Word("OPTIONS")
      );

   class FirstLine : public Match
   {
   public:
      BString _method;
      BString _path;
      BString _version;
   public:
      FirstLine() : Match()
      {
         _match =
            Capture(
               Method.copy(),
               _method
            ) and
            Blanks and
            Capture(
               Path.copy(),
               _path
            ) and
            Blanks and
            Capture(
               Version.copy(),
               _version
            ) and
            NewLine;
      }
      
   };
   
   class Body : public bee::fish::json::_Object
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
   
      virtual bool match(int character)
      {
         ++_contentLength;
      
        return _Object::match(character);
      }
   
      virtual unsigned long contentLength()
      {
         return _contentLength;
      }
   

   };

   class Request : public Match {
   public:
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
               NewLine.copy(),
               new Optional(
                  NewLine.copy()
               ),
               _optionalBody
            );
         _contentLength = -1;
      }
   
      virtual optional<bool> result()
      {
         optional<bool> result = Match::result();
         if (result != true)
            return result;
         
         unsigned long contentLength =
            Request::contentLength();
         unsigned long bodyContentLength =
            _body->contentLength();
      
         result =
            (contentLength ==
            bodyContentLength);
         
         return result;
      
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
