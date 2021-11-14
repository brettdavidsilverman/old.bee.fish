#include <iostream>
#include "../test/test.h"
#include "../b-string/string.h"
#include "power-encoding.h"
#include "encoding.h"


using namespace std;
using namespace BeeFishTest;

namespace BeeFishPowerEncoding
{

   inline bool test() {
   
      bool ok = true;
      
      EncodeToStream encodingOut(cin, cout);

      for (int i = 0; i < 256; i++)
      {
         encodingOut.writeBit(true);
         encodingOut << (unsigned char)i;
         cout << endl;
      }

      cout << "Count: " << encodingOut.count() << endl;
      ok &= testResult("Number count", encodingOut.count() == 0);
  
      stringstream strstream;
      EncodeToStream encoding(strstream, strstream);
      BString test;
      
      encoding << "Hello Bee";

      ok &= testResult(
         "Hello Bee write Count",
         ( encoding.count() == 0 )
      );

      encoding >> test;

      cout << "Encoding count: " << encoding.count() << endl;

      ok &= testResult(
         "Hello Bee",
         ( test == "Hello Bee" )
      );

      ok &= testResult(
         "Hello Bee read Count",
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
      
      EncodeToBits bitEncoding;
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
  
      BString start = "ABC Hello World";
      BitStream stream;
      stream << start;
      stream.reset();
      BString finish;
      stream >> finish;
      
      ok &= testResult(
         "Bit Stream",
         start == finish
      );
      
      cout << finish << endl;
      cout << stream.toData().toBase64() << endl;
      
      if (ok)
         cout << endl << "SUCCESS" << endl;
      else
         cout << endl << "FAIL" << endl;
      
      return ok;
   
   }

}