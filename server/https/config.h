#ifndef BEE_FISH_SERVER__CONFIG_H
#define BEE_FISH_SERVER__CONFIG_H

#define DATABASE_FILE "/home/bee/bee.fish.data"

//#define PI
//#define ANDROID

#ifdef PI

   #define KEY_FILE "/etc/ssl/certs/selfsigned.key"
   #define CERT_FILE "/etc/ssl/certs/selfsigned.crt"
   #define HOST_NAME L"www.bee.fish"

#else
#ifdef ANDROID
   #define KEY_FILE "selfsigned.pem"
   #define CERT_FILE "selfsigned.crt"
   #define HOST_NAME L"android.bee.fish"

#else
   #define KEY_FILE "/etc/letsencrypt/live/bee.fish/privkey.pem"
   #define CERT_FILE "/etc/letsencrypt/live/bee.fish/fullchain.pem"

   #define HOST_NAME L"bee.fish"

#endif
#endif

//#define TMP_DH_FILE "dh2048.pem"

#endif
