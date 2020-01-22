/*

slices

Copyright (C) 2017-2020 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "config/all.h"
#include <Wire.h>

// -----------------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------------

void heartbeat() {

    static unsigned long last_uptime = 0;
    static unsigned char uptime_overflows = 0;

    if (millis() < last_uptime) ++uptime_overflows;
    last_uptime = millis();
    unsigned long uptime_seconds = uptime_overflows * (UPTIME_OVERFLOW / 1000) + (last_uptime / 1000);
    unsigned int free_heap = ESP.getFreeHeap();

    DEBUG_MSG_P(PSTR("[MAIN] Uptime: %ld seconds\n"), uptime_seconds);
    DEBUG_MSG_P(PSTR("[MAIN] Free heap: %d bytes\n"), free_heap);
    #if ENABLE_ADC_VCC
        DEBUG_MSG_P(PSTR("[MAIN] Power: %d mV\n"), ESP.getVcc());
    #endif

    #if ENABLE_MQTT

        #if (MQTT_REPORT_INTERVAL)
            mqttSend(MQTT_TOPIC_INTERVAL, HEARTBEAT_INTERVAL / 1000);
        #endif
        #if (MQTT_REPORT_APP)
            mqttSend(MQTT_TOPIC_APP, APP_NAME);
        #endif
        #if (MQTT_REPORT_VERSION)
            mqttSend(MQTT_TOPIC_VERSION, APP_VERSION);
        #endif
        #if (MQTT_REPORT_HOSTNAME)
            mqttSend(MQTT_TOPIC_HOSTNAME, getSetting("hostname").c_str());
        #endif
        #if (MQTT_REPORT_IP)
            mqttSend(MQTT_TOPIC_IP, getIP().c_str());
        #endif
        #if (MQTT_REPORT_MAC)
            mqttSend(MQTT_TOPIC_MAC, WiFi.macAddress().c_str());
        #endif
        #if (MQTT_REPORT_RSSI)
            mqttSend(MQTT_TOPIC_RSSI, String(WiFi.RSSI()).c_str());
        #endif
        #if (MQTT_REPORT_UPTIME)
            mqttSend(MQTT_TOPIC_UPTIME, String(uptime_seconds).c_str());
        #endif
        #if (MQTT_REPORT_FREEHEAP)
            mqttSend(MQTT_TOPIC_FREEHEAP, String(free_heap).c_str());
        #endif
        #if (MQTT_REPORT_STATUS)
            mqttSend(MQTT_TOPIC_STATUS, MQTT_STATUS_ONLINE);
        #endif

    #endif

}

void hardwareSetup() {
    Serial.begin(SERIAL_BAUDRATE);
    Wire.begin(SDA_PIN, SCL_PIN);
}

void hardwareLoop() {

    // Heartbeat
    static unsigned long last_uptime = 0;
    if ((millis() - last_uptime > HEARTBEAT_INTERVAL) || (last_uptime == 0)) {
        last_uptime = millis();
        heartbeat();
    }

}

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void welcome() {

    DEBUG_MSG_P(PSTR("%s %s\n"), (char *) APP_NAME, (char *) APP_VERSION);
    DEBUG_MSG_P(PSTR("%s\n%s\n\n"), (char *) APP_AUTHOR, (char *) APP_WEBSITE);
    DEBUG_MSG_P(PSTR("ChipID: %06X\n"), ESP.getChipId());
    DEBUG_MSG_P(PSTR("CPU frequency: %d MHz\n"), ESP.getCpuFreqMHz());
    DEBUG_MSG_P(PSTR("Last reset reason: %s\n"), (char *) ESP.getResetReason().c_str());
    DEBUG_MSG_P(PSTR("Memory size (SDK): %d bytes\n"), ESP.getFlashChipSize());
    DEBUG_MSG_P(PSTR("Memory size (CHIP): %d bytes\n"), ESP.getFlashChipRealSize());
    DEBUG_MSG_P(PSTR("Free heap: %d bytes\n"), ESP.getFreeHeap());
    DEBUG_MSG_P(PSTR("Firmware size: %d bytes\n"), ESP.getSketchSize());
    DEBUG_MSG_P(PSTR("Free firmware space: %d bytes\n"), ESP.getFreeSketchSpace());
    DEBUG_MSG_P(PSTR("\n\n"));

}

void setup() {

    hardwareSetup();
    welcome();

    eepromSetup();
    settingsSetup();
    if (getSetting("hostname").length() == 0) {
        setSetting("hostname", getIdentifier());
        saveSettings();
    }

    i2c_scanner();

    rtcSetup();
    webSetup();
    buttonSetup();
    soundSetup();

    delay(500);

    wifiSetup();
    otaSetup();
    terminalSetup();
    #if ENABLE_MDNS
    mdnsServerSetup();
    #endif
    #if ENABLE_MQTT
    mqttSetup();
    #endif
    ntpSetup();

    matrixSetup();

    // drivers
    #if ENABLE_DRIVER_BASIC
        basicClockSetup();
    #endif
    #if ENABLE_DRIVER_SCROLL
        scrollClockSetup();
    #endif
    #if ENABLE_DRIVER_WORDCLOCK
        wordClockSetup();
    #endif
    #if ENABLE_DRIVER_FIBONACCI
        fibonacciClockSetup();
    #endif
    #if ENABLE_DRIVER_BINARY
        binaryClockSetup();
    #endif
    #if ENABLE_DRIVER_GAME_OF_LIFE
        gameOfLifeSetup();
    #endif
    #if ENABLE_DRIVER_CANVAS
        canvasSetup();
    #endif
    #if ENABLE_DRIVER_CIRCLE
        circleClockSetup();
    #endif
    #if ENABLE_DRIVER_GAUGE
        gaugeClockSetup();
    #endif

    driverSetup();

}

void loop() {

    eepromLoop();
    hardwareLoop();
    buttonLoop();
    wifiLoop();
    otaLoop();
    #if ENABLE_MQTT
    mqttLoop();
    #endif
    ntpLoop();
    terminalLoop();
    
    driverLoop();

}
