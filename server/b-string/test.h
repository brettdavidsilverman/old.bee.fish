#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "string.h"
#include "../test/test.h"

using namespace bee::fish::test;

namespace bee::fish::b_string
{
   inline bool testCharacters();
   inline bool testBitStream();
   inline bool testBStrings();
   inline bool testSplit();
   inline bool testTrim();
   inline bool testHex();
   inline bool testData();
   inline bool testEmojis();
   
   
   inline bool test()
   {
   
      bool ok = true;
     
      ok &= testCharacters();
      ok &= testBitStream();
      ok &= testBStrings();
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
      Character::Value value = c;
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
   
   inline bool testBitStream()
   {
      cout << "Bit Stream" << endl;
      
      bool ok = true;
      
      BitStream writeStream;
      writeStream.writeBit(1);
      
      Data& data = writeStream._data;
       ok &= testResult(
         "Write bit",
         (data.size() == 1 &&
          data[0] == 0b10000000)
      );
      
      BitStream readStream(data);
      bool bit = readStream.readBit();
      
      ok &= testResult(
         "Write/Read bit",
         (bit == true)
      );
      
      Data testData;
      BitStream stream1(testData);

      BString actual = "Hello World";
      stream1 << actual;
  
      BitStream stream2(testData);
      BString compare;
      stream2 >> compare;

      cerr << compare;
      
      bool result =
          (compare == actual);
      cerr << "*3";
      ok &= testResult(
         "Transfer data from stream",
         result
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
      
      Data data = start.toData();
      
      compare = BString(data);
      ok &= testResult(
         "B-String from data compare",
         (start == compare)
      );
      
      
      
      cout << endl;
    
      return ok;
   }
   
   inline bool testSplit()
   {
      cout << "Split" << endl;
      
      bool ok = true;
      cerr << "Here 0" << endl;
      BString test = "a:b:c";
      cerr << "Here 1" << endl;
      
      vector<BString> split = test.split(':');
      cerr << "Here 2" << endl;
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
      
      Data data = "ᛒᚢᛞᛖ";
      
      BString bstring(data);

      ok &= testResult(
         "From data to bstring",
         bstring == "ᛒᚢᛞᛖ"
      );
     
      Data compare = bstring;
      ok &= testResult(
         "From bstring to data",
         data == compare
      );
      
      unsigned long ulong = 101;
      Data ulongData(ulong);
      unsigned long ulongCompare =
         (unsigned long)ulongData;
      ok &= testResult(
         "From unsigned long to data and back",
         ulongCompare == ulong
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
         md5hash.size()// == "b10a8db164e0754105b7a99be72e3fe5"
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
