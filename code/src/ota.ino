/*

OTA MODULE

Copyright (C) 2016-2020 by Xose Pérez <xose dot perez at gmail dot com>

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
        uint8_t value = (progress / (total / 100));
        DEBUG_MSG_P(PSTR("[OTA] Progress: %u%%\r"), value);
        driverProgress(value);
        matrixRefresh(false);
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
