/*

WORD CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_WORDCLOCK

#include <FastLED_GFX.h>
#include "driver-word-clock.h"

#define LANGUAGE_CATALAN    1
#define LANGUAGE_SPANISH    2

int _word_previous_hour = -1;
int _word_previous_minute = -1;
bool _word_force = false;
unsigned int _wordclock_time_pattern[16] = {0};

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

/**
 * Loads a word code into the time pattern
 * @param  clockword code          a tupla defining the leds to be lit to form a word
 * @param  unsigned  int *         LED pattern array representation
 */
void _wordClockLoadCode(clockword code, unsigned int * pattern) {
   pattern[code.row] = pattern[code.row] | code.positions;
}

void _wordClockEspanol(byte hour, byte minute, unsigned int * pattern) {

    /*

    00 => en punto
    01 => pasada/s
    02 => pasada/s

    03 => casi y cinco
    04 => casi y cinco
    05 => y cinco
    06 => y cinco pasadas
    07 => y cinco pasadas

    08 => casi y diez
    09 => casi y diez
    10 => y diez
    11 => y diez pasadas
    12 => y diez pasadas

    13 => casi y cuarto
    14 => casi y cuarto
    15 => y cuarto
    16 => y cuarto pasadas
    17 => y cuarto pasadas

    18 => casi y veinte
    19 => casi y veinte
    20 => y veinte
    21 => y veinte pasadas
    22 => y veinte pasadas

    23 => casi y veinticinco
    24 => casi y veinticinco
    25 => y veinticinco
    26 => y veinticinco pasadas
    27 => y veinticinco pasadas

    28 => casi y media
    29 => casi y media
    30 => y media
    31 => y media pasadas
    32 => y media pasadas

    33 => casi menos veinticinco
    34 => casi menos veinticinco
    35 => menos veinticinco
    36 => menos veinticinco pasadas
    37 => menos veinticinco pasadas

    38 => casi menos veinte
    39 => casi menos veinte
    40 => menos veinte
    41 => menos veinte pasadas
    42 => menos veinte pasadas

    43 => casi menos cuarto
    44 => casi menos cuarto
    45 => menos cuarto
    46 => menos cuarto pasadas
    47 => menos cuarto pasadas

    48 => casi menos diez
    49 => casi menos diez
    50 => menos diez
    51 => menos diez pasadas
    52 => menos diez pasadas

    53 => casi menos cinco
    54 => casi menos cinco
    55 => menos cinco
    56 => menos cinco pasadas
    57 => menos cinco pasadas

    58 => casi
    59 => casi

    */

    // indica si la hora se referencia a la actual o a la posterior
    bool hour_is_current = (minute < 33);

    // hora en formato 12 y referida
    byte hour_12 = (hour > 12) ? hour - 12 : hour;
    if (!hour_is_current) hour_12++;
    if (hour_12 == 13) hour_12 = 1;

    // indica si la hora es plural o singular
    bool hour_is_singular = (hour_12 == 1);

    // ARTICLE
    if (hour_is_singular) {
        _wordClockLoadCode((clockword) ESP_ES, pattern);
        _wordClockLoadCode((clockword) ESP_LA, pattern);
    } else {
        _wordClockLoadCode((clockword) ESP_SON, pattern);
        _wordClockLoadCode((clockword) ESP_LAS, pattern);
    }

    // BLOCKS
    byte reference = ((minute + 2) / 5) % 12;
    byte index = (minute + 2) % 5;
    if (reference == 0) {
        // NOP
    } else if (reference < 7) {
        _wordClockLoadCode((clockword) ESP_Y, pattern);
    } else {
        _wordClockLoadCode((clockword) ESP_MENOS, pattern);
    }
    if (reference ==  1) _wordClockLoadCode((clockword) ESP_CINCO_B, pattern);
    if (reference ==  2) _wordClockLoadCode((clockword) ESP_DIEZ_B, pattern);
    if (reference ==  3) _wordClockLoadCode((clockword) ESP_CUARTO, pattern);
    if (reference ==  4) _wordClockLoadCode((clockword) ESP_VEINTE, pattern);
    if (reference ==  5) _wordClockLoadCode((clockword) ESP_VEINTICINCO, pattern);
    if (reference ==  6) _wordClockLoadCode((clockword) ESP_MEDIA, pattern);
    if (reference ==  7) _wordClockLoadCode((clockword) ESP_VEINTICINCO, pattern);
    if (reference ==  8) _wordClockLoadCode((clockword) ESP_VEINTE, pattern);
    if (reference ==  9) _wordClockLoadCode((clockword) ESP_CUARTO, pattern);
    if (reference == 10) _wordClockLoadCode((clockword) ESP_DIEZ_B, pattern);
    if (reference == 11) _wordClockLoadCode((clockword) ESP_CINCO_B, pattern);

    // MODIFIERS
    if (index < 2) _wordClockLoadCode((clockword) ESP_CASI, pattern);
    if (index > 2) {
        if (hour_is_singular) {
            _wordClockLoadCode((clockword) ESP_PASADA, pattern);
        } else {
            _wordClockLoadCode((clockword) ESP_PASADAS, pattern);
        }
    }
    if (minute == 0) _wordClockLoadCode((clockword) ESP_EN_PUNTO, pattern);

    // HORAS
    switch (hour_12) {
        case  1: _wordClockLoadCode((clockword) ESP_UNA, pattern); break;
        case  2: _wordClockLoadCode((clockword) ESP_DOS, pattern); break;
        case  3: _wordClockLoadCode((clockword) ESP_TRES, pattern); break;
        case  4: _wordClockLoadCode((clockword) ESP_CUATRO, pattern); break;
        case  5: _wordClockLoadCode((clockword) ESP_CINCO, pattern); break;
        case  6: _wordClockLoadCode((clockword) ESP_SEIS, pattern); break;
        case  7: _wordClockLoadCode((clockword) ESP_SIETE, pattern); break;
        case  8: _wordClockLoadCode((clockword) ESP_OCHO, pattern); break;
        case  9: _wordClockLoadCode((clockword) ESP_NUEVE, pattern); break;
        case 10: _wordClockLoadCode((clockword) ESP_DIEZ, pattern); break;
        case 11: _wordClockLoadCode((clockword) ESP_ONCE, pattern); break;
        default: _wordClockLoadCode((clockword) ESP_DOCE, pattern); break;
    }

    // FRANJA HORARIA
    _wordClockLoadCode((clockword) ESP_DE_F, pattern);
    _wordClockLoadCode((clockword) ESP_LA_F, pattern);
    if (hour < 6) {
        _wordClockLoadCode((clockword) ESP_NOCHE, pattern);
    } else if (hour < 13) {
        _wordClockLoadCode((clockword) ESP_MANANA, pattern);
    } else if (hour < 21) {
        _wordClockLoadCode((clockword) ESP_TARDE, pattern);
    } else {
        _wordClockLoadCode((clockword) ESP_NOCHE, pattern);
    }

}

void _wordClockCatala(byte hour, byte minute, unsigned int * pattern) {

    /*

    00 => en punt
    01 => tocada/es
    02 => tocada/es
    03 => ben tocada/es
    04 => ben tocada/es

    05 => vora mig quart

    06 => vora mig quart
    07 => mig quart
    08 => mig quart tocat
    09 => vora un quart menys 5
    10 => un quart menys 5
    11 => un quart menys 5 tocat
    12 => un quart menys 5 tocat
    13 => un quart menys 5 ben tocat
    14 => vora un quart
    15 => un quart
    16 => un quart tocat
    17 => un quart tocat
    18 => un quart ben tocat
    19 => vora un quart i 5
    20 => un quart i 5

    21 => vora un quart i mig
    22 => un quart i mig
    23 => un quart i mig tocat
    24 => vora dos quarts menys 5
    25 => dos quarts menys 5
    26 => dos quarts menys 5 tocats
    27 => dos quarts menys 5 tocats
    28 => dos quarts menys 5 ben tocats
    29 => vora dos quarts
    30 => dos quarts
    31 => dos quarts tocats
    32 => dos quarts tocats
    33 => dos quarts ben tocats
    34 => vora dos quarts i 5
    35 => dos quarts i 5

    36 => vora dos quarts i mig
    37 => dos quarts i mig
    38 => dos quarts i mig tocats
    39 => vora tres quarts menys 5
    40 => tres quarts menys 5
    41 => tres quarts menys 5 tocats
    42 => tres quarts menys 5 tocats
    43 => tres quarts menys 5 ben tocats
    44 => vora tres quarts
    45 => tres quarts
    46 => tres quarts tocats
    47 => tres quarts tocats
    48 => tres quarts ben tocats
    49 => vora tres quarts i 5
    50 => tres quarts i 5

    51 => vora tres quart i mig
    52 => tres quarts i mig
    53 => tres quarts i mig tocats
    54 => vora menys 5
    55 => menys 5
    56 => menys 5 tocats
    57 => menys 5 tocats
    58 => menys 5 ben tocats
    59 => vora

    */

    // Code from Wordclock project (https://github.com/xoseperez/wordclock)

    // indica si l'hora es referencia a l'actual o la posterior
    bool hour_is_current = (minute < 5);

    // hora en format 12 i referida
    byte hour_12 = (hour > 12) ? hour - 12 : hour;
    if (!hour_is_current) hour_12++;
    if (hour_12 == 13) hour_12 = 1;

    // indica si l'hora va precedida de l'article (la, les) o del determinant (de, d')
    bool hour_with_article = (minute < 5) || (minute > 58);

    // indica si l'hora és plural o singular
    bool hour_is_singular = ((5 <= minute) && (minute <= 23)) || ((minute < 5) && (hour_12 == 1));

    // VERB
    if (hour_is_singular) {
        _wordClockLoadCode((clockword) CAT_ES, pattern);
    } else {
        _wordClockLoadCode((clockword) CAT_SON, pattern);
    }

    // EN PUNT
    if (minute == 0) {
        _wordClockLoadCode((clockword) CAT_EN_PUNT, pattern);
    }

    // PRIMERS MINUTS
    if ((1 <= minute) && (minute <= 4)) {
    if (hour_is_singular) {
        _wordClockLoadCode((clockword) CAT_TOCADA, pattern);
    } else {
        _wordClockLoadCode((clockword) CAT_TOCADES, pattern);
    }
    if (minute > 2) {
        _wordClockLoadCode((clockword) CAT_BEN, pattern);
    }
    }
    if (minute == 5) {
        _wordClockLoadCode((clockword) CAT_VORA, pattern);
        _wordClockLoadCode((clockword) CAT_MIG, pattern);
        _wordClockLoadCode((clockword) CAT_QUART, pattern);
    }

    // FRANJA DEL MIG
    //if ((6 <= minute) && (minute <= 53)) {
    if (6 <= minute) {

        // convenient words
        clockword tocat = (hour_is_singular) ? (clockword) CAT_TOCAT : (clockword) CAT_TOCATS;

        byte quarts = (minute - 6) / 15;
        byte index = (minute - 6) % 15;
        if (index >= 3) quarts++;

        if (quarts == 1) _wordClockLoadCode((clockword) CAT_UN_Q, pattern);
        if (quarts == 2) _wordClockLoadCode((clockword) CAT_DOS_Q, pattern);
        if (quarts == 3) _wordClockLoadCode((clockword) CAT_TRES_Q, pattern);
        if (index < 3) {
            if (quarts == 0) {
                _wordClockLoadCode((clockword) CAT_MIG, pattern);
            } else {
                _wordClockLoadCode((clockword) CAT_I_MIG, pattern);
            }
        }
        if (quarts < 2) {
            _wordClockLoadCode((clockword) CAT_QUART, pattern);
        } else if (quarts < 4) {
            _wordClockLoadCode((clockword) CAT_QUARTS, pattern);
        }

        switch (index) {

            case 0:
                _wordClockLoadCode((clockword) CAT_VORA, pattern);
                break;

            case 2:
                _wordClockLoadCode(tocat, pattern);
                break;

            case 3:
                _wordClockLoadCode((clockword) CAT_VORA, pattern);
                _wordClockLoadCode((clockword) CAT_MENYS, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                break;

            case 4:
                _wordClockLoadCode((clockword) CAT_MENYS, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                break;

            case 5:
            case 6:
                _wordClockLoadCode((clockword) CAT_MENYS, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                _wordClockLoadCode(tocat, pattern);
                break;

            case 7:
                _wordClockLoadCode((clockword) CAT_MENYS, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                _wordClockLoadCode((clockword) CAT_BEN_Q, pattern);
                _wordClockLoadCode(tocat, pattern);
                break;

            case 8:
                _wordClockLoadCode((clockword) CAT_VORA, pattern);
                break;

            case 10:
            case 11:
                _wordClockLoadCode(tocat, pattern);
                break;

            case 12:
                _wordClockLoadCode((clockword) CAT_BEN_Q, pattern);
                _wordClockLoadCode(tocat, pattern);
                break;

            case 13:
                _wordClockLoadCode((clockword) CAT_VORA, pattern);
                _wordClockLoadCode((clockword) CAT_I, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                break;

            case 14:
                _wordClockLoadCode((clockword) CAT_I, pattern);
                _wordClockLoadCode((clockword) CAT_CINC_Q, pattern);
                break;

        }

    }

    // HORES
    switch (hour_12) {
        case  1: _wordClockLoadCode((clockword) CAT_UNA, pattern); break;
        case  2: _wordClockLoadCode((clockword) CAT_DUES, pattern); break;
        case  3: _wordClockLoadCode((clockword) CAT_TRES, pattern); break;
        case  4: _wordClockLoadCode((clockword) CAT_QUATRE, pattern); break;
        case  5: _wordClockLoadCode((clockword) CAT_CINC, pattern); break;
        case  6: _wordClockLoadCode((clockword) CAT_SIS, pattern); break;
        case  7: _wordClockLoadCode((clockword) CAT_SET, pattern); break;
        case  8: _wordClockLoadCode((clockword) CAT_VUIT, pattern); break;
        case  9: _wordClockLoadCode((clockword) CAT_NOU, pattern); break;
        case 10: _wordClockLoadCode((clockword) CAT_DEU, pattern); break;
        case 11: _wordClockLoadCode((clockword) CAT_ONZE, pattern); break;
        default: _wordClockLoadCode((clockword) CAT_DOTZE, pattern); break;
    }


    // PARTICULES DE LES HORES
    if (hour_with_article) {
        if (hour_is_singular) {
            _wordClockLoadCode((clockword) CAT_LA, pattern);
        } else {
            _wordClockLoadCode((clockword) CAT_LES, pattern);
        }
    } else {
        if (hour_12 == 1) {
            _wordClockLoadCode((clockword) CAT_D_UNA, pattern);
        } else if (hour_12 == 11) {
            _wordClockLoadCode((clockword) CAT_D_ONZE, pattern);
        } else {
            _wordClockLoadCode((clockword) CAT_DE, pattern);
        }
    }

    // FRANJA HORARIA
    if (hour < 6) {
        _wordClockLoadCode((clockword) CAT_DE_F, pattern);
        _wordClockLoadCode((clockword) CAT_LA_F, pattern);
        _wordClockLoadCode((clockword) CAT_NIT, pattern);
    } else if (hour < 13) {
        _wordClockLoadCode((clockword) CAT_DEL, pattern);
        _wordClockLoadCode((clockword) CAT_MATI, pattern);
    } else if (hour < 21) {
        _wordClockLoadCode((clockword) CAT_DE_F, pattern);
        _wordClockLoadCode((clockword) CAT_LA_F, pattern);
        _wordClockLoadCode((clockword) CAT_TARDA, pattern);
    } else {
        _wordClockLoadCode((clockword) CAT_DE_F, pattern);
        _wordClockLoadCode((clockword) CAT_LA_F, pattern);
        _wordClockLoadCode((clockword) CAT_NIT, pattern);
    }

}

void _wordClearPattern() {
    for (byte i=0; i<MATRIX_HEIGHT; i++) _wordclock_time_pattern[i] = 0;
}

bool _wordLoadPattern(uint8_t language, bool force = false) {

    RtcDateTime now = rtcGet();
    int current_minute = now.Minute();
    int current_hour = now.Hour();
    if ((!force) && (current_minute == _word_previous_minute) && (current_hour == _word_previous_hour)) return false;
    _word_previous_minute = current_minute;
    _word_previous_hour = current_hour;

    _wordClearPattern();

    // Load strings
    if (language == LANGUAGE_CATALAN) {
        _wordClockCatala(current_hour, current_minute, _wordclock_time_pattern);
    } else {
        _wordClockEspanol(current_hour, current_minute, _wordclock_time_pattern);
    }

    return true;

}
void _wordLoadPatternInMatrix(unsigned int * pattern, unsigned long color) {
    for (byte y=0; y < MATRIX_HEIGHT; y++) {
        unsigned int value = pattern[y];
        for (byte x=0; x < MATRIX_WIDTH; x++) {
            if ((value & 1) == 1) {
                matrixSetPixelColor(MATRIX_WIDTH - x - 1, y, color);
            }
            value >>= 1;
        }
    }
}

// matrix configuration
#define UPDATE_MATRIX 30
#define MATRIX_BIRTH_RATIO 75
#define MATRIX_SPEED_MIN 4
#define MATRIX_SPEED_MAX 2
#define MATRIX_LENGTH_MIN 5
#define MATRIX_LENGTH_MAX 20
#define MATRIX_LIFE_MIN 10
#define MATRIX_LIFE_MAX 40
#define MATRIX_MAX_RAYS 60
#define STICKY_COUNT 1000
#define STICKY_PAUSE 10000
#define STICKY_MAX 500

CRGB getMatrixColor(byte current, byte total) {
    byte green = map(current, 0, total, 255, 0);
    byte red = current == 0 ? 255 : 0;
    return CRGB(red, green, 0);
}

byte _wordCountLEDs() {

    byte char_total = 0;

    for (byte y=0; y<MATRIX_HEIGHT; y++) {
        unsigned int row = _wordclock_time_pattern[y];
        while (row>0) {
            if (row & 1) char_total++;
            row >>= 1;
        }
    }

    return char_total;

}

void _wordRain(uint8_t language) {

    static unsigned long count = 0;
    static unsigned long next_update = millis();
    static byte current_num_rays = 0;
    static ray_struct ray[MATRIX_MAX_RAYS];
    static unsigned int countdown = 0;

    static bool sticky = false;
    static bool create = true;
    static byte char_total = 0;
    static byte char_so_far = 0;
    static unsigned int local_pattern[MATRIX_HEIGHT];

    byte i = 0;

    if (!_word_force && (next_update > millis())) return;
    _word_force = false;

    if (create && (current_num_rays < MATRIX_MAX_RAYS)) {
        bool do_create = random(0, 100) < MATRIX_BIRTH_RATIO;
        if (do_create) {
            i=0;
            while (ray[i].life > 0) i++;
            ray[i].x = random(0, MATRIX_WIDTH);
            ray[i].y = random(-5, 5);
            ray[i].speed = random(MATRIX_SPEED_MAX, MATRIX_SPEED_MIN);
            ray[i].length = random(MATRIX_LENGTH_MIN, MATRIX_LENGTH_MAX);
            ray[i].life = random(MATRIX_LIFE_MIN, MATRIX_LIFE_MAX);
            current_num_rays++;
        }
    }

    if ((!sticky) && (count > STICKY_COUNT)) {
        sticky = true;
        countdown = STICKY_MAX;
        _wordLoadPattern(language);
        char_total = _wordCountLEDs();
        for (i=0; i<MATRIX_HEIGHT; i++) local_pattern[i] = 0;
        char_so_far = 0;
    }

    matrixClear();

    for (i=0; i<MATRIX_MAX_RAYS; i++) {
        if (ray[i].life > 0) {

            // update ray position depending on speed
            if (count % ray[i].speed == 0) {
                ray[i].y = ray[i].y + 1;
                ray[i].life = ray[i].life - 1;
            }

            // get colors for each pixel
            byte start = 0;
            if (ray[i].life <= ray[i].length) {
                start = ray[i].length - ray[i].life ;
            }

            bool active = false;
            for (byte p=start; p<ray[i].length; p++) {
                int y = ray[i].y - p;
                if (0 <= y && y < MATRIX_HEIGHT) {
                    matrixSetPixelColor(ray[i].x, y, getMatrixColor(p, ray[i].length));
                }
                active |= (y < MATRIX_HEIGHT);
            }
            if (!active) ray[i].life = 0;

            // we are in sticky mode
            if (sticky) {

                byte y = ray[i].y;

                if (0 <= y && y < MATRIX_HEIGHT) {

                    // check if we have hit a led in the _wordclock_time_pattern matrix
                    unsigned int value = 1 << ray[i].x;
                    if ((_wordclock_time_pattern[y] & value) == value) {

                        // check if we have already hit this led before
                        if ((local_pattern[y] & value) != value) {

                            // kill the ray
                            ray[i].life = ray[i].length - 1;
                            char_so_far++;

                            // save it into local pattern
                            local_pattern[y] = local_pattern[y] + value;

                            // are we done?
                            if (char_so_far == char_total) {
                                create = false;
                            }

                        }

                    }

                }

            }

            // free ray if dead
            if (ray[i].life == 0) current_num_rays--;

        }
    
    }

    if (sticky) {
        if (countdown > 0) {
            if (--countdown == 0) {
                for (i=0; i<MATRIX_HEIGHT; i++) local_pattern[i] = _wordclock_time_pattern[i];
                create = false;
            }
        }
    }

    if (sticky or !create) {
        // draw hit leds
        _wordLoadPatternInMatrix(local_pattern, CRGB::Yellow);
    }

    matrixRefresh();

    if ((current_num_rays == 0) and !create) {
        sticky = false;
        create = true;
        count = 0;
        next_update += STICKY_PAUSE;
    } else {
        count++;
        next_update += UPDATE_MATRIX;
    }

}

void _wordUpdateClock() {
    matrixClear();
    _wordLoadPatternInMatrix(_wordclock_time_pattern, CRGB::Orange);
    matrixRefresh();
}

void wordClockStart() {
    _word_previous_hour = -1;
    _word_previous_minute = -1;
    _word_force = true;
}

void wordClockLoop(uint8_t language, bool moving) {
    if (moving) {
        _wordRain(language);
    } else {
        if (_wordLoadPattern(language)) {
            _wordUpdateClock();
        }
    }
}

void wordClockStatus(unsigned char state) {
    CRGB color = CRGB(0, 0, 32);
    matrixSetPixelColor(6, 2, 1 == state ? color : CRGB::Black);
    matrixSetPixelColor(4, 3, 2 == state ? color : CRGB::Black);
    matrixSetPixelColor(8, 3, 3 == state ? color : CRGB::Black);
    matrixSetPixelColor(9, 3, 4 == state ? color : CRGB::Black);
    matrixSetPixelColor(9, 8, 5 == state ? color : CRGB::Black);
}

void wordClockSetup() {

    driverRegister(
        "WordClock Català", 
        wordClockStart,
        []() {
            wordClockLoop(LANGUAGE_CATALAN, false);
        }, 
        NULL,
        wordClockStatus, 
        driverCommonProgress
    );

    driverRegister(
        "WordClock Español", 
        wordClockStart,
        []() {
            wordClockLoop(LANGUAGE_SPANISH, false);
        }, 
        NULL, 
        wordClockStatus, 
        driverCommonProgress
    );

    driverRegister(
        "WordClock Català Matrix", 
        wordClockStart,
        []() {
            wordClockLoop(LANGUAGE_CATALAN, true);
        }, 
        NULL,
        wordClockStatus, 
        driverCommonProgress
    );

    driverRegister(
        "WordClock Español Matrix", 
        wordClockStart,
        []() {
            wordClockLoop(LANGUAGE_SPANISH, true);
        }, 
        NULL, 
        wordClockStatus, 
        driverCommonProgress
    );

}

#endif
