#ifndef BEE_FISH_SERVER__RESPONSE_H
#define BEE_FISH_SERVER__RESPONSE_H

#include "session.h"
#include "request.h"

using namespace std;

namespace bee::fish::https {

class Response {
public:
   Response(
      Session* session
   );
   
   string write(size_t length);
   
   bool end();
   
private:
   string _response;
   size_t _bytesTransferred = 0;
};

};

#endif
