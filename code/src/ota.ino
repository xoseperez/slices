/*

OTA MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "ArduinoOTA.h"

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

void otaConfigure() {
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setHostname(getSetting("hostname", HOSTNAME).c_str());
    ArduinoOTA.setPassword(getSetting("adminPass", ADMIN_PASS).c_str());
}

void otaSetup() {

    otaConfigure();

    ArduinoOTA.onStart([]() {
        DEBUG_MSG_P(PSTR("[OTA] Start\n"));
        wsSend("{\"message\": \"OTA update started\"}");
        matrixClear();
        matrixStopScroll();
    });

    ArduinoOTA.onEnd([]() {
        DEBUG_MSG_P(PSTR("\n[OTA] End\n"));
        wsSend("{\"action\": \"reload\"}");
        delay(100);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

        Adafruit_NeoMatrix * matrix = getMatrix();
        unsigned long color = matrix->Color(10, 255, 10);
        #ifdef FIBONACCI
            long num = MATRIX_WIDTH * progress / total;
            for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
                if (num-- > 0) matrix->drawPixel(x, MATRIX_HEIGHT-1, color);
            }
        #else
            long num = MATRIX_WIDTH * MATRIX_HEIGHT * progress / total;
            for (unsigned char y=0; y<MATRIX_HEIGHT; y++) {
                for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
                    if (num-- > 0) matrix->drawPixel(x, MATRIX_HEIGHT-y-1, color);
                }
            }
        #endif
        matrix->show();

        DEBUG_MSG_P(PSTR("[OTA] Progress: %u%%\r"), (progress / (total / 100)));

    });

    ArduinoOTA.onError([](ota_error_t error) {
        #if DEBUG_PORT
            DEBUG_MSG_P(PSTR("\n[OTA] Error[%u]: "), error);
            if (error == OTA_AUTH_ERROR) DEBUG_MSG_P(PSTR("Auth Failed\n"));
            else if (error == OTA_BEGIN_ERROR) DEBUG_MSG_P(PSTR("Begin Failed\n"));
            else if (error == OTA_CONNECT_ERROR) DEBUG_MSG_P(PSTR("Connect Failed\n"));
            else if (error == OTA_RECEIVE_ERROR) DEBUG_MSG_P(PSTR("Receive Failed\n"));
            else if (error == OTA_END_ERROR) DEBUG_MSG_P(PSTR("End Failed\n"));
        #endif
    });

    ArduinoOTA.begin();

}

void otaLoop() {
    ArduinoOTA.handle();
}
