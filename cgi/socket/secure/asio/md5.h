#ifndef BEE_SERVER__MD5_H
#define BEE_SERVER__MD5_H
#include <iostream>
#include <iomanip>
#include <openssl/md5.h>

namespace bee::fish::server {

std::string md5(const std::string & str);

}

#endif