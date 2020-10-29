#include <iostream>
#include "parser.h"
#include "../https/basic-authorization.h"

using namespace std;

namespace bee::fish::power_encoding
{

class Encoding : public PowerEncoding
{
protected:
   istream& _in;
   ostream& _out;
   long _count;
   
public:
   Encoding(istream& in, ostream& out) :
      _in(in),
      _out(out)
   {
      _count = 0;
   }
      
   virtual void writeBit(bool bit)
   {
#ifdef DEBUG
      cerr << 'w' << (bit ? '1' : '0');
#endif
      if (bit)
         ++_count;
      else
         --_count;
         
      _out << ( bit ? '1' : '0' );
   }
   
   virtual bool readBit()
   {
      if (_in.eof())
         throw runtime_error("End of input stream");
         
      char bit;
      _in >> bit;
      
#ifdef DEBUG
      cerr << 'r' << bit;
#endif

      bool b = ( bit != '0' );
      if (b)
         ++_count;
      else
         --_count;
         
      
      return b;
   }
   
   virtual bool peekBit()
   {
      return _in.peek() != '0';
   }
   
   long count()
   {
      return _count;
   }
   
};

bool test() {
   
   bool ok = true;
   
   class CharA : public Character
   {
   public:
      CharA() : Character('A') {
      }
      
      virtual ~CharA() 
      {
         cerr << "~CharA()";
      }
      
   };
   
   // Character
   CharA character;
   ok &= character.read("Ab");
   cerr << "Character:\t" 
        << ok
        << endl;
   
   character = CharA();
   ok &= (character.read("Ba") == false);
   cerr << "Character:\t" 
        << ok
        << character
        << endl;
   
   // Range
   Range range('a', 'z');
   ok &= range.read("abc");
   cerr << "Range:" << range << endl;
   cerr << ok << endl;

   
   // Word
   Word word("Brett");
   word.read("Bre", false);
   ok &= word.read("tt");
   cerr << "Word:" << word << endl;
   cerr << ok << endl;

   
   And sentence(
      new Word("Hello"),
      new Character(' '),
      new Word("World")
   );
   
   ok &= sentence.read("Hello World");
  
   cerr << "Sentence: " 
        << sentence << endl;
   cerr << ok << endl;
   
   // Case Insensitve Word
   CIWord ciword("Brett");
   ok &= ciword.read("breTT");
   cerr << "Case Insensitive Word: " << ciword << endl;
   cerr << ok << endl;
 
   // Or
   Or _or(
      new Word("Brett"),
      new Word("sad")
   );
   
   ok &= _or.read("sad");
   
   cerr << "Or:" << _or << endl;
   cerr << ok << endl;

   // Not
   Not _not1 (
      new Range('a', 'z')
   );
   
   ok &=_not1.read("A");
   cerr << "Not1:" << _not1 << endl;
   cerr << ok << endl;

   Not _not2(
      new Range('a', 'z')
   );
      
   _not2.read("a");
   if (_not2.success() == true)
      ok = false;
   cerr << "Not2:" << _not2 << endl;
   cerr << ok << endl;

   // Repeat
   
   class RepeatA : public Repeat<CharA>
   {
   public:
  
      virtual void addItem(Match* match)
      {
         Repeat::addItem(match);
      }
      
   } repeat;

   ok &= repeat.read("AAA");
   cerr << "Repeat:\t" << ok << endl;
 
   
   class BlankChar : public Or {
   public:
      BlankChar() : Or(
         new Character(' '),
         new Character('\t')
      )
      {
      }
   };


   And blanks(
      new Character('*'),
      new Repeat<BlankChar>(),
      new Character('*')
   );
      
   ok &= blanks.read("*   *");
   cerr << "Blanks:" << blanks << endl;
   cerr << ok << endl;

   // Optional

   And optional(
      new Word("Brett"),
      new Optional(
         new Repeat<BlankChar>()
      ),
      new Word("ABC")
   );
   
   ok &= optional.read("Brett   ABC");
   cerr << "Optional:" << optional << endl;
   cerr << "optional:" << 
      optional[1].value() << endl;
   cerr << ok << endl;
   
   class CharacterDot : public Character {
   public:
      CharacterDot() : Character('.')
      {}
   };
   
   And opt(
      new Optional(
         new Repeat<CharacterDot>()
      ),
      new Word("Brett")
   );
 
   ok &= opt.read("...Brett");
   cerr << opt << endl;
      std::stringstream stream1;
   Encoding encoding1(stream1, stream1);
   encoding1.operator <<  < int[3], int > ({0, 1, 2});

   cerr << endl;
   vector<int> a;
   encoding1 >> a;
   cerr << endl;
   
   for (auto v : a)
      cerr << v << endl;
   

   Encoding encoding(cin, cerr);
   
   for (int i = 0; i < 256; i++) {
      encoding << (unsigned char)i;
      cerr << endl;
   }

   cerr << "Count: " << encoding.count() << endl;
   
   cerr << "String a" << endl;
   encoding << "a";
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

   cerr << "WString a" << endl;
   encoding << L"a";
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

   cerr << "4294967280L" << endl;
   encoding << 4294967280L;
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

   wcerr << L"🍄" << endl;
   encoding << L"🍄";
   wcerr << endl;
   wcerr << "Count: " << encoding.count() << endl;

   stringstream stream;
   Encoding enc(stream, stream);
   
   cerr << "8 stream" << endl;
   enc << 8;
   cerr << endl;
   int i;
   enc >> i;

   cerr << i << endl;
   cerr << "Count: " << enc.count() << endl;

   enc << "Hello World";
   string str;
   enc >> str;
   cerr << str << endl;
   cerr << "Count: " << enc.count() << endl;
   
   enc << L"🍄I love this planet";
   wstring wstr;
   enc >> wstr;
   wcerr << wstr << endl;
   cerr << "Count: " << enc.count() << endl;
   
   if (wstr == L"🍄I love this planet")
      cerr << "It works" << endl;
  
   cerr << "Basic authorization with wide characters"
        << endl;
        
   BasicAuthorization basicAuth("Basic 8J+NhDpwYXNzd29yZA==");

   if (basicAuth.success())
   {
      throw "$";
      wcerr << basicAuth.username()
            << L":"
            << basicAuth.password() 
            << endl;
   }
   else
      ok = false;
      
   
   cerr << ok << endl;
   
   if (ok)
      cerr << endl << "SUCCESS" << endl;
   else
      cerr << endl << "FAIL" << endl;
      
   return ok;
   
}

}