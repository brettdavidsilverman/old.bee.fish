#include <mcheck.h>
#include "server.h"
#include "version.h"
#include "wstring.h"
#include "config.h"

int main(int argc, char* argv[])
{
   try
   {
   
      std::cout << "HTTPS Secure Server ASIO" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Version: "
           << BEE_FISH_SERVER_HTTPS_VERSION
           << std::endl;
        bee::fish::parser::json::String::write(std::cout, HOST_NAME);
        std::cout << std::endl;
 
      
      if (argc != 2)
      {
         std::cerr << "Usage: server <port>\n";
         return 1;
      }
      
           
      boost::asio::io_context io_context;

      bee::fish::server::Server
         server
         (
            HOST_NAME,
            io_context,
            std::atoi(argv[1])
         );

      io_context.run();
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
      
   }

   return 0;
}