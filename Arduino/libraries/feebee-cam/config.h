#ifndef FEEBEE_CAM__CONFIG_H
#define FEEBEE_CAM__CONFIG_H

// Watch dog time out for camera web server
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

#endif
