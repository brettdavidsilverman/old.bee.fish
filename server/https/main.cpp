#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>

#ifdef SERVER

#include <linux/limits.h>

#include "https.h"
#include "test.h"

using namespace BeeFishDatabase;
using namespace BeeFishHTTPS;


int main(int argc, const char* argv[])
{

/*
   cout << "Standard out" << endl;
   cerr << "Standard err" << endl;
   clog << "Standard log" << endl;

   return 0;
*/
   try
   {
   
      BString databaseFile =
         File::getFullPath(
            BEE_FISH_DATABASE_FILE
         );
         
      BString logFile =
         File::getFullPath(
            BEE_FISH_LOG_FILE
         );
      
      appFactories.add<HTTPSAuthentication>();
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
        << "Log file: "
           << logFile.toUTF8()
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
      
      BeeFishHTTPS::Server
         server
         (
            HOST_NAME,
            databaseFile,
            logFile,
            io_context,
            port
         );

      io_context.run();
      
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
      return -1;
   }

   return 0;
}

#else
int main(int argc, const char* argv[])
{
   std::cerr << "Https only runs on server" << std::endl;
   return 0;
}
#endif
