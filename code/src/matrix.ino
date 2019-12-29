/*

MATRIX MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <FastLED_Matrix.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>

CRGB _leds[MATRIX_LEDS];
FastLED_Matrix * _matrix;

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

unsigned int getTextWidth(const char * text) {
    int16_t x1, y1;
    uint16_t w, h;
    _matrix->getTextBounds((char *) text, 0, 0, &x1, &y1, &w, &h);
    return w;
}

void matrixWrite(int x, int y, const char * text, unsigned long color) {
    _matrix->setCursor(x, y);
    _matrix->setTextColor(color);
    _matrix->print(text);
}

void matrixWrite(int x, int y, const char * text) {
    matrixWrite(x, y, text, CRGB::Blue);
}

void matrixScroll(byte y, const char * text, bool once, blindCallback endScroll) {
    if (_text) free(_text);
    _text = strdup(text);
    _textWidth = getTextWidth(text);
    _scrollOnce = once;
    _scrollY = y;
    _scrollX = _matrix->width();
    _endScroll = endScroll;
    scrollTicker.attach_ms(MATRIX_SCROLL_INTERVAL, matrixLoop);
    //DEBUG_MSG_P(PSTR("[MATRIX] Scrolling text '%s'\n"), text);
}

void matrixScroll(const char * text) {
    matrixScroll(0, text, false, NULL);
}

void matrixLoop() {

    if (--_scrollX < -_textWidth) {
        if (_scrollOnce) matrixStopScroll();
        if (_endScroll) (_endScroll)();
        if (_scrollOnce) return;
        _scrollX = _matrix->width();
    }

    FastLED.clear();
    matrixWrite(_scrollX, _scrollY, _text, CRGB::Red);
    matrixRefresh();

}

void matrixRefresh(bool show_wifi_status) {

    if (show_wifi_status) {
        if (!wifiConnected()) {
            if (WiFi.getMode() == WIFI_AP) {
                _matrix->drawPixel(0, MATRIX_HEIGHT - 1, CRGB::Blue);
            } else {
                _matrix->drawPixel(0, MATRIX_HEIGHT - 1, CRGB::Red);
            }
        } else if (!ntpConnected()) {
            _matrix->drawPixel(0, MATRIX_HEIGHT - 1, CRGB::Orange);
        } else {
            _matrix->drawPixel(0, MATRIX_HEIGHT - 1, CRGB::Black);
        }
    }

    // Clock code
    #if ENABLE_DRIVER_CIRCLE
        circleClockLoop();
    #endif

    FastLED.show();

}

void matrixRefresh() {
    matrixRefresh(true);
}

void matrixStopScroll() {
    scrollTicker.detach();
}

void matrixClear() {
    FastLED.clear();
}

void matrixSetPixelColor(unsigned int x, unsigned int y, CRGB color) {
    if (0 > x || x >= MATRIX_WIDTH) return;
    if (0 > y || y >= MATRIX_HEIGHT) return;
    unsigned int index = matrixGetIndex(x, y);
    _leds[index] = color;
}

void matrixAddPixelColor(unsigned int index, CRGB color) {
    if (0 <= index && index < MATRIX_LEDS) {
        _leds[index] += color;
    }
}

void matrixSetPixelColor(unsigned int index, CRGB color) {
    if (0 <= index && index < MATRIX_LEDS) {
        _leds[index] = color;
    }
}

unsigned int matrixGetIndex(unsigned int x, unsigned int y) {
    unsigned int index = 0;
    #if MATRIX_SIZE == MATRIX_16x16
        if (x % 2 == 1) y = 15 - y;
        index = x * 16 + y;
    #endif
    #if MATRIX_SIZE == MATRIX_16x8
        if (x>7) { x -= 8; index += 64; }
        index += y * 8 + x;
    #endif
    #if MATRIX_SIZE == MATRIX_8x8
        index = (MATRIX_WIDTH - x - 1) * MATRIX_WIDTH + y;
    #endif
    return index;
}

void matrixTest() {
    matrixClear();
    for (int i=0; i< MATRIX_LEDS; i++) {
        _leds[i] = CRGB::Red;
        FastLED.show();
        delay(200);
    }
}

void matrixSetup() {

    FastLED.addLeds<MATRIX_CHIPSET, MATRIX_PIN, MATRIX_COLOR_ORDER>(_leds, MATRIX_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(MATRIX_DEFAULT_BRIGHTNESS);

    _matrix = new FastLED_Matrix(MATRIX_WIDTH, MATRIX_HEIGHT);
    _matrix->setPixels(_leds);
    _matrix->setIndexFunction(matrixGetIndex);
    _matrix->setTextWrap(false);

    //matrixTest();

}
