#ifndef BEE_FISH_DATABASE__TEST_H
#define BEE_FISH_DATABASE__TEST_H

#include "../b-string/string.h"
#include "../test/test.h"
#include "database.h"
#include "path.h"


using namespace bee::fish::test;

namespace bee::fish::database
{
   inline bool testCreate();
   inline bool testWrite();
   inline bool testRead();
   inline bool testCompare();
   
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testCreate();
      ok &= testWrite();
      ok &= testRead();
      ok &= testCompare();
      
      testResult(
         "Remove file",
         (remove("test.data") == 0)
      );
      
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
      path << "Hello";
      
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
      path << "Hello";
      
      Path path2(test);
      path2 << "Hello";
      
      ok &= testResult(
         "Compare paths",
         (path == path2)
      );
      
      cout << endl;
      
      return ok;
   }


      
}

#endif
