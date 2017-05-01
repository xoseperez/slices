/*

RTC MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <RtcDS1307.h>
RtcDS1307<TwoWire> rtc(Wire);

// -----------------------------------------------------------------------------
// RTC
// -----------------------------------------------------------------------------

void rtcSetup() {

    rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!rtc.IsDateTimeValid()) {
        DEBUG_MSG_P(PSTR("[RTC] Lost confidence in the RTC. Updating time to compile time\n"));
        rtc.SetDateTime(compiled);
    }

    if (!rtc.GetIsRunning()) {
        DEBUG_MSG_P(PSTR("[RTC] RTC not actively running. Starting now\n"));
        rtc.SetIsRunning(true);
    }

    RtcDateTime now = rtc.GetDateTime();
    if (now < compiled) {
        DEBUG_MSG_P(PSTR("[RTC] Time is older than compile time. Updating time to compile time\n"));
        rtc.SetDateTime(compiled);
        now = compiled;

    }

    DEBUG_MSG_P(
        PSTR("[RTC] Time: %04u/%02u/%02u %02u:%02u:%02u\n"),
        now.Year(),
        now.Month(),
        now.Day(),
        now.Hour(),
        now.Minute(),
        now.Second()
    );

}

void rtcSet(RtcDateTime now) {
    rtc.SetDateTime(now);
}

RtcDateTime rtcGet() {
    return rtc.GetDateTime();
}
