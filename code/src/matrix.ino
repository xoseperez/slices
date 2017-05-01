/*

MATRIX MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

//#include "fonts/FreeSansBold5pt8b.h"

Adafruit_NeoMatrix * _matrix;
Ticker scrollTicker;

char * _text;
int _textWidth = 0;
bool _scrollOnce = false;
int _scrollPosition = -9999;
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

void matrixScroll(const char * text, bool once, blindCallback endScroll) {
    Adafruit_NeoMatrix * matrix = getMatrix();
    if (_text) free(_text);
    _text = strdup(text);
    _textWidth = getTextWidth(text);
    _scrollOnce = once;
    _scrollPosition = matrix->width();
    _endScroll = endScroll;
    scrollTicker.attach(MATRIX_SCROLL_INTERVAL, matrixLoop);
}

void matrixScroll(const char * text) {
    matrixScroll(text, false, NULL);
}

void matrixStopScroll() {
    scrollTicker.detach();
}

void matrixLoop() {

    Adafruit_NeoMatrix * matrix = getMatrix();

    if (--_scrollPosition < -_textWidth) {
        if (_scrollOnce) matrixStopScroll();
        if (_endScroll) (_endScroll)();
        if (_scrollOnce) return;
        _scrollPosition = matrix->width();
    }

    matrix->fillScreen(0);
    matrix->setCursor(_scrollPosition, 0);
    matrix->setTextColor(matrix->Color(255, 0, 0));
    matrix->print(_text);
    matrix->show();

}

void matrixSetup() {

    // Create object
    _matrix = new Adafruit_NeoMatrix(
        MATRIX_WIDTH,
        MATRIX_HEIGHT,
        MATRIX_PIN,
        NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
        NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
        NEO_GRB            + NEO_KHZ800
    );

    _matrix->begin();
    _matrix->setTextWrap(false);
    //_matrix->setFont(&FreeSansBold5pt8b);
    _matrix->setBrightness(MATRIX_DEFAULT_BRIGHTNESS);

}
