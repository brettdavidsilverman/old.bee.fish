#include <iostream>
#include "power-encoding.h"
#include "encoding.h"
#include "../test/test.h"

using namespace std;
using namespace bee::fish::test;

namespace bee::fish::power_encoding
{

   inline bool test() {
   
      bool ok = true;
      
      StreamEncoding encodingOut(cin, cerr);

      for (int i = 0; i < 256; i++)
      {
         encodingOut.writeBit(true);
         encodingOut << (unsigned char)i;
         cerr << endl;
      }

      cerr << "Count: " << encodingOut.count() << endl;
      ok &= testResult("Number count", encodingOut.count() == 0);
  
      stringstream stream;
      StreamEncoding encoding(stream, stream);
      BString test;
      
      encoding << "Hello World";
      encoding >> test;
      ok &= testResult(
         "Hello World",
         ( test == "Hello World" ) &&
         ( encoding.count() == 0 )
      );
    
      test.clear();
      
      encoding << "🍄";
      encoding >> test;
      ok &= testResult(
         "String 🍄",
         ( test == "🍄" ) &&
         ( encoding.count() == 0 )
      );

 
      encoding.writeBit(true);
      encoding << 8;
      int i = 0;
      if (encoding.readBit())
         encoding >> i;
         
      ok &= testResult(
         "Number 8",
         ( i == 8 ) &&
         ( encoding.count() == 0 )
      );
      
      BitEncoding bitEncoding;
      bitEncoding.writeBit(true);
      bitEncoding << "🍄I love this planet";
      BString str;
      if (bitEncoding.readBit())
         bitEncoding >> str;
         
      ok &= testResult(
         "🍄I love this planet",
         ( str == "🍄I love this planet" )
      );

      bool testBool;

      encoding << true;
      encoding >> testBool;
      ok &= testResult(
         "True",
         testBool == true
      );
      
      
      encoding << false;
      encoding >> testBool;
      ok &= testResult(
         "False",
         testBool == false
      );
  
      if (ok)
         cerr << endl << "SUCCESS" << endl;
      else
         cerr << endl << "FAIL" << endl;
      
      return ok;
   
   }

}