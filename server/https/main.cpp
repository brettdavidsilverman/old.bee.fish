#include <stdlib.h> 
#include <stdio.h> 
#include <linux/limits.h>
#include "https.h"
#include "test.h"

using namespace bee::fish::database;
using namespace bee::fish::https;

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
);


int main(int argc, const char* argv[])
{
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
      
      appFactories.add<Authentication>();
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
           << databaseFile
           << std::endl
        << "Log file: "
           << logFile
           << std::endl
        << "Host: "
           << HOST_NAME
           << std::endl;
 
      if (hasArg(argc, argv, "-test") >= 0)
      {
         cout << "Testing..." << endl;
         if (!bee::fish::https::test())
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
      
      bee::fish::https::Server
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

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return i;
   }
   
   return -1;
}