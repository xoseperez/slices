/*

CIRCLE CLOCK DRIVER

Copyright (C) 2017-2024 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_CIRCLE

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

uint16_t _circle_status_pixel = 0;

void circleClockLoop() {

    RtcDateTime now = rtcGet();
    int current_second = now.Second();
    int current_minute = now.Minute();
    int current_hour = now.Hour();

    // Prepare the display
    for (unsigned int index = 0; index < CIRCLE_SIZE; index++) {
        matrixSetPixelColor(CIRCLE_START + index, 0);
    }

    unsigned int hour = ((current_hour % 12) * 60 + current_minute) / 12;
    matrixSetPixelColor(CIRCLE_START + hour, CRGB::Red);
    matrixAddPixelColor(CIRCLE_START + current_minute, CRGB::Green);
    matrixAddPixelColor(CIRCLE_START + current_second, CRGB::DarkBlue);

    _circle_status_pixel = CIRCLE_START + current_second;
    matrixRefresh();

}

void circleStatus(uint8_t value) {
    if (0 == value) return;
    CRGB color = stateColor(value);
    matrixSetPixelColor(_circle_status_pixel, color);
}

void circleProgress(uint8_t value) {
    long num = CIRCLE_SIZE * value / 100;
    for (unsigned char x=0; x<CIRCLE_SIZE; x++) {
        matrixSetPixelColor(CIRCLE_START + x, (num >= x) ? CRGB::Orange : 0x080808);
    }
}

void circleClockSetup() {
    driverRegister("Clock", NULL, circleClockLoop, NULL, circleStatus, circleProgress);
}

#endif
