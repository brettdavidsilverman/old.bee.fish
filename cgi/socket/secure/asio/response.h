#include "session.h"
#include "request.h"

using namespace std;

namespace bee::fish::server {

class response {
public:
   response(
      bee::fish::server::session* session,
      bee::fish::server::request* request
   );
   
   string write(size_t length);
   
   bool end();
   
private:
   string _response;
   size_t _bytes_transferred = 0;
};

};
