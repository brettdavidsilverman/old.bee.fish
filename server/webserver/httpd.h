#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string.h>
#include <stdio.h>

//Server control functions

void serve_forever(const char *PORT);

// Client request

// user shall implement this function

void route();

#endif