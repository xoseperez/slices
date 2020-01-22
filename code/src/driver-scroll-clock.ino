/*

SCROLL CLOCK DRIVER

Copyright (C) 2017-2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_SCROLL

#include <FastLED_GFX.h>

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

String _scrollClockSpanish(byte hour, byte minute) {

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

    String output;

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
        output += "ES LA";
    } else {
        output += "SON LAS";
    }

    // HORAS
    switch (hour_12) {
        case  1: output += " UNA"; break;
        case  2: output += " DOS"; break;
        case  3: output += " TRES"; break;
        case  4: output += " CUATRO"; break;
        case  5: output += " CINCO"; break;
        case  6: output += " SEIS"; break;
        case  7: output += " SIETE"; break;
        case  8: output += " OCHO"; break;
        case  9: output += " NUEVE"; break;
        case 10: output += " DIEZ"; break;
        case 11: output += " ONCE"; break;
        default: output += " DOCE"; break;
    }

    // BLOCKS
    byte reference = ((minute + 2) / 5) % 12;
    byte index = (minute + 2) % 5;
    if (reference == 0) {
        // NOP
    } else if (reference < 7) {
        output += " Y";
    } else {
        output += " MENOS";
        reference = 12 - reference;
    }
    if (reference ==  1) output += " CINCO";
    if (reference ==  2) output += " DIEZ";
    if (reference ==  3) output += " CUARTO";
    if (reference ==  4) output += " VEINTE";
    if (reference ==  5) output += " VEINTICINCO";
    if (reference ==  6) output += " MEDIA";

    // MODIFIERS
    if (index < 2) output += " CASI";
    if (index > 2) {
        if (hour_is_singular) {
            output += " PASADA";
        } else {
            output += " PASADAS";
        }
    }
    if (minute == 0) output += " EN PUNTO";

    // FRANJA HORARIA
    output += " DE LA";
    if (hour < 6) {
        output += " NOCHE";
    } else if (hour < 13) {
        output += " MAÑANA";
    } else if (hour < 21) {
        output += " TARDE";
    } else {
        output += " NOCHE";
    }

    return output;

}

String _scrollClockCatalan(byte hour, byte minute) {

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

    String output;

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
        output += "ES";
    } else {
        output += "SON";
    }

    if (minute == 5) {
        output += " VORA MIG QUART";
    }

    // FRANJA DEL MIG
    if (6 <= minute) {

        // convenient words
        String tocat = hour_is_singular ? String(" TOCAT") : String(" TOCATS");

        byte quarts = (minute - 6) / 15;
        byte index = (minute - 6) % 15;
        if (index >= 3) quarts++;

        if (index == 0 || index == 3 || index == 8 || index == 13) output += " VORA";

        if (quarts == 0 && index < 3) output += " MIG";
        if (quarts == 1) output += " UN";
        if (quarts == 2) output += " DOS";
        if (quarts == 3) output += " TRES";
        if (quarts < 2) {
            output += " QUART";
        } else if (quarts < 4) {
            output += " QUARTS";
        }
        if (quarts > 0 && index < 3) output += " I MIG";

        if (index == 2 || index == 10 || index == 11) output += tocat;
        if (index == 3 || index == 4) output += " MENYS CINC";
        if (index == 5 || index == 6) output += " MENYS CINC" + tocat;
        if (index == 7) output += " MENYS CINC BEN" + tocat;
        if (index == 12) output += " BEN" + tocat;
        if (index == 13 || index == 14) output += " I CINC";

    }

    // PARTICULES DE LES HORES
    if (hour_with_article) {
        if (hour_is_singular) {
            output += " LA";
        } else {
            output += " LES";
        }
    } else {
        if ((hour_12 != 1) && (hour_12 != 11)) {
            output += " DE";
        }
    }

    // HORES
    if (hour_12 ==  1) output += (hour_with_article) ? " UNA" : " D'UNA";
    if (hour_12 ==  2) output += " DUES";
    if (hour_12 ==  3) output += " TRES";
    if (hour_12 ==  4) output += " QUATRE";
    if (hour_12 ==  5) output += " CINC";
    if (hour_12 ==  6) output += " SIS";
    if (hour_12 ==  7) output += " SET";
    if (hour_12 ==  8) output += " VUIT";
    if (hour_12 ==  9) output += " NOU";
    if (hour_12 == 10) output += " DEU";
    if (hour_12 == 11) output += (hour_with_article) ? " ONZE" : " D'ONZE";
    if (hour_12 == 12) output += " DOTZE";

    // EN PUNT
    if (minute == 0) output += " EN PUNT";

    // PRIMERS MINUTS
    if ((1 <= minute) && (minute <= 4)) {
        if (minute > 2) output += " BEN";
        if (hour_is_singular) {
            output += " TOCADA";
        } else {
            output += " TOCADES";
        }
    }

    // FRANJA HORARIA
    if (hour < 5) {
        output += " DE LA NIT";
    } else if (hour < 12) {
        output += " DEL MATI";
    } else if (hour < 20) {
        output += " DE LA TARDA";
    } else if (hour < 22) {
        output += " DEL VESPRE";
    } else {
        output += " DE LA NIT";
    }

    return output;

}

String _scrollClockEnglish(byte hour, byte minute) {

    String output;

    // indica si l'hora es referencia a l'actual o la posterior
    bool hour_is_current = (minute < 31);

    // hora en format 12 i referida
    byte hour_12 = (hour > 12) ? hour - 12 : hour;
    if (!hour_is_current) hour_12++;
    if (hour_12 == 13) hour_12 = 1;

    // minuts referits a l'hora
    if (minute > 30) minute = 60 - minute;

    // VERB
    output += "IT'S";

    if (minute > 0) {

        if (1 == minute) output += " ONE";
        if (2 == minute) output += " TWO";
        if (3 == minute) output += " THREE";
        if (4 == minute) output += " FOUR";
        if (5 == minute) output += " FIVE";
        if (6 == minute) output += " SIX";
        if (7 == minute) output += " SEVEN";
        if (8 == minute) output += " EIGHT";
        if (9 == minute) output += " NINE";
        if (10 == minute) output += " TEN";
        if (11 == minute) output += " ELEVEN";
        if (12 == minute) output += " TWELVE";
        if (13 == minute) output += " THIRTEEN";
        if (14 == minute) output += " FOURTEEN";
        if (15 == minute) output += " A QUARTER";
        if (16 == minute) output += " SIXTEEN";
        if (17 == minute) output += " SEVENTEEN";
        if (18 == minute) output += " EIGHTEEN";
        if (19 == minute) output += " NINETEEN";
        if (20 == minute) output += " TWENTY";
        if (21 == minute) output += " TWENTYONE";
        if (22 == minute) output += " TWENTYTWO";
        if (23 == minute) output += " TWENTYTHREE";
        if (24 == minute) output += " TWENTYFOUR";
        if (25 == minute) output += " TWENTYFIVE";
        if (26 == minute) output += " TWENTYSIX";
        if (27 == minute) output += " TWENTYSEVEN";
        if (28 == minute) output += " TWENTYEIGHT";
        if (29 == minute) output += " TWENTYNINE";
        if (30 == minute) output += " HALF";

        if (hour_is_current) {
            output += " PAST";
        } else {
            output += " TO";
        }

    }

    // Hour
    if (0 == hour_12) output += " MIDNIGHT";
    if (1 == hour_12) output += " ONE";
    if (2 == hour_12) output += " TWO";
    if (3 == hour_12) output += " THREE";
    if (4 == hour_12) output += " FOUR";
    if (5 == hour_12) output += " FIVE";
    if (6 == hour_12) output += " SIX";
    if (7 == hour_12) output += " SEVEN";
    if (8 == hour_12) output += " EIGHT";
    if (9 == hour_12) output += " NINE";
    if (10 == hour_12) output += " TEN";
    if (11 == hour_12) output += " ELEVEN";
    if (12 == hour_12) output += " NOON";


    // EN PUNT
    if ((minute == 0) && (0 != hour_12) && (12 != hour_12)) {
        output += " O'CLOCK'";
    }

    // FRANJA HORARIA
    if (hour < 5) {
        output += " IN THE NIGHT";
    } else if (hour < 12) {
        output += " IN THE MORNING";
    } else if (hour < 20) {
        output += " IN THE AFTERNOON";
    } else if (hour < 22) {
        output += " IN THE EVENING";
    } else {
        output += " IN THE NIGHT";
    }

    return output;

}

void scrollClockStart(uint8_t language) {
    
    RtcDateTime now = rtcGet();
    int currentHour = now.Hour();
    int currentMinute = now.Minute();
    
    String text;
    if (LANGUAGE_CATALAN == language) {
        text = _scrollClockCatalan(currentHour, currentMinute);
    } else if (LANGUAGE_SPANISH == language) {
        text = _scrollClockSpanish(currentHour, currentMinute);
    } else if (LANGUAGE_ENGLISH == language) {
        text = _scrollClockEnglish(currentHour, currentMinute);
    } else {
        text = String("");
    }
    
    matrixScroll(0, text.c_str(), true, [language]() {
        scrollClockStart(language);
    });

}

void scrollClockSetup() {
    driverRegister("Scroll Clock Catalan", []() { scrollClockStart(LANGUAGE_CATALAN); }, NULL, NULL, driverCommonStatus, driverCommonProgress);
    driverRegister("Scroll Clock Spanish", []() { scrollClockStart(LANGUAGE_SPANISH); }, NULL, NULL, driverCommonStatus, driverCommonProgress);
    driverRegister("Scroll Clock English", []() { scrollClockStart(LANGUAGE_ENGLISH); }, NULL, NULL, driverCommonStatus, driverCommonProgress);
}

#endif
