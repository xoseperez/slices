#include <FastLED.h>
#include <FastLED_GFX.h>
#include <FastLED_Matrix.h>

FastLED_Matrix::FastLED_Matrix(int w, int h) : FastLED_GFX(w, h) {}

void FastLED_Matrix::drawPixel(int16_t x, int16_t y, CRGB color) {
    if (_indexFn) {
        if (0 > x || x >= WIDTH) return;
        if (0 > y || y >= HEIGHT) return;
        _pixels[(_indexFn)(x, y)] = color;
    }
}

void FastLED_Matrix::setPixels(CRGB * pixels) {
    _pixels = pixels;
};

void FastLED_Matrix::setIndexFunction(TIndexFunction fn) {
    _indexFn = fn;
}
