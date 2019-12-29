/*

WIFI MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "JustWifi.h"

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

String getIP() {
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP().toString();
    }
    return WiFi.localIP().toString();
}

String getNetwork() {
    if (WiFi.getMode() == WIFI_AP) {
        return jw.getAPSSID();
    }
    return WiFi.SSID();
}

void wifiDisconnect() {
    #if ENABLE_POW
        powEnable(false);
    #endif
    jw.disconnect();
}

void resetConnectionTimeout() {
    jw.resetReconnectTimeout();
}

bool wifiConnected() {
    return jw.connected();
}

bool createAP() {
    jw.disconnect();
    jw.resetReconnectTimeout();
    jw.enableAP(true);
    return true;
}

void wifiConfigure() {

    jw.setHostname(getSetting("hostname", HOSTNAME).c_str());
    jw.setSoftAP(getSetting("hostname", HOSTNAME).c_str(), getSetting("adminPass", ADMIN_PASS).c_str());
    jw.setReconnectTimeout(WIFI_RECONNECT_INTERVAL);
    jw.enableAPFallback(true);
    jw.cleanNetworks();

    int i;
    for (i = 0; i< WIFI_MAX_NETWORKS; i++) {
        if (getSetting("ssid" + String(i)).length() == 0) break;
        if (getSetting("ip" + String(i)).length() == 0) {
            jw.addNetwork(
                getSetting("ssid" + String(i)).c_str(),
                getSetting("pass" + String(i)).c_str()
            );
        } else {
            jw.addNetwork(
                getSetting("ssid" + String(i)).c_str(),
                getSetting("pass" + String(i)).c_str(),
                getSetting("ip" + String(i)).c_str(),
                getSetting("gw" + String(i)).c_str(),
                getSetting("mask" + String(i)).c_str(),
                getSetting("dns" + String(i)).c_str()
            );
        }
    }

    // Scan for best network only if we have more than 1 defined
    jw.enableScan(i>1);

}

void wifiDebug(WiFiMode_t modes) {

    #if DEBUG_SUPPORT
    bool footer = false;

    if (((modes & WIFI_STA) > 0) && ((WiFi.getMode() & WIFI_STA) > 0)) {

        DEBUG_MSG_P(PSTR("[WIFI] ------------------------------------- MODE STA\n"));
        DEBUG_MSG_P(PSTR("[WIFI] SSID  %s\n"), WiFi.SSID().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] IP    %s\n"), WiFi.localIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MAC   %s\n"), WiFi.macAddress().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] GW    %s\n"), WiFi.gatewayIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] DNS   %s\n"), WiFi.dnsIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MASK  %s\n"), WiFi.subnetMask().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] HOST  http://%s.local\n"), WiFi.hostname().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] BSSID %s\n"), WiFi.BSSIDstr().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] CH    %d\n"), WiFi.channel());
        DEBUG_MSG_P(PSTR("[WIFI] RSSI  %d\n"), WiFi.RSSI());
        footer = true;

    }

    if (((modes & WIFI_AP) > 0) && ((WiFi.getMode() & WIFI_AP) > 0)) {
        DEBUG_MSG_P(PSTR("[WIFI] -------------------------------------- MODE AP\n"));
        DEBUG_MSG_P(PSTR("[WIFI] SSID  %s\n"), _wifiSoftAPSSID().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] PASS  %s\n"), _wifiSoftAPPSK().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] IP    %s\n"), WiFi.softAPIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MAC   %s\n"), WiFi.softAPmacAddress().c_str());
        footer = true;
    }

    if (WiFi.getMode() == 0) {
        DEBUG_MSG_P(PSTR("[WIFI] ------------------------------------- MODE OFF\n"));
        DEBUG_MSG_P(PSTR("[WIFI] No connection\n"));
        footer = true;
    }

    if (footer) {
        DEBUG_MSG_P(PSTR("[WIFI] ----------------------------------------------\n"));
    }
    #endif //DEBUG_SUPPORT

}

void wifiDebug() {
    wifiDebug(WIFI_AP_STA);
}

void _wifiUpdateSoftAP() {
    if (WiFi.softAPgetStationNum() == 0) {
        jw.setSoftAP(getSetting("hostname").c_str(), getAdminPass().c_str());
    }
}

#ifdef DEBUG_PORT

void _wifiDebugCallback(justwifi_messages_t code, char * parameter) {

    // -------------------------------------------------------------------------

    if (code == MESSAGE_SCANNING) {
        DEBUG_MSG_P(PSTR("[WIFI] Scanning\n"));
    }

    if (code == MESSAGE_SCAN_FAILED) {
        DEBUG_MSG_P(PSTR("[WIFI] Scan failed\n"));
    }

    if (code == MESSAGE_NO_NETWORKS) {
        DEBUG_MSG_P(PSTR("[WIFI] No networks found\n"));
    }

    if (code == MESSAGE_NO_KNOWN_NETWORKS) {
        DEBUG_MSG_P(PSTR("[WIFI] No known networks found\n"));
    }

    if (code == MESSAGE_FOUND_NETWORK) {
        DEBUG_MSG_P(PSTR("[WIFI] %s\n"), parameter);
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_CONNECTING) {
        DEBUG_MSG_P(PSTR("[WIFI] Connecting to %s\n"), parameter);
    }

    if (code == MESSAGE_CONNECT_WAITING) {
        // too much noise
    }

    if (code == MESSAGE_CONNECT_FAILED) {
        DEBUG_MSG_P(PSTR("[WIFI] Could not connect to %s\n"), parameter);
    }

    if (code == MESSAGE_CONNECTED) {
        wifiDebug(WIFI_STA);
    }

    if (code == MESSAGE_DISCONNECTED) {
        DEBUG_MSG_P(PSTR("[WIFI] Disconnected\n"));
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_ACCESSPOINT_CREATING) {
        DEBUG_MSG_P(PSTR("[WIFI] Creating access point\n"));
    }

    if (code == MESSAGE_ACCESSPOINT_CREATED) {
        wifiDebug(WIFI_AP);
    }

    if (code == MESSAGE_ACCESSPOINT_FAILED) {
        DEBUG_MSG_P(PSTR("[WIFI] Could not create access point\n"));
    }

    if (code == MESSAGE_ACCESSPOINT_DESTROYED) {
        _wifiUpdateSoftAP();
        DEBUG_MSG_P(PSTR("[WIFI] Access point destroyed\n"));
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_WPS_START) {
        DEBUG_MSG_P(PSTR("[WIFI] WPS started\n"));
    }

    if (code == MESSAGE_WPS_SUCCESS) {
        DEBUG_MSG_P(PSTR("[WIFI] WPS succeded!\n"));
    }

    if (code == MESSAGE_WPS_ERROR) {
        DEBUG_MSG_P(PSTR("[WIFI] WPS failed\n"));
    }

    // ------------------------------------------------------------------------

    if (code == MESSAGE_SMARTCONFIG_START) {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config started\n"));
    }

    if (code == MESSAGE_SMARTCONFIG_SUCCESS) {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config succeded!\n"));
    }

    if (code == MESSAGE_SMARTCONFIG_ERROR) {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config failed\n"));
    }

}

#endif

void wifiRegister(wifi_callback_f callback) {
    jw.subscribe(callback);
}

void wifiSetup() {

    WiFi.persistent(false);
    wifiConfigure();

    // Message callbacks
	#ifdef DEBUG_PORT
        wifiRegister(_wifiDebugCallback);
    #endif

    wifiRegister([](justwifi_messages_t code, char * parameter) {

        // NTP connection reset
        if (code == MESSAGE_CONNECTED) {
            ntpConnect();
        }

        // Display notifications
        if (code == MESSAGE_CONNECTED || code == MESSAGE_DISCONNECTED || code == MESSAGE_ACCESSPOINT_CREATED) {
            matrixRefresh();
        }

    });

}

void wifiLoop() {
    jw.loop();
}
