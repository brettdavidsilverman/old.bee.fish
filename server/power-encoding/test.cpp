#include <iostream>
#include "../parser/parser.h"
#include "power-encoding.h"
#include "encoding.h"
#include "../https/basic-authorization.h"

using namespace std;

namespace bee::fish::power_encoding
{

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

      cerr << basicAuth.username()
           << ":"
           << basicAuth.password() 
           << endl;
   }
   else
      ok = false;
      
   Encoding enc3(cin, cerr);
   
   cerr << "true:";
   enc3 << true;
   cerr << endl;
   
   cerr << "false:";
   enc3 << false;
   cerr << endl;
   
   
   cerr << ok << endl;
   
   if (ok)
      cerr << endl << "SUCCESS" << endl;
   else
      cerr << endl << "FAIL" << endl;
      
   return ok;
   
}

}