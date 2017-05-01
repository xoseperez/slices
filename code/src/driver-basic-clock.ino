/*

BASIC CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_NeoMatrix.h>

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void basicClockStart() {
    RtcDateTime now = rtcGet();
    char buffer[6];
    snprintf(buffer, 6, "%02u:%02u", now.Hour(), now.Minute());
    matrixScroll(buffer, true, basicClockStart);
}

void basicClockSetup() {
    driverRegister("Basic clock", basicClockStart, NULL, NULL);
}
