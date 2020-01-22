/*

NTP MODULE

Copyright (C) 2016-2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <TimeLib.h>
#include <NtpClientLib.h>

bool _ntpConnected = false;

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

void ntpConnect() {

    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    int sign = offset > 0 ? 1 : -1;
    offset = abs(offset);
    int tz_hours = sign * (offset / 60);
    int tz_minutes = sign * (offset % 60);

    NTP.begin(
        getSetting("ntpServer", NTP_SERVER).c_str(), 
        tz_hours, 
        getSetting("ntpDST", NTP_DAY_LIGHT).toInt() == 1,
        tz_minutes
    );

}

void ntpDisconnect() {
    NTP.stop();
}

bool ntpConnected() {
    return _ntpConnected;
}

void ntpSetup() {

    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            if (error == noResponse) {
                DEBUG_MSG_P(PSTR("[NTP] Error: NTP server not reachable\n"));
            } else if (error == invalidAddress) {
                DEBUG_MSG_P(PSTR("[NTP] Error: Invalid NTP server address\n"));
            }
            _ntpConnected = false;
        } else {
            time_t sync = NTP.getLastNTPSync();
            DEBUG_MSG_P(PSTR("[NTP] Time: %s\n"), (char *) NTP.getTimeDateString(sync).c_str());
            rtcSet(RtcDateTime(year(sync), month(sync), day(sync), hour(sync), minute(sync), second(sync)));
            matrixRefresh();
            _ntpConnected = true;
        }
    });

}

void ntpLoop() {
    now();
}
