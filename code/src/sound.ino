/*

SOUND MODULE

Copyright (C) 2016 by Xose Pérez <xose at tarpunacoop dot org>

*/

// -----------------------------------------------------------------------------
// SOUND
// -----------------------------------------------------------------------------

void sound(unsigned int frequency, unsigned long duration) {
    tone(BUZZER_PIN, frequency, duration);
}

void soundSetup() {
    pinMode(BUZZER_PIN, OUTPUT);
}
