/*

BASIC CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_BASIC

#include <FastLED_GFX.h>

int basicClockPrevious = 99;

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

#if MATRIX_SIZE == MATRIX_16x16

void basicClockLoop() {

    RtcDateTime now = rtcGet();
    if (now.Minute() == basicClockPrevious) return;
    basicClockPrevious = now.Minute();

    char buffer[4];
    matrixClear();
    snprintf(buffer, 3, "%02u", now.Hour());
    matrixWrite(2, 0, buffer);
    snprintf(buffer, 3, "%02u", now.Minute());
    matrixWrite(2, 8, buffer);
    matrixRefresh();

}

void basicClockSetup() {
    driverRegister(
        "basic-clock",
        []{
            basicClockPrevious = 99;
            basicClockLoop();
        },
        basicClockLoop,
        NULL,
        driverCommonStatus, 
        driverCommonProgress
    );
}

#else

void basicClockStart() {
    RtcDateTime now = rtcGet();
    char buffer[6];
    snprintf(buffer, 6, "%02u:%02u", now.Hour(), now.Minute());
    matrixScroll(0, buffer, true, basicClockStart);
}

void basicClockSetup() {
    driverRegister("Basic clock", basicClockStart, NULL, NULL, driverCommonStatus, driverCommonProgress);
}

#endif

#endif
