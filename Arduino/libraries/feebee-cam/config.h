#ifndef FEEBEE_CAM__CONFIG_H
#define FEEBEE_CAM__CONFIG_H

// Watc h dog time out for camera web server
// (5 minutes)
#define WDT_TIMEOUT (5 * 60)

// Seconds to wake up every x seconds
#define CHECK_EVERY_SECONDS 30.0

//  Configuration of NTP //

// choose the best fitting NTP server pool for your country
#define MY_NTP_SERVER "au.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TIMEZONE "AEST-10"

// Settings for default WiFi connection
// Handy for downloading files
#define ACCESS_POINT_SSID "FeebeeCam"
#define DEFAULT_SSID "laptop"         // your network SSID (name)
#define DEFAULT_PASSWORD "feebeegeeb3"    // your network password

// Milliseconds to wait for connection to ssid
#define WAIT_FOR_STA_CONNECT_TIME_OUT 60000 // 1 minute

// Seconds to deep sleep after error
#define SLEEP_SECONDS_AFTER_ERROR 10

// Local DNS host name
#define LOCAL_DNS_HOST_NAME "bee.fish.local"

// Web request time out in milliseconds
#define WEB_REQUEST_TIMEOUT 10000

#endif
