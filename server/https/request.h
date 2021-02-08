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

   const Match BlankChar =
      Character(' ') or
      Character('\t');

   const Repeat Blanks(BlankChar);

   const Match NewLine =
      (
         Character('\r') and
         ~Character('\n')
      ) or
      Character('\n');

   const Match Base64Char =
      Range('0', '9') or
      Range('a', 'z') or
      Range('A', 'Z') or
      Character('+') or
      Character('/');
   
   const Match Base64 =
      Repeat(Base64Char) and
      ~Character('=') and
      ~Character('=');
   
   const Match Colon =
      ~Blanks and
      Character(':') and
      ~Blanks;

   const Match HeaderNameCharacter =
      not (
         Character(':') or
         BlankChar or
         NewLine
      );

   const Repeat HeaderName(
      HeaderNameCharacter
   );

   const Match HeaderValueCharacter =
      Not(NewLine);


   const Repeat HeaderValue(
      HeaderValueCharacter
   );

   class Header : public Match
   {
   protected:
      string _name;
      string _value;
   public:
      Header() : Match(
         Capture(
            HeaderName,
            _name
         ) and
         Colon and
         Capture(
            HeaderValue,
            _value
         ) and
         NewLine
      )
      {
      }
      
      virtual string& name()
      {
         return _name;
      }
      
      virtual string& value()
      {
         return _value;
      }
   };


   class Headers :
      public Repeat,
      public map<std::string, std::string>
   {
   public:
      Headers() : Repeat(Header())
      {}

      virtual void addItem(Match* match) {
    
         Header* header = (Header*)match;
   
         std::string lower_name =
            boost::to_lower_copy(
               header->_name()
            );
         
         map<std::string, std::string>::
            operator[] (lower_name) = header->value();
      
         Repeat::addItem(match);
      }
   
      std::string& operator[] (const string& name)
      {
         std::string lower_name =
            boost::to_lower_copy(
               name
            );
         return map<std::string, std::string>::
            operator[] (lower_name);
      }
   
      bool contains(const string& name) 
      {
         std::string lower_name =
            boost::to_lower_copy(
               name
            );
         return count(lower_name) > 0;
      }
   
      friend ostream& operator << (ostream& out, Headers& headers)
      {
         for (auto it = headers.begin();
                   it != headers.end();
                 ++it)
         {
            string header = it->first;
            string value = it->second;
            out
               << header
               << '\t'
               << value
               << endl;
         }
      
         return out;
      }
   
   
   
   };

   const Match Version =
      Word("HTTP/1.") and
      Range('0', '9');

   const Match PathCharacter =
      not (
         BlankChar or
         Character('\r') or
         Character('\n')
      );
   
   const Repeat Path(PathCharacter);

   const Match Method =
      Word("GET") or
      Word("PUT") or
      Word("POST") or
      Word("DELETE") or
      Word("OPTIONS");
      

   class FirstLine : public Match
   {
   protected:
      string _method;
      string _path;
      string _version;
   public:
      FirstLine() : Match(
         Capture(
            Method,
            [this](Capture& item)
            {
               this->_method =
                  item.value();
            }
         ) and
         Blanks and
         Capture(
            Path,
            [this](Capture& item)
            {
               this->_path =
                  item.value();
            }
         ) and
         Blanks and
         Capture(
            Version,
            [this](Capture& item)
            {
               this->_version =
                  item.value();
            }
         )
         NewLine
      (
      {}
      
      virtual string& method()
      {
         return _method;
      }
      
      virtual string& path()
      {
         return _path;
      }
      
      virtual string& version()
      {
         return _version;
      }
   };
   
   class Body : public Match
   {
   protected:
      unsigned long _contentLength;
   
   public:
      Body() : Match(Object)
      {
         _contentLength = 0;
      }
   
   
      bool hasToken()
      {
         return (contains(L"token"));
      }
   
      Object& token()
      {
         JSON& json = (*this)[L"token"];
      return (Object&)json.item();
   }
   
   string& method()
   {
      return (*this)[L"method"].value();
   }
   
   string& key()
   {
      return (*this)[L"key"].value();
   }
   
   string& value()
   {
      return (*this)[L"value"].value();
   }
   
   bool valueIsNull()
   {
      JSON& jsonValue = (*this)[L"value"];
      return jsonValue.isNull();
   }
   
   virtual bool match(int character)
   {
      ++_contentLength;
      
      return Object::match(character);
   }
   
   virtual unsigned long contentLength()
   {
      return _contentLength;
   }
   

};

class Request : public And {
public:
   FirstLine* _firstLine;
   Optional*  _optionalBody;
   Body*      _body;
   Headers*   _headers;
   long _contentLength;
   Request() :
      And(
      
         _firstLine = new FirstLine(),
         _headers   = new Headers(),
         new NewLine(),
         new Optional(
            new NewLine()
         ),
         _optionalBody = new Optional(
            _body = new Body()
         )
      )
   {
      _contentLength = -1;
   }
   
   virtual optional<bool> result()
   {
      optional<bool> result = And::result();
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
   
   virtual FirstLine& firstLine() {
      return *_firstLine;
   }
   
   virtual string& method() {
      return _firstLine->method();
   }
   
   virtual string& path() {
      return _firstLine->path();
   }
   
   virtual string& version() {
      return _firstLine->version();
   }
   
   virtual Headers& headers()
   {
      return *_headers;
   }
   
   virtual bool hasBody()
   {
      return _optionalBody->matched();
   }
   
   virtual Body& body()
   {
      return *_body;
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
};

};

#endif
