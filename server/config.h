#ifndef BEE_FISH_SERVER__CONFIG_H
#define BEE_FISH_SERVER__CONFIG_H

#define BEE_FISH_DATABASE_FILE "/home/bee/bee.fish.data"
#define BEE_FISH_LOG_FILE "/home/bee/bee.fish.log"

#if __has_include("../../config.h")
   #include "../../config.h"
#endif

#define CLIENT

//#define PI
//#define LOCAL

//#define DISABLE_CACHE

/*
#ifdef PI

   #define KEY_FILE "/etc/ssl/certs/selfsigned.key"
   #define CERT_FILE "/etc/ssl/certs/selfsigned.crt"
   #define HOST_NAME "www.bee.fish"
*/
#ifdef LOCAL
   #define KEY_FILE "/home/bee/bee.fish/server/https/selfsigned.pem"
   #define CERT_FILE "/home/bee/bee.fish/server/https/selfsigned.crt"
   #define HOST_NAME "laptop.bee.fish"

#elif defined(LIVE)
   #define KEY_FILE "/etc/letsencrypt/live/bee.fish/privkey.pem"
   #define CERT_FILE "/etc/letsencrypt/live/bee.fish/fullchain.pem"

   #define HOST_NAME "bee.fish"
#else
   #error "No certificates installed"
#endif

#define FILE_SYSTEM_PATH "/home/bee/bee.fish"
#define TEMP_DIRECTORY "/var/tmp"

#define TMP_DH_FILE "dh2048.pem"



#endif
