/*

MATRIX MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>

//#include "fonts/FreeSansBold5pt8b.h"

Adafruit_NeoMatrix * _matrix;
Ticker scrollTicker;

char * _text;
int _textWidth = 0;
bool _scrollOnce = false;
int _scrollX = -9999;
int _scrollY = 0;
blindCallback _endScroll;

// -----------------------------------------------------------------------------
// SETUP
// -----------------------------------------------------------------------------

Adafruit_NeoMatrix * getMatrix() {
    return _matrix;
}

unsigned int getTextWidth(const char * text) {
    Adafruit_NeoMatrix * matrix = getMatrix();
    int16_t x1, y1;
    uint16_t w, h;
    matrix->getTextBounds((char *) text, 0, 0, &x1, &y1, &w, &h);
    return w;
}

void matrixClear() {
    Adafruit_NeoMatrix * matrix = getMatrix();
    matrix->fillScreen(0);
    matrix->show();
}

void matrixRefresh() {
    Adafruit_NeoMatrix * matrix = getMatrix();
    if (!wifiConnected()) {
        if (WiFi.getMode() == WIFI_AP) {
            matrix->drawPixel(0, MATRIX_HEIGHT - 1, matrix->Color(10, 10, 255));
        } else {
            matrix->drawPixel(0, MATRIX_HEIGHT - 1, matrix->Color(255, 10, 10));
        }
    } else if (!ntpConnected()) {
        matrix->drawPixel(0, MATRIX_HEIGHT - 1, matrix->Color(255, 255, 10));
    }
    matrix->show();
}

void matrixWrite(int x, int y, const char * text, unsigned long color) {
    Adafruit_NeoMatrix * matrix = getMatrix();
    matrix->setCursor(x, y);
    matrix->setTextColor(color);
    matrix->print(text);
}

void matrixWrite(int x, int y, const char * text) {
    Adafruit_NeoMatrix * matrix = getMatrix();
    unsigned long color = matrix->Color(10, 10, 255);
    matrixWrite(x, y, text, color);
}

void matrixScroll(byte y, const char * text, bool once, blindCallback endScroll) {
    Adafruit_NeoMatrix * matrix = getMatrix();
    if (_text) free(_text);
    _text = strdup(text);
    _textWidth = getTextWidth(text);
    _scrollOnce = once;
    _scrollY = y;
    _scrollX = matrix->width();
    _endScroll = endScroll;
    scrollTicker.attach(MATRIX_SCROLL_INTERVAL, matrixLoop);
}

void matrixScroll(const char * text) {
    matrixScroll(0, text, false, NULL);
}

void matrixStopScroll() {
    scrollTicker.detach();
}

void matrixLoop() {

    Adafruit_NeoMatrix * matrix = getMatrix();

    if (--_scrollX < -_textWidth) {
        if (_scrollOnce) matrixStopScroll();
        if (_endScroll) (_endScroll)();
        if (_scrollOnce) return;
        _scrollX = matrix->width();
    }

    matrix->fillScreen(0);
    matrixWrite(_scrollX, _scrollY, _text, matrix->Color(255, 0, 0));
    matrix->show();

}

void matrixSetup() {

    // Create object
    _matrix = new Adafruit_NeoMatrix(
        MATRIX_WIDTH,
        MATRIX_HEIGHT,
        MATRIX_PIN,
        MATRIX_MODE,
        MATRIX_TYPE
    );

    _matrix->begin();
    _matrix->setTextWrap(false);
    //_matrix->setFont(&FreeSansBold5pt8b);
    _matrix->setBrightness(MATRIX_DEFAULT_BRIGHTNESS);

}
