/*

CIRCLE CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_CIRCLE

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void circleDrawComet(unsigned int index, CRGB color) {
    unsigned int dimming = 256;
    unsigned int count = 0;
    while (dimming > 0) {
        int i = (index - count) % CIRCLE_SIZE;
        matrixAddPixelColor(CIRCLE_START + i, color % dimming);
        dimming /= 2;
        ++count;
    }
}

void circleDrawWave(double position, int length, CRGB color) {
    position = position + 60;
    unsigned int center = (int) position;
    for (unsigned int index = center - length; index < center + length; index++) {
        unsigned int distance = 2 * abs(index - position);
        unsigned int dimming = (distance == 0) ? 256 : 256 / distance;
        DEBUG_MSG_P("[] %d\n", dimming);
        matrixAddPixelColor(CIRCLE_START + index % CIRCLE_SIZE, (dimming == 256) ? color  : color % dimming);
    }
}

void circleClockLoop() {

    //static unsigned long current = 0;;

    RtcDateTime now = rtcGet();
    double currentSecond = now.Second();
    //if (currentSecond == current) return;
    //current = currentSecond;
    int currentMinute = now.Minute();
    int currentHour = now.Hour();
    int currentMillis = millis() % 1000;

    // Prepare the display
    for (unsigned int index = 0; index < CIRCLE_SIZE; index++) {
        matrixSetPixelColor(CIRCLE_START + index, 0);
    }

    unsigned int hour = ((currentHour % 12) * 60 + currentMinute) / 12;
    matrixSetPixelColor(CIRCLE_START + hour, CRGB::Red);
    matrixAddPixelColor(CIRCLE_START + currentMinute, CRGB::Green);
    matrixAddPixelColor(CIRCLE_START + currentSecond, CRGB::Blue);
    //circleDrawWave(currentSecond + currentMillis / 1000.0, 5, CRGB::Blue);

    //matrixRefresh();

}

void circleClockSetup() {
    driverRegister("circle-clock", NULL, circleClockLoop, NULL);
}

#endif
