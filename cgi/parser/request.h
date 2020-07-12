#ifndef BEE_FISH_SERVER__REQUEST_H
#define BEE_FISH_SERVER__REQUEST_H

#include <map>
#include <boost/algorithm/string.hpp>

#include "parser.h"


using namespace bee::fish::parser;

namespace bee::fish::server {

class BlankChar : public Or {
public:
   BlankChar() :
      Or(
         {
            new Character(' '),
            new Character('\t')
         }
      )
   {
   }
};

class Blanks :
   public Repeat<BlankChar> 
{

};

class NewLine : public And {
public:
   NewLine() :
      And({
         new Character('\r'),
         new Character('\n')
      })
   {
   }
   
};

class Base64Char : public Or {
public:
   Base64Char() :
      Or ({
         new Range('0', '9'),
         new Range('a', 'z'),
         new Range('A', 'Z')
      })
   {
   }
};
   
class Base64 : public And {
public:
   Base64(Match* next) : And({
      new Repeat<Base64Char>,
      new Optional(
         new Character('='),
         next
      )
   })
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
};

class Colon : public Character {
public:
   Colon() : Character(':')
   {
   }
};

class HeaderNameCharacter : public Not {
public:
   HeaderNameCharacter() :
      Not(
         new Or(
            {
               new Colon(),
               new NewLine()
            }
         )
      )
   {
   }
};

class HeaderName :
   public Repeat<HeaderNameCharacter> {
};

class AbstractHeader {
public:
   virtual const string& name() const = 0;
   virtual const string& value() const = 0;
};

class GenericHeader :
   public And,
   public AbstractHeader
{
public:
   GenericHeader() : And(
      {
         new HeaderName(),
         new Optional(
            new Blanks(),
            new Colon()
         ),
         new Optional(
            new Blanks(),
            new HeaderValue()
         ),
         new NewLine()
      }
   )
   {
   }
   
   virtual const string& name() const {
      return _inputs[0]->value();
   }
   
   virtual const string& value() const {
      return _inputs[4]->value();
   }
};

class BasicAuthorizationHeader :
   public And,
   public AbstractHeader {
   
public:
   BasicAuthorizationHeader() : And({
      new CIWord("Authorization"),
      new Optional(
         new Blanks(),
         new Colon()
      ),
      new Optional(
         new Blanks(),
         new And({
            new CIWord("Basic"),
            new Blanks(),
            new Base64(new NewLine())
         })
      )
   })
   {
   }
      
   virtual const string& name() const {
      return _inputs[0]->value();
   }
   
   virtual const string& value() const {
      return _inputs[4]->value();
   }
   
   string base64() const {
      return _inputs[4]->
         inputs()[2]->value();
   }
      
};

class Header : 
   public Or,
   public AbstractHeader
{
public:
   Header() : Or({
      //new BasicAuthorizationHeader(),
      new GenericHeader()
     // new GenericHeader()
   })
   {}
   
   virtual BasicAuthorizationHeader*
   basicAuthorizationHeader() const {
      return
         (BasicAuthorizationHeader*)
            (this->item());
   }
   
   virtual const string& name() const {
      return this->item()->name();
   }
   
   virtual const string& value() const {
      return this->item()->value();
   }
   
   Header* item() const {
      return (Header*)(Or::item());
   }
   
};

class Headers :
   public Repeat<Header>,
   public map<string, AbstractHeader*>
{
public:
/*
   virtual void addItem(Header* header) {
      
      (*this)[
         boost::to_lower_copy(
            header->name()
         )
      ] = header;
   }
   
   virtual void checkSuccess() {
      if (size() > 0) {
         setSuccess(true);
      }
      else {
         setSuccess(false);
      }
   }
   */
};

class Version : public Word {
public:
   Version() :
      Word("HTTP/1.1")
   {
   }
};

class PathCharacter : public Not {
public:
   PathCharacter() :
      Not(
         new Or({
            new BlankChar(),
            new NewLine()
         })
      )
   {
   }
};

class Path : public Repeat<PathCharacter> {
public:
   Path() {
   }
};

class verb : public Or {
public:
   verb() :
      Or({
         new Word("GET"),
         new Word("PUT"),
         new Word("POST"),
         new Word("DELETE"),
         new Word("OPTIONS")
      })
   {
   }
};

class FirstLine : public And {
public:
   FirstLine() :
      And({
         new verb(),
         new Blanks(),
         new Path(),
         new Blanks(),
         new Version(),
         new NewLine()
      })
   {
   }
   
   virtual const string& verb() {
      return inputs()[0]->value();
   }
   
   virtual const string& path() {
      return inputs()[2]->value();
   }
   
   virtual const string& version() {
      return inputs()[4]->value();
   }
};

class request : public And {
public:
   request() :
      And({
         new FirstLine()
        // new Header()
         /*new NewLine()*/
      })
   {
   }
   
   FirstLine* firstLine() {
      return (FirstLine*)(inputs()[0]);
   }
   
   virtual const string& verb() {
      return firstLine()->verb();
   }
   
   virtual const string& path() {
      return firstLine()->path();
   }
   
   virtual const string& version() {
      return firstLine()->version();
   }
   
   virtual Headers* headers() const
   {
      return (Headers*)(inputs()[1]);
   }
  

};

};

#endif
