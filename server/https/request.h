#ifndef BEE_FISH_SERVER__REQUEST_H
#define BEE_FISH_SERVER__REQUEST_H

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "../parser/parser.h"
#include "../parser/json/json.h"

using namespace bee::fish::parser;
using namespace bee::fish::parser::json;

namespace bee::fish::server {
		
			class BlankChar : public Or {
			public:
			   BlankChar() : Or(
			      new Character(' '),
			      new Character('\t')
			   )
			   {
			   }
			};
			
			class Blanks : public Repeat<BlankChar>
			{
			public:
			   Blanks() : Repeat<BlankChar>()
			   {}
			};
			
			class NewLine : public Or {
			public:
			   NewLine() : Or(
			      new And(
			         new Character('\r'),
			         new Optional(
			            new Character('\n')
			         )
			      ),
			      new Character('\n')
			   )
			   {}
			};
			
			class Base64Char : public Or {
			public:
			   Base64Char() : Or (
			      new Range('0', '9'),
			      new Range('a', 'z'),
			      new Range('A', 'Z'),
			      new Character('+'),
			      new Character('/')
			   )
			   {}
			};
			   
			class Base64 : public And {
			public:
			   Base64() : And(
			      new Repeat<Base64Char>(),
			      new Optional(
			         new Character('=')
			      ),
			      new Optional(
			         new Character('=')
			      )
			   )
			   {
			   }
			};   
			   
			class Colon : public And {
			public:
			   Colon() : And(
			      new Optional(
			         new Blanks()
			      ),
			      new Character(':'),
			      new Optional(
			         new Blanks()
			      )
			   )
			   {}
			};
			
			class HeaderNameCharacter : public Not {
			public:
			   HeaderNameCharacter() : Not(
			      new Or(
			         new Character(':'),
			         new BlankChar(),
			         new NewLine()
			      )
			   )
			   {
			   }
			};
			
			class HeaderName :
			   public Repeat<HeaderNameCharacter>
			{
			public:
			   HeaderName() :
			      Repeat<HeaderNameCharacter>()
			   {
			      _capture = true;
			   }
			};
			
			class HeaderValueCharacter: public Not {
			public:
			   HeaderValueCharacter() :
			      Not(
			         new NewLine()
			      )
			   {
			   }
			};
			
			
			class HeaderValue:
			   public Repeat<HeaderValueCharacter>
			{
			public:
			   HeaderValue() :
			      Repeat<HeaderValueCharacter>()
			   {
			      _capture = true;
			   }
			};
			
			class Header :
			   public And
			{
			public:
			   Header() : And(
			      new HeaderName(),
			      new Colon(),
			      new HeaderValue(),
			      new NewLine()
			   )
			   {
			   }
			   
			   virtual ~Header() {
			   }
			   
			   virtual string& _name() {
			      return (*this)[0].value();
			   }
			   
			   virtual string& value() {
			      return (*this)[2].value();
			   }
			   
			   virtual void write(ostream& out) {
			      out << "Header(" 
			          << _value 
			          << ")"
			          << endl;
			   }
			   
			};
			
			
			class Headers :
			   public Repeat<Header>,
			   public map<std::string, std::string>
			{
			public:
			   Headers() : Repeat<Header>()
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
			
			class Version : public And {
			public:
			   Version() : And(
			      new Word("HTTP/1."),
			      new Range('0', '9')
			   )
			   {
			      _capture = true;
			   }
			};
			
			class PathCharacter : public Not {
			public:
			   PathCharacter() : Not(
			      new Or(
			         new BlankChar(),
			         new Character('\r'),
			         new Character('\n')
			      )
			   )
			   {
			   }
			};
			
			class Path :
			   public Repeat<PathCharacter> {
			public:
			   Path() : Repeat<PathCharacter>()
			   {
			      _capture = true;
			   }
			};
			
			class Method : public Or {
			public:
			   Method() : Or(
			      new Word("GET"),
			      new Word("PUT"),
			      new Word("POST"),
			      new Word("DELETE"),
			      new Word("OPTIONS")
			   )
			   {
			      _capture = true;
			   }
			};
			
			class FirstLine : public And {
			public:
			   FirstLine() : And(
			      new Method(),
			      new Blanks(),
			      new Path(),
			      new Blanks(),
			      new Version(),
			      new NewLine()
			   )
			   {
			   }
			   
			   virtual string& method() 
			   {
			      return (*this)[0].value();
			   }
			   
			   virtual string& path()
			   {
			      return (*this)[2].value();
			   }
			   
			   virtual string& version() 
			   {
			      return (*this)[4].value();
			   }
			};
			
			class Body : public Object
			{
			public:
			   Body() : Object()
			   {
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
			   
			};
			
			class Request : public And {
			public:
			   Optional* _optionalBody;
			   Body* _body;
			   Request() :
			      And(
			      
			         new FirstLine(),
			         new Headers(),
			         new NewLine(),
			         _optionalBody = new Optional(
			            _body = new Body()
			         )
			      )
			   {
			   }
			   
			   virtual FirstLine& firstLine() {
			      return (FirstLine&)((*this)[0]);
			   }
			   
			   virtual string& method() {
			      return firstLine().method();
			   }
			   
			   virtual string& path() {
			      return firstLine().path();
			   }
			   
			   virtual string& version() {
			      return firstLine().version();
			   }
			   
			   virtual Headers& headers()
			   {
			      return (Headers&)((*this)[1]);
			   }
			   
			   virtual bool hasBody()
			   {
			      //Optional& body =
			       //  (Optional&)((*this)[3]);
			      return _optionalBody->matched();
			   }
			   
			   virtual Body& body()
			   {
			      //Optional& optional =
			      //   (Optional&)((*this)[3]);
			     // Body& body =
			      //   (Body&)optional.item();
			      return *_body;
			   }
			  
			
			};
			
};
			
#endif
