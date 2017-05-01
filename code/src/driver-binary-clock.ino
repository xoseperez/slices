/*

BASIC CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_NeoMatrix.h>



// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void binaryClockDraw(byte number, byte column, byte red, byte green, byte blue) {

    Adafruit_NeoMatrix * matrix = getMatrix();
    unsigned long color_on = matrix->Color(red, green, blue);
    unsigned long color_off = matrix->Color(red / 4, green / 4, blue / 4);

    for (byte row=5; row>1; row--) {
        unsigned long use_color = (number & 1) ? color_on : color_off;

        #if MATRIX_SIZE == MATRIX_8x8
            matrix->drawPixel(column, row, use_color);
        #else
            matrix->drawPixel(column*2, row*2, use_color);
            matrix->drawPixel(column*2+1, row*2, use_color);
            matrix->drawPixel(column*2, row*2+1, use_color);
            matrix->drawPixel(column*2+1, row*2+1, use_color);
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
        Adafruit_NeoMatrix * matrix = getMatrix();
        matrix->fillScreen(0);

        binaryClockDraw(currentHour / 10, 0, 255, 0, 0);
        binaryClockDraw(currentHour % 10, 1, 255, 0, 0);
        binaryClockDraw(currentMinute / 10, 3, 0, 255, 0);
        binaryClockDraw(currentMinute % 10, 4, 0, 255, 0);
        binaryClockDraw(currentSecond / 10, 6, 0, 0, 255);
        binaryClockDraw(currentSecond % 10, 7, 0, 0, 255);

        matrixRefresh();

        DEBUG_MSG_P("%ld\n", matrix->Color(255,0,0));
        DEBUG_MSG_P("%ld\n", matrix->Color(0,255,0));
        DEBUG_MSG_P("%ld\n", matrix->Color(0,0,255));
        DEBUG_MSG_P("%ld\n", matrix->Color(0,0,0));

    }

}

void binaryClockSetup() {
    driverRegister("binary-clock", NULL, binaryClockLoop, NULL);
}
