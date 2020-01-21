/*

BASIC CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_BINARY

#include <FastLED.h>

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void binaryClockDraw(byte number, byte column, byte red, byte green, byte blue) {

    CRGB color_on = CRGB(red, green, blue);
    CRGB color_off = CRGB(red, green, blue) % 16;

    for (byte row=5; row>1; row--) {

        CRGB use_color = (number & 1) ? color_on : color_off;

        #if (MATRIX_HEIGHT == 8)
            matrixSetPixelColor(column, row, use_color);
        #else
            matrixSetPixelColor(column*2, row*2, use_color);
            matrixSetPixelColor(column*2+1, row*2, use_color);
            matrixSetPixelColor(column*2, row*2+1, use_color);
            matrixSetPixelColor(column*2+1, row*2+1, use_color);
        #endif

        number >>= 1;

    }

}

void binaryClockLoop() {

    static unsigned long last = 0;

    if (millis() - last > ONE_SECOND) {

        last = millis();

        RtcDateTime now = rtcGet();
        int currentHour = now.Hour();
        int currentMinute = now.Minute();
        int currentSecond = now.Second();

        // Prepare the display
        matrixClear();

        int column_base = (MATRIX_WIDTH == MATRIX_HEIGHT) ? 0 : (MATRIX_WIDTH / 2 - 4);

        binaryClockDraw(currentHour / 10, 0 + column_base, 255, 0, 0);
        binaryClockDraw(currentHour % 10, 1 + column_base, 255, 0, 0);
        binaryClockDraw(currentMinute / 10, 3 + column_base, 0, 255, 0);
        binaryClockDraw(currentMinute % 10, 4 + column_base, 0, 255, 0);
        binaryClockDraw(currentSecond / 10, 6 + column_base, 0, 0, 255);
        binaryClockDraw(currentSecond % 10, 7 + column_base, 0, 0, 255);

        matrixRefresh();

    }

}

void binaryClockSetup() {
    driverRegister("Binary clock", NULL, binaryClockLoop, NULL, driverCommonStatus, driverCommonProgress);
}

#endif
