#ifndef FEEBEE_CAM__CONFIG_H
#define FEEBEE_CAM__CONFIG_H

// Watc h dog time out for camera web server
// (5 minutes)
#define WDT_TIMEOUT (5 * 60)

// Seconds to wake up every x seconds
#define WAKEUP_EVERY_SECONDS (60.0 * 15.0)

// Seconds to wait till take picture
#define TAKE_PICTURE_EVERY (60.0 * 59.0)

//  Configuration of NTP //

// choose the best fitting NTP server pool for your country
#define MY_NTP_SERVER "au.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
// https://bee.fish/beehive/time-zones.json

#define MY_TIMEZONE "AEST-10"
#define MY_TIMEZONE_LABEL "Australia/Brisbane"
#define MY_SSID "Bloody Bees"
#define MY_LABEL "Bloody Bees"
#define MY_PASSWORD "password"

// Settings for default WiFi connection
// Handy for downloading files
#define ACCESS_POINT_SSID "FeebeeCam"
#define DEFAULT_SSID "laptop"         // your network SSID (name)
#define DEFAULT_PASSWORD "feebeegeeb3"    // your network password

// Milliseconds to wait for connection to ssid
#define WAIT_FOR_WIFI_CONNECT 120000 // 2 minute

// Seconds to deep sleep after error
#define SLEEP_SECONDS_AFTER_ERROR 10

// Local DNS host name
#define LOCAL_DNS_HOST_NAME "bee.fish.local"

// Web request time out in milliseconds
#define WEB_REQUEST_TIMEOUT 20000

// Number of seconds to wait for camera to be used
#define CAMERA_WATCH_DOG_SECONDS (1000 * 60 * 5) // 5 minutes

#define PUBLIC_SECRET_HASH "4db14a0e15e8a6a1bf1eda4dcb5e41c4db7ec311575722b88ac8b3fc0019e2f57ba2518a042f8f6292955f6187f675cee3e94564903faa069194720ff374ca55"

#define BEEHIVE_VERSION "1.3"

#define MAX_WEB_CLIENTS 3

#define COMMAND_CORE      0
#define CAMERA_CORE       1
#define WEB_SERVER_CORE (-1)

#endif
