#ifndef BEE_FISH_SERVER__REQUEST_H
#define BEE_FISH_SERVER__REQUEST_H

#include <map>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include <parser.h>


using namespace bee::fish::parser;

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
      new Range('A', 'Z')
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
   {}
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
   {}
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
   {}
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
   
   virtual const string& name() const {
      return (*this)[0].value();
   }
   
   virtual const string& value() const {
      return (*this)[2].value();
   }
   
   virtual string& value() {
      return (*this)[2].value();
   }
   
   virtual void write(ostream& out) const {
      out << "Header{" 
          << _value 
          << "}"
          << endl;
   }
   
};


class Headers :
   public Repeat<Header>,
   public map<std::string, Header*>
{
public:
   Headers() : Repeat<Header>()
   {}

   virtual void add_item(Match* match) {
    
      Header* header = (Header*)match;
   
      std::string lower_name =
         boost::to_lower_copy(
            header->name()
         );
         
      map<std::string, Header*>::
         operator[] (lower_name) = header;
      
   }
   
   Header* operator[] (const string& name) {
      return map<std::string, Header*>::
         operator[] (name);
   }
   
   bool contains(const string& name) {
      return count(name) > 0;
   }
   
   
};

class Version : public Word {
public:
   Version() : Word("HTTP/1.1")
   {
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
   }
};

class Method : public Or {
public:
   Method() : Or(
      {
         new Word("GET"),
         new Word("PUT"),
         new Word("POST"),
         new Word("DELETE"),
         new Word("OPTIONS")
      }
   )
   {
   }
};

class FirstLine : public And {
public:
   FirstLine() : And(
      {
         new Method(),
         new Blanks(),
         new Path(),
         new Blanks(),
         new Version(),
         new NewLine()
      }
   )
   {
   }
   
   virtual const string& method() const {
      return (*this)[0].value();
   }
   
   virtual const string& path() const {
      return (*this)[2].value();
   }
   
   virtual const string& version() const {
      return (*this)[4].value();
   }
};

class request : public And {
public:
   request() :
      And(
         new FirstLine(),
         new Headers(),
         new NewLine()
      )
   {
   }
   
   virtual const FirstLine& firstLine() const {
      return (FirstLine&)((*this)[0]);
   }
   
   virtual const string& method() const {
      return firstLine().method();
   }
   
   virtual const string& path() const {
      return firstLine().path();
   }
   
   virtual const string& version() const {
      return firstLine().version();
   }
   
   virtual Headers& headers() const
   {
      return (Headers&)((*this)[1]);
   }
  

};

};

#endif
