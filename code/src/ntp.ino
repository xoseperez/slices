/*

NTP MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <TimeLib.h>
#include <NtpClientLib.h>

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

void ntpConnect() {
    NTP.begin(NTP_SERVER, NTP_TIME_OFFSET, NTP_DAY_LIGHT);
    NTP.setInterval(NTP_UPDATE_INTERVAL);
}

void ntpSetup() {

    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            if (error == noResponse) {
                DEBUG_MSG_P(PSTR("[NTP] Error: NTP server not reachable\n"));
            } else if (error == invalidAddress) {
                DEBUG_MSG_P(PSTR("[NTP] Error: Invalid NTP server address\n"));
            }
        } else {
            time_t sync = NTP.getLastNTPSync();
            DEBUG_MSG_P(PSTR("[NTP] Time: %s\n"), (char *) NTP.getTimeDateString(sync).c_str());
            rtcSet(RtcDateTime(year(sync), month(sync), day(sync), hour(sync), minute(sync), second(sync)));
        }
    });

}

void ntpLoop() {
    now();
}
