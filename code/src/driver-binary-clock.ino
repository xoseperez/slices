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
        matrix->drawPixel(column, row, use_color);
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

        matrix->show();

        //sound(262, 100);

    }

}

void binaryClockSetup() {
    driverRegister("Binary clock", NULL, binaryClockLoop, NULL);
}
