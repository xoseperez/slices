/*

CANVAS DRIVER

Copyright (C) 2017-2024 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_CANVAS

#include <FastLED_GFX.h>

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void canvasDraw(byte x, byte y, const char * color) {

    DEBUG_MSG_P("[CANVAS] x:%d y:%d color:%s\n", x, y, color);

    char * p = (char *) color;
    ++p;
    unsigned long value = strtol(p, NULL, 16);
    unsigned char red = (value >> 16) & 0xFF;
    unsigned char green = (value >> 8) & 0xFF;
    unsigned char blue = (value) & 0xFF;

    matrixSetPixelColor(x, y, CRGB(red, green, blue));
    matrixRefresh();

}

void canvasSetup() {
    driverRegister("Canvas", NULL, NULL, NULL, driverCommonStatus, driverCommonProgress);
}

#endif
