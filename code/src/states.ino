/*

STATES MODULE

Copyright (C) 2019 by Xose Pérez <xose dot perez at gmail dot com>

*/

#define STATE_IDLE          0
#define STATE_WPS           1
#define STATE_AP            2
#define STATE_CONNECTING    3
#define STATE_CONNECTED     4
#define STATE_ERROR         5

#include <Ticker.h>
Ticker _state_ticker;
int _state = STATE_IDLE;

void stateSet(int state) {

    if (_state == state) return;
    
    _state = state;
    DEBUG_MSG_P(PSTR("[STATE] State: %d\n"), _state);

    _state_ticker.detach();
    if (STATE_ERROR == state) {
       _state_ticker.once(5, stateSet, STATE_IDLE);
    }

    matrixRefresh();

}

void stateSound(uint8_t state) {
    if (STATE_WPS == state) sound(400, 500);
    if (STATE_AP == state) sound(400, 500);
    if (STATE_CONNECTING == state) sound(500, 500);
    if (STATE_CONNECTED == state) sound(600, 500);
    if (STATE_ERROR == state) sound(700, 500);
}

CRGB stateColor(uint8_t state) {
    if (STATE_WPS == state) return CRGB::Purple;
    if (STATE_AP == state) return CRGB::Blue;
    if (STATE_CONNECTING == state) return CRGB::Orange;
    if (STATE_CONNECTED == state) return CRGB::Green;
    if (STATE_ERROR == state) return CRGB::Red; 
    return CRGB::Black;
}

void stateShow() {
    driverStatus(_state);
}