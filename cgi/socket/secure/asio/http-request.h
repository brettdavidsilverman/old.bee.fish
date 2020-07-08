#ifndef BEE_FISH_PARSER_REQUEST
#define BEE_FISH_PARSER_REQUEST

#include <parser.h>

using namespace Bee::Fish::Parser;

namespace Bee::Fish {

class Whitespace : public Or {
public:
   Whitespace() :
      Or(
         new Character(' '),
         new Character('\t')
      )
   {
   }
};

class NewLine : public Or {
public:
   NewLine() :
      Or(
         new And(
            new Character('\r'),
            new Optional(
               new Character('\n')
            )
         ),
         new Character('\n')
      )
   {
   }
   
   virtual const string value() const {
      return "\r\n";
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
};

class Colon : public Character {
public:
   Colon() :
      Character(':')
   {
   }
};

class HeaderNameCharacter : public Not {
public:
   HeaderNameCharacter() :
      Not(
         new Or(
            new Colon(),
            new NewLine()
         )
      )
   {
   }
};

class HeaderName :
   public Repeat<HeaderNameCharacter> {
};

class Header : public And {
public:
   Header() :
      And(
         new HeaderName(),
         new Colon(),
         new Optional(
            new Whitespace()
         ),
         new HeaderValue(),
         new NewLine()
      )
   {
   }
   
   virtual string name() {
      return inputs()[0]->value();
   }
   
   virtual string value() {
      return inputs()[3]->value();
   }
};

class Headers : public Repeat<Header> {
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
         new Or(
            new Whitespace(),
            new NewLine()
         )
      )
   {
   }
};

class Path : public Repeat<PathCharacter> {
public:
   Path() {
   }
};

class Method : public Or {
public:
   Method() :
      Or(
         new Word("GET"),
         new Word("PUT"),
         new Word("POST"),
         new Word("DELETE"),
         new Word("OPTIONS")
      )
   {
   }
};

class FirstLine : public And {
public:
   FirstLine() :
      And(
         new Method(),
         new Whitespace(),
         new Path(),
         new Whitespace(),
         new Version(),
         new NewLine()
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

class http_request : public And {
public:
   http_request() :
      And(
         new FirstLine(),
         new Headers(),
         new NewLine()
      )
   {
   }
   
   FirstLine* firstLine() {
      return (FirstLine*)(inputs()[0]);
   }
   
   virtual string method() {
      return firstLine()->method();
   }
   
   virtual string path() {
      return firstLine()->path();
   }
   
   virtual string version() {
      return firstLine()->version();
   }
   
   virtual vector<Header*> headers() {
   
      Repeat<Header>* repeat =
         (Repeat<Header>*)inputs()[1];
         
      vector<Match*> matches =
         repeat->items();
         
      vector<Header*> headers;
      for (vector<Match*>::const_iterator
              it = matches.begin();
              it != matches.end();
              ++it)
      {
         Match* match = *it;
         Header* header = (Header*)match;
         headers.push_back(header);
      }
      return headers;
   }

};

}

#endif
