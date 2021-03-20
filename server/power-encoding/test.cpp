#include <iostream>
#include "power-encoding.h"
#include "encoding.h"

using namespace std;

namespace bee::fish::power_encoding
{

bool test() {
   
   bool ok = true;
   
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
   encoding << "a";
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

   cerr << "4294967280L" << endl;
   encoding << 4294967280L;
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

   cerr << "🍄" << endl;
   encoding << "🍄";
   cerr << endl;
   cerr << "Count: " << encoding.count() << endl;

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
   
   enc << "🍄I love this planet";
   str.clear();
   enc >> str;
   cerr << str << endl;
   cerr << "Count: " << enc.count() << endl;
   
   if (str == "🍄I love this planet")
      cerr << "It works" << endl;
  
   cerr << "Basic authorization with wide characters"
        << endl;
   
   
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