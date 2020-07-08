#include "https-session.h"


void https_session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (!error) {
      std::cout << _data.substr(
            0,
            bytes_transferred
         );
      
      std::cout << "handle_read()" << std::endl;
    
      http_request->read(
         _data.substr(
            0,
            bytes_transferred
         )
      );
      
      if (http_request->success() == true)
         std::cout << "ok joe" << std::endl;
      else
         std::cout << "Fail!" << std::endl;
         
      std::cout
        // << "'" << https_reader.method << "' "
         << "Path: '" << http_request->path() << "' "
       //  << "'" << https_reader.query << "' "
       //  << "'" << https_reader.version << "'"
         << std::endl;
      
      std::string response =
        "HTTP/1.1 200 OK\r\n";
        // "HTTP/1.1 401 Unauthorized\r\n";
      response +=
         "content-type: text/plain\r\n";
      response +=
         "content-length: " +
            std::to_string(bytes_transferred) +
            "\r\n";
      response += 
         "connection: keep-alive\r\n";
      response +=
         "Access-Control-Allow-Origin: *\r\n";
      response +=
         "WWW-Authenticate: Digest realm=\"bee@bee.fish\"\r\n";
      response += 
         "\r\n";
     
      boost::asio::write(
         *this,
         boost::asio::buffer(
            response,
            response.length()
         )
      );
        
      
      boost::asio::async_write(
         *this,
         boost::asio::buffer(
            _data,
            bytes_transferred
         ),
         boost::bind(
            &https_session::start,
            this
         )
      );
   }
   else
      delete this;
}
