#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string.h>
#include <stdio.h>

#define MAX_CLIENTS 1000


//Server control functions

void serve_forever(const char *PORT);

// Client request

// user shall implement this function

void route();

#endif