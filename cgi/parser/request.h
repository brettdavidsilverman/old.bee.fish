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
         new Character('\r'),
         new Character('\n')
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
         new Or(
            new Character('\r'),
            new Character('\n')
         )
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

class AbstractHeader {
public:
   virtual string name() const = 0;
   virtual string value() const = 0;
};

class GenericHeader :
   public And,
   public AbstractHeader
{
public:
   GenericHeader() : And(
      new HeaderName(),
      new Colon(),
      new HeaderValue(),
      new NewLine()
   )
   {
   }
   
   virtual ~GenericHeader() {
   }
   
   virtual string name() const {
      return _inputs[0]->value();
   }
   
   virtual string value() const {
      return _inputs[2]->value();
   }
   
   virtual void write(ostream& out) {
      out << "GenericHeader{" 
          << _value << "}" << endl;
   }
};

class BasicAuthorizationHeader :
   public And,
   public AbstractHeader {
   
public:
   BasicAuthorizationHeader() : And(
      new CIWord("Authorization"),
      new Colon(),
      new CIWord("Basic"),
      new Blanks(),
      new Base64(),
      new NewLine()
   )
   {
   }
      
   virtual string name() const {
      return _inputs[0]->value();
   }
   
   virtual string value() const {
      return _inputs[4]->value();
   }
   
   string base64() const {
      return _inputs[4]->value();
   }
   
   virtual void write(ostream& out) {
      out << "BasicAuthorizationHeader{"
          << _value << "}" << endl;
   }
      
};
/*
class Header :  public GenericHeader
{
public:
   Header() : GenericHeader(
    
   )
   {}
   
   virtual string name() const 
   {
      return ((AbstractHeader*)_item)->name();
   }
   
   virtual string value() const {
      return Or::value();
   }
   
   virtual AbstractHeader*
   abstractHeader() const
   {
      return (AbstractHeader*)(&(item()));
   }
   
   virtual void write(ostream& out) const {
      cout << "Header{" << Or::value() << "}" << endl;
   }
   
   friend ostream& 
   operator << (ostream& out, const Header& header)
   {
      header.write(out);
      return out;
   }
   
};
*/
class Headers :
   public Repeat<GenericHeader>,
   public map<std::string, AbstractHeader*>
{
public:
   Headers() : Repeat<GenericHeader>()
   {}

   virtual void add_item(GenericHeader* header) {
      //AbstractHeader* abstract = 
      //   header->abstractHeader()
      cerr << *header << endl;
      /*
      std::string name =
         boost::to_lower_copy(
            header->name()
         );
      cerr << header->name() << endl;
      map<std::string, AbstractHeader*>::
         operator[] (name) = 
           &( header->abstractHeader());
      */
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
   
   virtual string method() {
      return inputs()[0]->value();
   }
   
   virtual string path() {
      return inputs()[2]->value();
   }
   
   virtual string version() {
      return inputs()[4]->value();
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
   
   FirstLine& firstLine() const {
      return *(FirstLine*)(_inputs[0]);
   }
   
   virtual const string method() const {
      return firstLine().method();
   }
   
   virtual string path() const {
      return firstLine().path();
   }
   
   virtual string version() const {
      return firstLine().version();
   }
   
   virtual Headers& headers() const
   {
      return *(Headers*)(_inputs[1]);
   }
  

};

};

#endif
