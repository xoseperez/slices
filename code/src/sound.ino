/*

SOUND MODULE

Copyright (C) 2016 by Xose Pérez <xose at tarpunacoop dot org>

*/

// -----------------------------------------------------------------------------
// SOUND
// -----------------------------------------------------------------------------

bool _sound_state = false;

void sound(unsigned int frequency, unsigned long duration) {
    tone(BUZZER_PIN, frequency, duration);
}

void sound() {
    noTone(BUZZER_PIN);
}

void alarm(bool state) {
    _sound_state = state;
}

void soundLoop() {

    if (_sound_state) {
    }
    
}

void soundSetup() {
    pinMode(BUZZER_PIN, OUTPUT);
}
