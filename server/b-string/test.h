#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "string.h"
#include "../test/test.h"

using namespace bee::fish::test;

namespace bee::fish::b_string
{
   inline bool testSplit();
   inline bool testBits();
   inline bool testBStrings();
   inline bool testHex();
   inline bool testData();
   inline bool testEmojis();
   
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testSplit();
      ok &= testBits();
      ok &= testBStrings();
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
   
   inline bool testBits()
   {
      cout << "Bit String 💗" << endl;
      
      bool ok = true;
      
      Data data = "💗";
      
      Bits bitString = 
         Bits::fromData(data);
      
      Data data2 = bitString.toData();
      
      ok &= testResult(
         "Bit String from data and back",
         (data2 == data)
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testBStrings()
   {
      cout << "B-Strings" << endl;
      
      bool ok = true;
      
      BString start = "Hello World";
      BString next = start;
      BString compare = "Hello World";
 
      ok &= testResult(
         "B-String compare",
         (next == compare)
      );
      
      cout << endl;
    
      return ok;
   }
   inline bool testHex()
   {
      cout << "Hex" << endl;
      
      bool ok = true;
      
      Data start = "Hello World";
      BString hex = start.toHex();
      BString compare = "48656c6c6f20576f726c64";

      ok &= testResult(
         "From Data to hex",
         (hex == compare)
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
      
      // std::string
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
      /*
      // BString
      BString bstring = "ᛒᚢᛞᛖ";
      Data dataFromBString = bstring;
      BString compare2 = dataFromBString;
      
      ok &= testResult(
         "From b-string to data and back",
         bstring == compare2
      );
      */
      Data dataStart = "Hello World";
      BString base64 = dataStart.toBase64();
      Data dataEnd = Data::fromBase64(base64);
      ok &= testResult(
         "From data to base64 and back",
         (dataStart == dataEnd)
      );
   
      std::string bstringMd5 = "Hello World";
      Data md5data = bstringMd5;
      BString md5hash = md5data.md5();
      
      ok &= testResult(
         "Compare md5 hash",
         md5hash == "b10a8db164e0754105b7a99be72e3fe5"
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
