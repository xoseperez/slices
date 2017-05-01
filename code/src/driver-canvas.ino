/*

CANVAS DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_NeoMatrix.h>

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

    Adafruit_NeoMatrix * matrix = getMatrix();
    matrix->drawPixel(x, y, matrix->Color(red, green, blue));
    matrix->show();

}

void canvasSetup() {
    driverRegister("canvas", NULL, NULL, NULL);
}
