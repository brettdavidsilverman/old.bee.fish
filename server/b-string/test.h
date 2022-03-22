#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "string.h"
#include "../test/test-result.h"

using namespace BeeFishTest;

namespace BeeFishBString
{
   inline bool testCharacters();
   inline bool testBStrings();
   inline bool testBStringStreams();
   inline bool testSplit();
   inline bool testTrim();
   inline bool testHex();
   inline bool testData();
   inline bool testEmojis();
   
   
   inline bool test()
   {
   
      bool ok = true;
     
      ok &= testCharacters();
      ok &= testBStrings();
      ok &= testBStringStreams();
      
      ok &= testSplit();
      ok &= testTrim();
      ok &= testHex();
      ok &= testData();
      ok &= testEmojis();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   
   inline bool testCharacters()
   {
      cout << "Characters" << endl;
      
      bool ok = true;
      
      Character c = 'a';
      Character value = c;
      ok &= testResult(
         "From value to character and back",
         ( value == 'a' )
      );
      
      Character b = c;
      ok &= testResult(
          "Copy and compare character",
          ( b == c )
      );
      
      Character d = 'z';
      d = b;
      ok &= testResult(
          "Assign and compare character",
          ( d == b )
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
      
      Character at = Character(0x0040);
      BString value = at;

      ok &= testResult(
         "B-String character @",
         (value == Character('@'))
      );

      BString bstr2 = "from Bee";
      BString bstr = "Hello " + bstr2;

      cout << "bstr::" << bstr << endl;

      ok &= testResult(
         "Const chart + bstr",
         (bstr == BString("Hello from Bee"))
      );
/*      
      BString empty = "";
      ok &= testResult(
         "BString as boolean false",
         (empty) == false
      );

      BString full = "full";
      ok &= testResult(
         "BString as boolean true",
         (full) == true
      );
 */ 
      cout << endl;

      return ok;
   }
   
   inline bool testBStringStreams()
   {
      cout << "B-String-Streams" << endl;
      
      bool ok = true;
      
      BStream stream;
      BString value;
      stream._onbuffer = [&value](const Data& buffer) {
         std::string string((const char*)buffer.data(), buffer.size());
         value = string;
      };

      stream << "Hello World";

      stream.flush();

      ok &= testResult(
         "B-String stream",
         (value == "Hello World")
      );

      cout << endl;

      return ok;
      
   }

   inline bool testSplit()
   {
      cout << "Split" << endl;
      
      bool ok = true;
      cout << "Here 0" << endl;
      BString test = "a:b:c";
      cout << "Here 1" << endl;
      
      vector<BString> split = test.split(':');
      cout << "Here 2" << endl;
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
   
   
   inline bool testTrim()
   {
      cout << "Trim" << endl;
      
      bool ok = true;
      
      BString sample = "   Hello World   ";
      
      ok &= testResult(
         "L-Trim",
         (sample.ltrim() == "Hello World   ")
      );
      
      ok &= testResult(
         "R-Trim",
         (sample.rtrim() == "   Hello World")
      );
      
      ok &= testResult(
         "Trim",
         (sample.trim() == "Hello World")
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
	      
      ok &= testResult(
         "From Data to hex",
         (hex.size())
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testData()
   {
      cout << "Data" << endl;
      
      bool ok = true;
     
     
      unsigned long ulong = 101;
      Data ulongData(ulong);
      unsigned long ulongCompare =
         (unsigned long)ulongData;
      ok &= testResult(
         "From unsigned long to data and back",
         ulongCompare == ulong
      );
      
      // BString
      BString bstring = "ᛒᚢᛞᛖ";
      Data dataFromBString = bstring;
      BString compare2 = dataFromBString;
      
      ok &= testResult(
         "From b-string to data and back",
         bstring == compare2
      );
      
      Data dataStart = "Hello World";
      BString base64 = dataStart.toBase64();
      Data dataEnd = Data::fromBase64(base64);
      ok &= testResult(
         "From data to base64 and back",
         (dataStart == dataEnd)
      );
   
   #ifdef SERVER
      std::string stringMd5 = "Hello World";
      Data md5data = stringMd5;
      BString md5hash = md5data.md5();

      ok &= testResult(
         "Compare md5 hash",
         (md5hash == "b10a8db164e0754105b7a99be72e3fe5")
      );
   #endif
   
      
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
