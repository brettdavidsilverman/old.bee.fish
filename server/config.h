#ifndef BEE_FISH_SERVER__CONFIG_H
#define BEE_FISH_SERVER__CONFIG_H

#define BEE_FISH_DATABASE_FILE "/home/bee/bee.fish.data"
#define BEE_FISH_LOG_FILE "/home/bee/bee.fish.log"

//#define PI
#define LOCAL
/*
#ifdef PI

   #define KEY_FILE "/etc/ssl/certs/selfsigned.key"
   #define CERT_FILE "/etc/ssl/certs/selfsigned.crt"
   #define HOST_NAME "www.bee.fish"
*/
#ifdef LOCAL
   #define KEY_FILE "/home/bee/bee.fish/server/https/selfsigned.pem"
   #define CERT_FILE "/home/bee/bee.fish/server/https/selfsigned.crt"
   #define HOST_NAME "localhost"

#else
   #define KEY_FILE "/etc/letsencrypt/live/bee.fish/privkey.pem"
   #define CERT_FILE "/etc/letsencrypt/live/bee.fish/fullchain.pem"

   #define HOST_NAME "bee.fish"

#endif

#define FILE_SYSTEM_PATH "/home/bee/bee.fish"
#define TEMP_DIRECTORY "/home/bee/temp"

#define TMP_DH_FILE "dh2048.pem"

#endif
