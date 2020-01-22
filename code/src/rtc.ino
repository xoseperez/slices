/*

RTC MODULE

Copyright (C) 2017-2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <RtcDS1307.h>
RtcDS1307<TwoWire> rtc(Wire);

// -----------------------------------------------------------------------------
// RTC
// -----------------------------------------------------------------------------

void rtcDebug() {

    RtcDateTime now = rtcGet();

    DEBUG_MSG_P(
        PSTR("[RTC] Time: %02u:%02u:%02u %02u/%02u/%04u\n"),
        now.Hour(),
        now.Minute(),
        now.Second(),
        now.Day(),
        now.Month(),
        now.Year()
    );

}

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

    rtcDebug();

    terminalRegisterCommand(F("TIME"), [](Embedis* e) {
        rtcDebug();
        terminalOK();
    });

}

void rtcSet(RtcDateTime now) {
    rtc.SetDateTime(now);
    rtcDebug();
}

RtcDateTime rtcGet() {
    return rtc.GetDateTime();
}
