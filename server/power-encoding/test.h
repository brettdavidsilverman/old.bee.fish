#include <iostream>
#include "../test/test.h"
#include "../b-string/string.h"
#include "power-encoding.h"
#include "encoding.h"


using namespace std;
using namespace bee::fish::test;

namespace bee::fish::power_encoding
{

   inline bool test() {
   
      bool ok = true;
      
      EncodeToStream encodingOut(wcin, wcerr);

      for (int i = 0; i < 256; i++)
      {
         encodingOut.writeBit(true);
         encodingOut << (unsigned char)i;
         wcerr << endl;
      }

      wcerr << "Count: " << encodingOut.count() << endl;
      ok &= testResult("Number count", encodingOut.count() == 0);
  
      wstringstream strstream;
      EncodeToStream encoding(strstream, strstream);
      BString test;
      
      encoding << "Hello World";
      encoding >> test;
      wcerr << encoding.count() << endl;
      ok &= testResult(
         "Hello World",
         ( test == L"Hello World" ) &&
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
      
      wcerr << finish << endl;
      cerr << stream.toData() << endl;
      
      if (ok)
         wcerr << endl << "SUCCESS" << endl;
      else
         wcerr << endl << "FAIL" << endl;
      
      return ok;
   
   }

}