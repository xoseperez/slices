#ifndef _FASTLED_MATRIX
#define _FASTLED_MATRIX

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <functional>

class FastLED_Matrix : public FastLED_GFX  {

    public:

        typedef std::function<unsigned int(unsigned int, unsigned int)> TIndexFunction;

        FastLED_Matrix(int w, int h);
        void drawPixel(int16_t x, int16_t y, CRGB color);
        void setIndexFunction(TIndexFunction fn);
        void setPixels(CRGB * pixels);

    private:

        FastLED_GFX * _matrix;
        CRGB * _pixels;
        TIndexFunction _indexFn;

};

#endif
