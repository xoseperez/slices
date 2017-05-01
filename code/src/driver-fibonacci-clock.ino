/*

FIBONACCI CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_NeoMatrix.h>

// blocks are:
// number : 1, 2, 3, 4,  5
// binary : 1, 2, 4, 8, 16
// value  : 1, 1, 2, 3,  5
byte fibonacci_code_00[] = {0};
byte fibonacci_code_01[] = {1, 2};
byte fibonacci_code_02[] = {4, 3};
byte fibonacci_code_03[] = {8, 5, 6};
byte fibonacci_code_04[] = {7, 9, 10};
byte fibonacci_code_05[] = {16, 11, 12};
byte fibonacci_code_06[] = {18, 13, 14, 17};
byte fibonacci_code_07[] = {20, 15, 19};
byte fibonacci_code_08[] = {24, 21, 22};
byte fibonacci_code_09[] = {23, 25, 26};
byte fibonacci_code_10[] = {28, 27};
byte fibonacci_code_11[] = {30, 29};
byte fibonacci_code_12[] = {31};
byte* fibonacci_codes[13] = {
   fibonacci_code_00, fibonacci_code_01, fibonacci_code_02, fibonacci_code_03,
   fibonacci_code_04, fibonacci_code_05, fibonacci_code_06, fibonacci_code_07,
   fibonacci_code_08, fibonacci_code_09, fibonacci_code_10, fibonacci_code_11,
   fibonacci_code_12
};
byte fibonacci_options[13] = {1, 2, 2, 3, 3, 3, 4, 3, 3, 3, 2, 2, 1};

#if MATRIX_SIZE == MATRIX_8x8

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

unsigned long fibonacci_colors[4];

unsigned char fibonacciPreviousMinute = 0;

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

unsigned char fibonacciClockRandomCode(unsigned int value) {
    byte * possible = fibonacci_codes[value];
    byte chosen = random(fibonacci_options[value]);
    return possible[chosen];
}

void fibonacciValueMatrix(unsigned int value, unsigned int * array) {

    unsigned char code = fibonacciClockRandomCode(value);

    unsigned block = 0;
    while (code > 0) {
        if ((code & 1) == 1) {
            for (unsigned char i=0; i<MATRIX_HEIGHT; i++) {
                array[i] = array[i] + fibonacci_blocks[block][i];
            }
        }
        ++block;
        code >>= 1;
    }


}

void fibonacciClockLoop() {

    RtcDateTime now = rtcGet();
    int currentMinute = now.Minute();
    if (currentMinute == fibonacciPreviousMinute) return;
    fibonacciPreviousMinute = currentMinute;
    int currentHour = now.Hour();

    // Load each value matrix separatelly
    unsigned int hours[MATRIX_HEIGHT] = {0};
    if (currentHour > 12) currentHour -= 12;
    fibonacciValueMatrix(currentHour, hours);
    unsigned int minutes[MATRIX_HEIGHT] = {0};
    fibonacciValueMatrix(currentMinute / 5, minutes);

    // Prepare the display
    Adafruit_NeoMatrix * matrix = getMatrix();
    matrix->fillScreen(0);

    // Walk the matrices choosing the colors
    #if MATRIX_SIZE == MATRIX_8x8
        int start = 1;
        int end = 6;
    #else
        int start = 2;
        int end = 12;
    #endif
    for (unsigned char y = start; y < end; y++) {
        for (unsigned char x = 0; x < MATRIX_WIDTH; x++) {
            int value = ((hours[y] >> x) & 1) + 2 * ((minutes[y] >> x) & 1);
            matrix->drawPixel(MATRIX_WIDTH - x - 1, y, fibonacci_colors[value]);
        }
    }

    for (unsigned char n = 0; n < currentMinute % 5; n++) {
        #if MATRIX_SIZE == MATRIX_8x8
            matrix->drawPixel(MATRIX_WIDTH - n - 1, MATRIX_HEIGHT - 1, fibonacci_colors[0]);
        #else
            matrix->drawPixel(MATRIX_WIDTH - n*3 - 1, MATRIX_HEIGHT - 1, fibonacci_colors[0]);
            matrix->drawPixel(MATRIX_WIDTH - n*3 - 1, MATRIX_HEIGHT - 2, fibonacci_colors[0]);
            matrix->drawPixel(MATRIX_WIDTH - n*3 - 2, MATRIX_HEIGHT - 1, fibonacci_colors[0]);
            matrix->drawPixel(MATRIX_WIDTH - n*3 - 2, MATRIX_HEIGHT - 2, fibonacci_colors[0]);
        #endif
    }

    matrixRefresh();

}

void fibonacciClockSetup() {

    Adafruit_NeoMatrix * matrix = getMatrix();

    fibonacci_colors[0] = matrix->Color(64, 64, 64);
    fibonacci_colors[1] = matrix->Color(255, 10, 10);
    fibonacci_colors[2] = matrix->Color(10, 255, 10);
    fibonacci_colors[3] = matrix->Color(10, 10, 255);

    driverRegister(
        "Fibonacci clock",
        []{
            fibonacciPreviousMinute = 99;
            fibonacciClockLoop();
        },
        fibonacciClockLoop,
        NULL
    );

}
