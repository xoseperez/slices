/*

FIBONACCI MOOD DRIVER

Copyright (C) 2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_FIBONACCI_MOOD

#include <FastLED_GFX.h>

/*
#if (MATRIX_SIZE == MATRIX_8x8 || MATRIX_SIZE == MATRIX_16x8)

// 00000000 0
// 55555333 1
// 55555333 2
// 55555333 3
// 55555122 4
// 55555122 5
// 00000000 6
// 00000000 7

unsigned int fibonacci_block_01a[] = {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00 };
unsigned int fibonacci_block_01b[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 };
unsigned int fibonacci_block_02[]  = {0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00 };
unsigned int fibonacci_block_03[]  = {0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00 };
unsigned int fibonacci_block_05[]  = {0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00 };

#else

// 0000000000000000 0
// 0000000000000000 1
// 5555555555333333 2
// 5555555555333333 3
// 5555555555333333 4
// 5555555555333333 5
// 5555555555333333 6
// 5555555555333333 7
// 5555555555112222 8
// 5555555555112222 9
// 5555555555112222 10
// 5555555555112222 11
// 0000000000000000 12
// 0000000000000000 13
// 0000000000000000 14
// 0000000000000000 15

unsigned int fibonacci_block_01a[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
unsigned int fibonacci_block_01b[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000 };
unsigned int fibonacci_block_02[]  = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x000F, 0x000F, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000 };
unsigned int fibonacci_block_03[]  = {0x0000, 0x0000, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
unsigned int fibonacci_block_05[]  = {0x0000, 0x0000, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0x0000, 0x0000, 0x0000, 0x0000 };

#endif

unsigned int * fibonacci_blocks[5] = { fibonacci_block_01a, fibonacci_block_01b, fibonacci_block_02, fibonacci_block_03, fibonacci_block_05};

*/

#define UPDATE_MOOD_INTERVAL 100

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
CRGB wheel(uint8_t index) {

   if(index < 85) {
      return CRGB(index * 3, 255 - index * 3, 0);
   } else if(index < 170) {
      index -= 85;
      return CRGB(255 - index * 3, 0, index * 3);
   } else {
      index -= 170;
      return CRGB(0, index * 3, 255 - index * 3);
   }

}

void fibonacciMoodLoop() {

    static unsigned long count = 0;
    static unsigned long last = 0;
    if (millis() - last < UPDATE_MOOD_INTERVAL) return;
    last = millis();

    // Prepare the display
    matrixClear();

    // Walk the matrices choosing the colors
    #if (MATRIX_SIZE == MATRIX_8x8 || MATRIX_SIZE == MATRIX_16x8)
        int start = 1;
        int end = 6;
        int shift = (MATRIX_WIDTH / 2 - 4);
    #else
        int start = 2;
        int end = 12;
        int shift = 0;
    #endif

    // Load matrix
    for (uint8_t i=0; i<5; i++) {

        CRGB color = wheel(((i * 256 / 5) + count) & 255);

        for (unsigned char y = start; y < end; y++) {
            unsigned int pattern = fibonacci_blocks[i][y];
            for (unsigned char x = 0; x < MATRIX_WIDTH; x++) {
                if ((pattern & 1) == 1) matrixSetPixelColor(MATRIX_WIDTH - (x + shift) - 1, y, color);
                pattern >>= 1;
            }
        }

    }

    matrixRefresh();
    count = (count + 1) % (256*5);

}

void fibonacciMoodSetup() {

    driverRegister(
        "Fibonacci mood",
        NULL,
        fibonacciMoodLoop,
        NULL,
        driverCommonStatus, 
        driverCommonProgress
    );

}

#endif
