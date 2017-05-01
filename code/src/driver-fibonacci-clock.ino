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

// pixels
// 00000000
// 55555333
// 55555333
// 55555333
// 55555122
// 55555122
// 00000000
// 00000000
byte fibonacci_block_01a[] = {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00 };
byte fibonacci_block_01b[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 };
byte fibonacci_block_02[]  = {0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00 };
byte fibonacci_block_03[]  = {0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00 };
byte fibonacci_block_05[]  = {0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00 };
byte * fibonacci_blocks[5] = { fibonacci_block_01a, fibonacci_block_01b, fibonacci_block_02, fibonacci_block_03, fibonacci_block_05};

unsigned long fibonacci_colors[4];


// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

unsigned char fibonacciClockRandomCode(unsigned int value) {
    byte * possible = fibonacci_codes[value];
    byte chosen = random(fibonacci_options[value]);
    return possible[chosen];
}

void fibonacciValueMatrix(unsigned int value, byte * array) {

    unsigned char code = fibonacciClockRandomCode(value);

    unsigned block = 0;
    while (code > 0) {
        if ((code & 1) == 1) {
            for (unsigned char i=0; i<8; i++) {
                array[i] = array[i] + fibonacci_blocks[block][i];
            }
        }
        ++block;
        code >>= 1;
    }


}

void fibonacciClockLoop() {

    static unsigned long last = 0;
    static unsigned char previousMinute = 0;

    if (millis() - last > ONE_SECOND) {

        last = millis();

        RtcDateTime now = rtcGet();
        int currentMinute = now.Minute();
        if (currentMinute == previousMinute) return;
        previousMinute = currentMinute;
        int currentHour = now.Hour();

        // Load each value matrix separatelly
        byte hours[8] = {0};
        if (currentHour > 12) currentHour -= 12;
        fibonacciValueMatrix(currentHour, hours);
        byte minutes[8] = {0};
        fibonacciValueMatrix(currentMinute / 5, minutes);

        // Prepare the display
        Adafruit_NeoMatrix * matrix = getMatrix();
        matrix->fillScreen(0);

        // Walk the matrices choosing the colors
        for (unsigned char y = 1; y < 6; y++) {
            for (unsigned char x = 0; x < 8; x++) {
                int value = ((hours[y] >> x) & 1) + 2 * ((minutes[y] >> x) & 1);
                matrix->drawPixel(7-x, y, fibonacci_colors[value]);
            }
        }

        for (unsigned char x = 8 - currentMinute % 5; x < 8; x++) {
            matrix->drawPixel(x, 7, fibonacci_colors[0]);
        }

        matrix->show();

    }

}

void fibonacciClockSetup() {

    Adafruit_NeoMatrix * matrix = getMatrix();

    fibonacci_colors[0] = matrix->Color(64, 64, 64);
    fibonacci_colors[1] = matrix->Color(255, 10, 10);
    fibonacci_colors[2] = matrix->Color(10, 255, 10);
    fibonacci_colors[3] = matrix->Color(10, 10, 255);

    driverRegister("Fibonacci clock", NULL, fibonacciClockLoop, NULL);

}
