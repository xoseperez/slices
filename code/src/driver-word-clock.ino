/*

WORD CLOCK DRIVER

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Adafruit_NeoMatrix.h>

#define LANGUAGE_CATALA     1
#define LANGUAGE_ESPANOL    2
#define LANGUAGE_ENGLISH    3

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

String wordClockCatalan(byte hour, byte minute) {

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

void wordClockStart() {

    RtcDateTime now = rtcGet();
    int currentHour = now.Hour();
    int currentMinute = now.Minute();

    String text = wordClockCatalan(currentHour, currentMinute);

    matrixScroll(3, text.c_str(), true, wordClockStart);

}

void wordClockSetup() {
    driverRegister("Word clock", wordClockStart, NULL, NULL);
}
