#include "https-session.h"
#include <optional>
#include <mcheck.h>

using namespace Bee::Fish;

void https_session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (error) {
      delete this;
      return;
   }
   
   std::cout <<
      _data.substr(
         0,
         bytes_transferred
      );
      
   std::cout << "handle_read()"
             << bytes_transferred
             << std::endl;
    
   request->read(
      _data.substr(
         0,
         bytes_transferred
      ),
      (bytes_transferred < _max_length)
   );
      
   optional<bool> success =
      request->success();
        
   if (success == true) {
      std::cout << "ok joe" << std::endl;
      string path = request->path();
      if (path == "/?mtrace") {
         cout << "*mtrace*" << endl;
         //delete this;
         //return;
         mtrace();
      }
      
      if (path == "/?muntrace") {
         cout << "*muntrace*" << endl;
         //delete this;
         //return;
         muntrace();
      }
      response = new http_response(request);
   }
   else if (success == false) {
      // Parse error, drop the connection
      std::cout << "Fail!" << std::endl;
      delete this;
      return;
   }
   else {
      // Continue reading
      std::cout << "...Read More..." << std::endl;
      https_session::async_read();
      return;
   }
      
   std::cout
      << "'" << request->method() << "' "
      << "'" << request->path() << "' "
      << "'" << request->version() << "'"
      << std::endl;
   
   if (response && !response->end())
      https_session::async_write();

}

void https_session::handle_write(
   const boost::system::error_code& error
)
{
   if (error) {
      delete this;
      return;
   }
   
   if (response->end()) {
      delete response;
      response = NULL;
      this->start();
   }
   else
      async_write();
}
