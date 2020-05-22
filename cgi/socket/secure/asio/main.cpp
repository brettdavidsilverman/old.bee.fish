#include "version.h"
#include "server.h"

int main(int argc, char* argv[])
{
   try
   {
      std::cout << "HTTPS Secure Server ASIO" << std::endl;
      std::cout << "Version: " << VERSION << std::endl;
      
      if (argc != 2)
      {
         std::cerr << "Usage: server <port>\n";
         return 1;
      }

      boost::asio::io_context io_context;

      server s(io_context, std::atoi(argv[1]));

      io_context.run();
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
   }

   return 0;
}