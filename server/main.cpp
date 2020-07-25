#include <mcheck.h>
#include "server.h"
#include "version.h"
#include "md5.h"
#include "base64.h"
#include "id.h"

int main(int argc, char* argv[])
{
   try
   {
   
      std::cout << "HTTPS Secure Server ASIO" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_SERVER__BUILD_DATE
           << std::endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_SERVER__BUILD_NUMBER
           << std::endl;
      
      if (argc != 2)
      {
         std::cerr << "Usage: server <port>\n";
         return 1;
      }
       
      Id id;
      
      boost::asio::io_context io_context;

      bee::fish::server::Server
         server(
            "bee.fish",
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