#include <iostream>
#include "parser.h"
#include "../power-encoding/power-encoding.h"
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
   
   
   
   std::stringstream stream1;
   Encoding encoding1(stream1, stream1);
   /*
   encoding1.operator <<  < int[3], int > ({0, 1, 2});

   cerr << endl;
   vector<int> a;
   encoding1 >> a;
   cerr << endl;
   
   for (auto v : a)
      cerr << v << endl;
   
   */
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