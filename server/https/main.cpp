#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>
#include "https.h"

#ifdef SERVER

#include <linux/limits.h>

#include "test.h"

using namespace BeeFishDatabase;
using namespace BeeFishHTTPS;

int main(int argc, const char* argv[])

{
  
   BeeFishHTTPS::Request postRequest;
   
   // Post with anything but JSON is not allowed....
   bool ok = true;
   ok &= testFile(
      "Post with encoded name value pairs",
      "server/https/tests/post.txt",
      postRequest,
      false
   );
   cout << "Post Request: " << postRequest.result()  << endl;

   return 0;

   try
   {
      initializeLogs();
   
      BString databaseFile =
         File::getFullPath(
            BEE_FISH_DATABASE_FILE
         );
         
      BString transactionFile =
         File::getFullPath(
            BEE_FISH_TRANSACTION_FILE
         );
      
      appFactories.add<AuthenticationApp>();
      appFactories.add<StorageApp>();
      appFactories.add<FileSystemApp>();
      
      
      
      std::cout << "HTTPS Secure Server" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Version: "
           << BEE_FISH_HTTPS_VERSION
           << std::endl
        << "Database file: "
           << databaseFile.toUTF8()
           << std::endl
        << "Transaction file: "
           << transactionFile.toUTF8()
           << std::endl
        << "Host: "
           << HOST_NAME
           << std::endl;

      if (hasArg(argc, argv, "-test") >= 0)
      {
         cout << "Testing HTTPS..." << endl;
         if (!BeeFishHTTPS::test())
            return 1;
            
         return 0;
      }
      
      unsigned int port;
      int portArg;
      if ((portArg = hasArg(argc, argv, "-port")) >= 0)
      {
         
         port = std::atoi(argv[portArg + 1]);
      }
      else
         port = 443;
      
      
      boost::asio::io_context io_context;

      std::cout << "Starting on port " << port << "..." << endl;

      BeeFishHTTPS::Server
         server
         (
            HOST_NAME,
            databaseFile,
            transactionFile,
            io_context,
            port
         );

      io_context.run();
      
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << std::endl;
      return -1;
   }
   catch(...) {
      std::cerr << "Unkown exception" << std::endl;
   }

   cerr << "main shoudln't quit" << endl;
   
   return 0;
}

#else
int main(int argc, const char* argv[])
{
   std::cerr << "Https only runs on server" << std::endl;
   return 0;
}
#endif

