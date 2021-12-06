#ifndef BEE_FISH_SERVER__CONFIG_H
#define BEE_FISH_SERVER__CONFIG_H

#define BEE_FISH_SERVER_LOG_FILE "/home/bee/server.log"
#define BEE_FISH_SERVER_ERR_FILE "/home/bee/server.err"

#define BEE_FISH_DATABASE_FILE "/home/bee/bee.fish.data"
#define BEE_FISH_TRANSACTION_FILE "/home/bee/bee.fish.transaction"

#if __has_include("../config.h")
   #include "../config.h"
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
#if !defined(LOCAL) && !defined(LIVE)
   #define LOCAL
#endif

#ifdef LOCAL
   #define KEY_FILE  "/home/bee/bee.fish/server/https/selfsigned.key"
   #define CERT_FILE "/home/bee/bee.fish/server/https/selfsigned.crt"
   #define HOST_NAME "laptop"

#elif defined(LIVE)
   #define KEY_FILE "/etc/letsencrypt/live/bee.fish/privkey.pem"
   #define CERT_FILE "/etc/letsencrypt/live/bee.fish/fullchain.pem"

   #define HOST_NAME "bee.fish"
#else
   #error "No certificates installed"
#endif

#ifdef SERVER
   #define FILE_SYSTEM_PATH "/home/bee/bee.fish"
   #define TEMP_DIRECTORY "/var/tmp"
#endif

#define TMP_DH_FILE "dhparam.pem"



#endif
