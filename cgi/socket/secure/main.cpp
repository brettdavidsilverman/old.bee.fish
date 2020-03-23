#include <limits.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "server.h"

std::string get_real_path(const std::string path);

int main(int argc, char **argv)
{
   std::cout << __cplusplus << std::endl;
   std::cerr << "Check error redirect" << std::endl;
   
   try {
      std::string path = ".";
      int thread_count = 10;
   
      if (argc > 1)
         path = argv[1];
   
      if (argc > 2)
         thread_count = atoi(argv[2]);
      
      std::string root_path =
         get_real_path(path);
      
      Server server(443, root_path, thread_count);
      server.wait();
   }
   catch (const char* error) {
      std::cerr << error << std::endl;
   }
   
   std::cout << "Finished" << std::endl;
   
   return 0;
}

std::string get_real_path(const std::string path) {

   char resolved_path[PATH_MAX];
   
   realpath(
      path.c_str(),
      resolved_path
   );
   
   std::string real_path = std::string(resolved_path);
   if (boost::ends_with(real_path, "/") == false)
      real_path += "/";
      
   return real_path;
}

