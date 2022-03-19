#ifndef BEE_FISH_SERVER__CONFIG_H
#define BEE_FISH_SERVER__CONFIG_H

#if __has_include("../config.h")
   #include "../config.h"
#endif


#define BEE_FISH_SERVER_LOG_FILE    HOME_DIRECTORY "/server.log"
#define BEE_FISH_SERVER_ERR_FILE    HOME_DIRECTORY "/server.err"

#define BEE_FISH_DATABASE_FILE      HOME_DIRECTORY "/bee.fish.data"
#define BEE_FISH_TRANSACTION_FILE   HOME_DIRECTORY "/bee.fish.transaction"


#endif
