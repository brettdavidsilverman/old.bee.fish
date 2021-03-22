#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "b-string.h"
#include "bit-string.h"
#include "data.h"
#include "../test/test.h"

using namespace bee::fish::test;

namespace bee::fish::b_string
{
   inline bool testSplit();
   inline bool testBitString();
   inline bool testHex();
   inline bool testData();
   inline bool testEmojis();
   
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testSplit();
      ok &= testBitString();
      ok &= testHex();
      ok &= testData();
      ok &= testEmojis();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testSplit()
   {
      cout << "Split" << endl;
      
      bool ok = true;
      
      BString test = "a:b:c";
      vector<BString> split = test.split(':');

      ok &= testResult(
         "Split abc",
            (split.size() == 3) &&
            (split[0] == "a")   &&
            (split[1] == "b")   &&
            (split[2] == "c")
      );
         
      cout << endl;
      
      return ok;
   }
   
   inline bool testBitString()
   {
      cout << "Bit String" << endl;
      
      bool ok = true;
      
      Data data = "ABC";
      
      BitString bitString = 
         BitString::fromData(data);
      
      BitString testBitString =
         BitString::fromBits(bitString.bits());
      
      ok &= testResult(
         "Bit String from data",
         (testBitString.data() == data)
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testHex()
   {
      cout << "Hex" << endl;
      
      bool ok = true;
      
      Char startChar = 'b';
      BString hexChar = startChar.toHex();
      Char compareChar = Char::fromHex(hexChar);
      ok &= testResult(
         "From Char to hex and back",
         (startChar == compareChar)
      );
      
      BString startString = "Hello World";
      BString hexString = startString.toHex();
      BString compareString = BString::fromHex(hexString);

      ok &= testResult(
         "From string to hex and back",
         (startString == compareString)
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testData()
   {
      cout << "Data" << endl;
      
      bool ok = true;
      
      Data data = "ᛒᚢᛞᛖ";
      //Data data = "abc";
      
      std::string string = data;
      ok &= testResult(
         "From data to string",
         string == "ᛒᚢᛞᛖ"
      );
     
      Data compare = string;
      ok &= testResult(
         "From string to data",
         data == compare
      );
      
      Data dataStart = "ᛒᚢᛞᛖ";
      Data base64 = dataStart.toBase64();
      Data dataEnd = Data::fromBase64(base64);
      ok &= testResult(
         "From data to base64 and back",
         dataStart == dataEnd
      );
 
      cout << endl;
      
      return ok;
   }
   
   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      std::string str = "Emoji 😀";
      BString bstr(str);
      std::string str2 = bstr;
      
      ok &= testResult(
         "Emoji 😀",
         (str == str2)
      );
      
      cout << endl;
      
      return ok;
   }

      
}

#endif
