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

void stateShow() {

    CRGB color = CRGB::Black;
    if (STATE_WPS == _state) color = CRGB::Purple;
    if (STATE_AP == _state) color = CRGB::Blue;
    if (STATE_CONNECTING == _state) color = CRGB::Orange;
    if (STATE_CONNECTED == _state) color = CRGB::Green;
    if (STATE_ERROR == _state) color = CRGB::Red;

    matrixSetPixelColor(0, MATRIX_HEIGHT - 1, color);

}