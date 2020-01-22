/*

GAUGE CLOCK DRIVER

Copyright (C) 2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_GAUGE

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

uint16_t _gauge_clock_hour = CIRCLE_SIZE;
int16_t _gauge_clock_peak = 0;

void gaugeClockLoop() {

    static uint8_t counter = 0;
    if (counter++ < 20) return;
    counter = 0;

    RtcDateTime now = rtcGet();
    int current_minute = now.Minute();
    int current_hour = now.Hour();

    int hour = ((current_hour % 12) * 60 + current_minute) / 12;
    //if (_gauge_clock_hour == hour) return;
    _gauge_clock_hour = hour;
    _gauge_clock_peak = (_gauge_clock_peak + 1) % CIRCLE_SIZE;

    // Prepare the display
    for (int index = 0; index < CIRCLE_SIZE; index++) {
        matrixSetPixelColor(CIRCLE_START + index, (index <= hour) ? CRGB::Green : CRGB::DarkBlue);
        int distance = abs(_gauge_clock_peak - index);
        if (distance > 5) distance = 5;
        uint8_t fraction = 255 - distance * 25;
        matrixDimPixelColor(CIRCLE_START + index, fraction);
    }

    matrixRefresh();

}

void gaugeClockStatus(uint8_t value) {
    CRGB color = stateColor(value);
    matrixSetPixelColor(CIRCLE_START + _gauge_clock_hour, color);
}

void gaugeClockProgress(uint8_t value) {
    long num = CIRCLE_SIZE * value / 100;
    for (unsigned char x=0; x<CIRCLE_SIZE; x++) {
        matrixSetPixelColor(CIRCLE_START + x, (num >= x) ? CRGB::Orange : 0x080808);
    }
}

void gaugeClockSetup() {
    driverRegister("Gauge Clock", NULL, gaugeClockLoop, NULL, gaugeClockStatus, gaugeClockProgress);
}

#endif
