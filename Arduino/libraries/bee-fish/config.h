#pragma once

#ifdef SERVER
#undef SERVER
#endif

//#define DEBUG

#ifndef DEBUG
#define LIVE
#endif

//#define DEBUG_PARSER

#ifdef DEBUG
   #define HOST_NAME "LAPTOP"
#else
   #define HOST_NAME "bee.fish"
#endif

#define HOST "https://" HOST_NAME


