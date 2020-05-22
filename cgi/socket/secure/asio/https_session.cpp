#include "https_session.h"


void https_session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (!error) {
  
      std::cout << "handle_read()" << std::endl;
      
      reader::read(
         data_.substr(
            0,
            bytes_transferred
         )
      );
      
      std::cout
         << "'" << https_session::method_ << "' "
         << "'" << https_session::path_ << "' "
         << "'" << https_session::query_ << "' "
         << "'" << https_session::version_ << "'"
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
            data_,
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
