/*

MDNS MODULE

Copyright (C) 2017-2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// mDNS Server
// -----------------------------------------------------------------------------

#if ENABLE_MDNS

#include <ESP8266mDNS.h>

#if ENABLE_MQTT

void _mdnsFindMQTT() {
    int count = MDNS.queryService("mqtt", "tcp");
    DEBUG_MSG_P(PSTR("[MQTT] MQTT brokers found: %d\n"), count);
    for (int i=0; i<count; i++) {
        DEBUG_MSG_P(PSTR("[MQTT] Broker at %s:%d\n"), MDNS.IP(i).toString().c_str(), MDNS.port(i));
        mqttSetBrokerIfNone(MDNS.IP(i), MDNS.port(i));
    }
}

#endif

void _mdnsServerStart() {
    if (MDNS.begin((char *) getSetting("hostname").c_str())) {
        DEBUG_MSG_P(PSTR("[MDNS] OK\n"));
    } else {
        DEBUG_MSG_P(PSTR("[MDNS] FAIL\n"));
    }
}

// -----------------------------------------------------------------------------

void mdnsServerSetup() {

    #if ENABLE_WEB
        MDNS.addService("http", "tcp", getSetting("webPort", WEBSERVER_PORT).toInt());
    #endif

    // Public ESPurna related txt for OTA discovery
    MDNS.addServiceTxt("arduino", "tcp", "app_name", APP_NAME);
    MDNS.addServiceTxt("arduino", "tcp", "app_version", APP_VERSION);
    MDNS.addServiceTxt("arduino", "tcp", "build_date", buildTime());
    MDNS.addServiceTxt("arduino", "tcp", "mac", WiFi.macAddress());
    MDNS.addServiceTxt("arduino", "tcp", "target_board", DEVICE_NAME);
    {
        char buffer[6] = {0};
        itoa(ESP.getFlashChipRealSize() / 1024, buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "mem_size", (const char *) buffer);
    }
    {
        char buffer[6] = {0};
        itoa(ESP.getFlashChipSize() / 1024, buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "sdk_size", (const char *) buffer);
    }
    {
        char buffer[6] = {0};
        itoa(ESP.getFreeSketchSpace(), buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "free_space", (const char *) buffer);
    }

    wifiRegister([](justwifi_messages_t code, char * parameter) {

        if (code == MESSAGE_CONNECTED) {
            _mdnsServerStart();
            #if MQTT_SUPPORT
                _mdnsFindMQTT();
            #endif // MQTT_SUPPORT
        }

        if (code == MESSAGE_ACCESSPOINT_CREATED) {
            _mdnsServerStart();
        }

    });

}

#endif // MDNS_SERVER_SUPPORT
