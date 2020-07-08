#include <parser.h>

using namespace Bee::Fish::Parser;

namespace Bee::Fish {

class NewLine : public And {
public:
   NewLine() :
      And(
         new Character('\r'),
         new Character('\n')
      )
   {
   }
};

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

class Verb : public Or {
public:
   Verb() :
      Or(
         new Word("GET"),
         new Word("PUT"),
         new Word("POST"),
         new Word("OPTIONS")
      )
   {
   }
};

class FirstLine : public And {
public:
   FirstLine() :
      And(
         new Verb(),
         new Whitespace(),
         new Path(),
         new Whitespace(),
         new Version(),
         new NewLine()
      )
   {
   }
   
   virtual string path() {
      return items()[2]->value();
   }
};

class http_request : public And {
public:
   http_request() :
      And(
         new FirstLine()
         //new Repeat<HeaderLine>(),
        // new NewLine()
      )
   {
   }
   
   FirstLine* firstLine() {
      return (FirstLine*)(items()[0]);
   }
   
   virtual string path() {
      return firstLine()->path();
   }
};

}
