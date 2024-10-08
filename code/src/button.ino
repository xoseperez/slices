/*

BUTTON MODULE

Copyright (C) 2016-2024 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// BUTTON
// -----------------------------------------------------------------------------

#include <DebounceEvent.h>
#include <vector>

typedef struct {
    DebounceEvent * button;
} button_t;

std::vector<button_t> _buttons;

bool buttonState(unsigned char id) {
    if (id >= _buttons.size()) return false;
    return _buttons[id].button->pressed();
}

uint8_t buttonEventMapping(uint8_t event, uint8_t count, uint16_t length) {
    if (event == EVENT_PRESSED) return BUTTON_EVENT_PRESSED;
    if (event == EVENT_CHANGED) return BUTTON_EVENT_CLICK;
    if (event == EVENT_RELEASED) {
        if (count == 1) {
            if (length > BUTTON_LNGLNGCLICK_LENGTH) return BUTTON_EVENT_LNGLNGCLICK;
            if (length > BUTTON_LNGCLICK_LENGTH) return BUTTON_EVENT_LNGCLICK;
            return BUTTON_EVENT_CLICK;
        }
        if (count == 2) return BUTTON_EVENT_DBLCLICK;
    }
    return BUTTON_EVENT_NONE;
}

void buttonEvent(unsigned int id, unsigned char event) {

    DEBUG_MSG_P(PSTR("[BUTTON] Pressed #%d, event: %d\n"), id, event);
    if (BUTTON_EVENT_NONE == event) return;

    // Devices with 1 button:
    //   Click: changes driver if more than one, else nothing
    //   DblClick: update time options (STA+NTP or AP)
    //   Long click: nothing
    //   Long-long click: factory reset & resets the board
    #ifndef BUTTON2_PIN

        // Single click enabled next driver
        if (BUTTON_EVENT_CLICK == event) {
            if (driverCount() > 1) {
                driverNext();
            } else {
                // nothing
            }
        }

        // Double click updates time (STA+NTP or AP)
        if (BUTTON_EVENT_DBLCLICK == event) {
            wifiOn(true);
        }

        // Long click does nothing
        if (BUTTON_EVENT_LNGCLICK == event) {
            // nothing
        }

        // Long-long click init WPS
        if (BUTTON_EVENT_LNGLNGCLICK == event) {
            resetSettings();
            deferredReset(100);
        }

    #endif

    // Device with 4 buttons:
    // TODO: handle device with more than 1 button

}

void buttonSetup() {

    #ifdef BUTTON1_PIN
    {
        _buttons.push_back({new DebounceEvent(BUTTON1_PIN, BUTTON1_MODE)});
    }
    #endif
    #ifdef BUTTON2_PIN
    {
        _buttons.push_back({new DebounceEvent(BUTTON2_PIN, BUTTON2_MODE)});
    }
    #endif
    #ifdef BUTTON3_PIN
    {
        _buttons.push_back({new DebounceEvent(BUTTON3_PIN, BUTTON3_MODE)});
    }
    #endif
    #ifdef BUTTON4_PIN
    {
        _buttons.push_back({new DebounceEvent(BUTTON4_PIN, BUTTON4_MODE)});
    }
    #endif

    DEBUG_MSG_P(PSTR("[BUTTON] Number of buttons: %d\n"), _buttons.size());
    buttonLoop();

}

void buttonLoop() {

    for (unsigned int i=0; i < _buttons.size(); i++) {
        if (unsigned char event = _buttons[i].button->loop()) {
            unsigned char count = _buttons[i].button->getEventCount();
            unsigned long length = _buttons[i].button->getEventLength();
            unsigned char mapped = buttonEventMapping(event, count, length);
            buttonEvent(i, mapped);
        }
   }

}
