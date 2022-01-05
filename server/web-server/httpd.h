#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string>
#include <stdio.h>

#define MAX_CLIENTS 10


//Server control functions

void startWebserver(const std::string& port);

// Client request

// user shall implement this function

void route();

#endif