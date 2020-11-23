#include <stdlib.h> 
#include <stdio.h> 
#include <linux/limits.h>
#include "server.h"
#include "version.h"
#include "wstring.h"
#include "config.h"

using namespace bee::fish::server;

int main(int argc, char* argv[])
{
   try
   {
      char buffer[PATH_MAX];
      char* databaseFile =
         realpath(DATABASE_FILE, buffer);
      
      std::cout << "HTTPS Secure Server" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Version: "
           << BEE_FISH_SERVER_HTTPS_VERSION
           << std::endl
        << "Database file: "
           << databaseFile
           << std::endl;
           
        bee::fish::parser::json::String::write(std::cout, HOST_NAME);
        std::cout << std::endl;
 
      
      if (argc != 2)
      {
         std::cerr << "Usage: server <port>\n";
         return 1;
      }
      
      unsigned int port = std::atoi(argv[1]);
      
      boost::asio::io_context io_context;

      bee::fish::server::Server
         server
         (
            HOST_NAME,
            databaseFile,
            io_context,
            port
         );

      io_context.run();
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
      
   }

   return 0;
}
