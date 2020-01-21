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
    ArduinoOTA.setHostname(getHostName().c_str());
    ArduinoOTA.setPassword(getAdminPass().c_str());
}

void otaSetup() {

    otaConfigure();

    ArduinoOTA.onStart([]() {
        DEBUG_MSG_P(PSTR("[OTA] Start\n"));
        wsSend("{\"message\": \"OTA update started\"}");
        driverStop();
    });

    ArduinoOTA.onEnd([]() {
        DEBUG_MSG_P(PSTR("\n[OTA] End\n"));
        wsSend("{\"action\": \"reload\"}");
        delay(100);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

        #ifdef DEVICE_FIBONACCI
            
            long num = MATRIX_WIDTH * progress / total;
            for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
                if (num >= x) matrixSetPixelColor(x, MATRIX_HEIGHT-1, CRGB::Green);
            }

        #elif CIRCLE_SIZE > 0
            
            long num = CIRCLE_SIZE * progress / total;
            for (unsigned char x=0; x<CIRCLE_SIZE; x++) {
                if (num >= x) matrixSetPixelColor(CIRCLE_START + x, CRGB::Green);
            }

        #else
            
            long num = MATRIX_WIDTH * MATRIX_HEIGHT * progress / total;
            for (unsigned char y=0; y<MATRIX_HEIGHT; y++) {
                if (num >= y) {
                    for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
                        matrixSetPixelColor(x, MATRIX_HEIGHT-y-1, CRGB::Green);
                    }
                }
            }

        #endif

        matrixRefresh(false);

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
