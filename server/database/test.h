#ifndef BEE_FISH_DATABASE__TEST_H
#define BEE_FISH_DATABASE__TEST_H

#include "../b-string/string.h"
#include "../test/test.h"
#include "database.h"
#include "path.h"


using namespace BeeFishTest;

namespace BeeFishDatabase
{
   inline bool testCreate();
   inline bool testWrite();
   inline bool testRead();
   inline bool testCompare();
   inline bool testData();
   inline bool testInsert();
   inline bool testExtract();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testCreate();
      ok &= testWrite();
      ok &= testRead();
      ok &= testCompare();
      ok &= testData();
      
      testResult(
         "Remove file",
         (remove("test.data") == 0)
      );
      
      ok &= testCreate();
      ok &= testInsert();
      ok &= testExtract();
      
      remove("test.data");
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testCreate()
   {
      cout << "Create" << endl;
      
      bool ok = true;
      
      remove("test.data");
      
      Database test("test.data");
      
      ok &= testResult(
         "Test filename",
         (test.filename() == "test.data")
      );
      
      test.close();
      
      return ok;
   }
   
   inline bool testWrite()
   {
      cout << "Write" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      path = path["Hello"];
      
      ok &= testResult(
         "Write Hello",
         (path.isDeadEnd())
      );
      
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testRead()
   {
      cout << "Read" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      
      ok &= testResult(
         "Read Hello",
         (path.contains("Hello"))
      );
      
      
      cout << endl;
    
      return ok;
   }
   
   inline bool testCompare()
   {
      cout << "Compare" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      path = path["Hello"];
      
      Path path2(test);
      path2 = path2["Hello"];
      
      ok &= testResult(
         "Compare paths",
         (path == path2)
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testData()
   {
      cout << "Data" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      path = path["Hello"];
      path.setData("hello");
      BString compare;
      path.getData(compare);
         
      ok &= testResult(
         "Compare data",
         (
            compare == "hello"
         )
      );
      

      path = Path(test);
      path = path["World"];
      Data data = "Hello World";
      path.setData(data);
      Data compare2;
      path.getData(compare2);
         
      ok &= testResult(
         "Compare data",
         (
            compare2 == Data("Hello World")
         )
      );

      cout << endl;
      
      return ok;
   }
      
   inline bool testInsert()
   {
      cout << "Insert" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      Path root(path);
      
      path << BString("123.45");
      
      ok &= testResult(
         "Inset number",
         (path.isDeadEnd())
      );
      
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testExtract()
   {
      cout << "Extract" << endl;
      
      bool ok = true;
      
      Database test("test.data");
      
      Path path(test);
      
      BString number;
      
      path >> number;
      
      ok &= testResult(
         "Read Number",
         number == "123.45"
      );
      
      ok &= testResult(
         "After read number",
         (path.isDeadEnd())
      );
      
      cout << endl;
    
      return ok;
   }
}

#endif
