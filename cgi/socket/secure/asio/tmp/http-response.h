#include "session.h"

using namespace std;

namespace Bee::Fish {

class https_response {
public:
   https_response(
      session* session,
      request* request)
   {
      
      string echo_headers;
      vector<Header*> headers =
         request->headers();
   
      for (vector<Header*>::iterator
           it = headers.begin();
           it != headers.end();
           ++it)
      {
         Header* header = *it;
         echo_headers += header->name()
                   + '\t'
                   + header->value()
                   + "\r\n";
      }
      
      _response =
      //"HTTP/1.1 200 OK\r\n";
       "HTTP/1.1 401 Unauthorized\r\n";
      _response +=
         "content-type: text/plain\r\n";
      _response +=
         "content-length: " +
         std::to_string(echo_headers.length()) +
         "\r\n";
      _response += 
         "connection: keep-alive\r\n";
      _response +=
         "Access-Control-Allow-Origin: http://localhost:8000\r\n";
      _response +=
         "Access-Control-Allow-Credentials: true\r\n";
      _response +=
         "WWW-Authenticate: Basic realm=\"https://bee.fish\"\r\n";
      //_response +=
      //   "WWW-Authenticate: Digest realm=\"bee@bee.fish\"\r\n";
      _response += 
         "\r\n";
      _response += echo_headers;
   }
   
   string write(size_t length) {
   
      if (_bytes_transferred + length >
         _response.length())
         length = _response.length() -
            _bytes_transferred;
            
      string retval =
         _response.substr(
            _bytes_transferred,
            length
         );
         
      _bytes_transferred += length;
      
      return retval;
   }
   
   bool end() {
      return
         _bytes_transferred >=
         _response.length();
   }
   
private:
   string _response;
   size_t _bytes_transferred = 0;
};

}