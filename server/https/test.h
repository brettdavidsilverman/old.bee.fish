#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include <fstream>
#include <filesystem>
#include "../test/test.h"

#include "request.h"

using namespace std::filesystem;
using namespace bee::fish::test;

namespace bee::fish::https
{

   inline bool testRequest();

   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testRequest();

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testRequest()
   {
      cout << "Test request" << endl;
      
      bool ok = true;
     
      
      // open the sample session file
      path filePath("../https/request.txt");
      ifstream input(filePath, ifstream::binary);
      size_t size = file_size(filePath);
      std::string buffer(size, 0);
      input.read(&buffer[0], size);
      
      bee::fish::https::Request request;
      request.read(buffer, false);
      
      cerr <<"****" << request._result << endl;
      
      ok &= testResult(
         "Parse result",
         (request._result == true)
      );
      
      ok &= testResult(
         "Request method",
         (request.method() == "GET")
      );
      
      cout << endl;
      
      return ok;
   }


      
}

#endif
