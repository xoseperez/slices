/*

SOUND MODULE

Copyright (C) 2016-2020 by Xose Pérez <xose at tarpunacoop dot org>

*/

// -----------------------------------------------------------------------------
// SOUND
// -----------------------------------------------------------------------------

void sound(unsigned int frequency, unsigned long duration, unsigned char times) {
    while (times) {
        sound(frequency, duration);
        --times;
        if (times > 0) nice_delay(duration);
    }
}

void sound(unsigned int frequency, unsigned long duration) {
    tone(BUZZER_PIN, frequency, duration);
}

void sound() {
    noTone(BUZZER_PIN);
}


void soundSetup() {

    terminalRegisterCommand(F("SOUND"), [](Embedis* e) {

        if (e->argc == 3) {
            int frequency = String(e->argv[1]).toInt();
            int duration = String(e->argv[2]).toInt();
            sound(frequency, duration);
        } else {
            sound();
        }

        terminalOK();

    });

    pinMode(BUZZER_PIN, OUTPUT);

}
