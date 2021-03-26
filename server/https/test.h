#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include <fstream>
#include "../test/test.h"

#include "request.h"
#include "basic-authorization.h"


using namespace bee::fish::test;

namespace bee::fish::https
{

   inline bool testRequest();
   inline bool testBasicAuthorization();

   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testRequest();
      ok &= testBasicAuthorization();
      
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
      ifstream input("../https/sample-session.txt");
      
      bee::fish::https::Request request;
      
      ok &= testResult(
         "Parse result",
         (request.read(input, true) == true)
      );
      
      ok &= testResult(
         "Request method",
         (request.method() == "GET")
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testBasicAuthorization()
   {
      cout << "Test basic authorization" << endl;
      
      bool ok = true;
     
      // open the sample session file
      ifstream input("../https/sample-basic-authorization.txt");
      BString line;

      ok &= testResult(
         "Read line",
         getline(input, line)
      );
      
      input.close();
      
      bee::fish::https::BasicAuthorization
         authorization(line);
         
      ok &= testResult(
         "Parse basic authorization",
         (authorization._result == true)
      );
     
      if (ok)
         ok &= testResult(
            authorization._credentials->_username + " check username",
            (authorization._credentials->_username == "🍄 🍌")
         );
      
      
      cout << endl;
      
      return ok;
   }
   

      
}

#endif
