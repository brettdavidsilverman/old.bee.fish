#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string>
#include <stdio.h>

#define MAX_CLIENTS 1000


//Server control functions

void serve_forever(const std::string& port);

// Client request

// user shall implement this function

void route();

#endif