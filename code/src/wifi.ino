/*

WIFI MODULE

Copyright (C) 2016-2024 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "JustWifi.h"
#include "Ticker.h"

Ticker _wifi_off_ticker;
uint8_t _wifi_previous_hour = 25;
uint8_t _wifi_sync_mode = 0;
uint8_t _wifi_hours_left = 0;
bool _wifi_wps_in_progress = false;

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

String _wifiSoftAPSSID() {
    struct softap_config config;
    wifi_softap_get_config(&config);

    char* name = reinterpret_cast<char*>(config.ssid);
    char ssid[sizeof(config.ssid) + 1];
    memcpy(ssid, name, sizeof(config.ssid));
    ssid[sizeof(config.ssid)] = '\0';

    return String(ssid);
}

String _wifiSoftAPPSK() {
    struct softap_config config;
    wifi_softap_get_config(&config);

    char* pass = reinterpret_cast<char*>(config.password);
    char psk[sizeof(config.password) + 1];
    memcpy(psk, pass, sizeof(config.password));
    psk[sizeof(config.password)] = '\0';

    return String(psk);
}

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

bool startWPS() {
    WiFi.disconnect();
    jw.startWPS();
    return true;
}

void preOff() {
    if (wsCount() == 0) {
        wifiOff();
    } else {
        DEBUG_MSG_P(PSTR("[WIFI] Keeping WiFi on since there is a client connected\n"));
        if (WIFI_OFF_AFTER>0) _wifi_off_ticker.once(WIFI_OFF_AFTER, preOff);
    }
}

bool wifiOff() {
    jw.enableAPFallback(false);
    jw.turnOff();
    return true;
}

bool wifiOn(bool open_ap) {
    jw.turnOn();
    jw.enableAPFallback(open_ap);
    return true;
}

void wifiConfigure() {

    _wifi_sync_mode = getSetting("timeSync", TIME_SYNC_MODE).toInt();

    jw.setHostname(getHostName().c_str());
    if (hasSetting("adminPass")) {
        jw.setSoftAP(getHostName().c_str(), getAdminPass().c_str());
    } else {
        jw.setSoftAP(getHostName().c_str());
    }
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

    #ifdef DEBUG_PORT
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
    #endif //DEBUG_PORT

}

void wifiDebug() {
    wifiDebug(WIFI_AP_STA);
}

void _wifiUpdateSoftAP() {
    if (WiFi.softAPgetStationNum() == 0) {
        if (hasSetting("adminPass")) {
            jw.setSoftAP(getHostName().c_str(), getAdminPass().c_str());
        } else {
            jw.setSoftAP(getHostName().c_str());
        }
    }
}

void _wifiSave() {

    String ssid = WiFi.SSID();
    String pass = WiFi.psk();

    // Look for the same SSID
    uint8_t count = 0;
    while (count < WIFI_MAX_NETWORKS) {
        if (!hasSetting("ssid", count)) break;
        if (ssid.equals(getSetting("ssid", count, ""))) break;
        count++;
    }

    // If we have reached the max we overwrite the first one
    if (WIFI_MAX_NETWORKS == count) count = 0;

    setSetting("ssid", count, ssid);
    setSetting("pass", count, pass);

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
        _wifi_wps_in_progress = false;
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config failed\n"));
    }

    // ------------------------------------------------------------------------

    if (code == MESSAGE_TURNING_ON) {
        DEBUG_MSG_P(PSTR("[WIFI] Turning WiFi ON\n"));
    }

    if (code == MESSAGE_TURNING_OFF) {
        DEBUG_MSG_P(PSTR("[WIFI] Turning WiFi OFF\n"));
    }

}

#endif

void wifiRegister(wifi_callback_f callback) {
    jw.subscribe(callback);
}

void _wifiInitCommand() {

    terminalRegisterCommand(F("WIFI"), [](Embedis* e) {
        wifiDebug();
        terminalOK();
    });

    terminalRegisterCommand(F("WIFI.ON"), [](Embedis* e) {

        bool ap_enable = true;

        if (e->argc == 2) {
            String param = String(e->argv[1]);
            ap_enable = param.toInt() == 1;
        }

        wifiOn(ap_enable);
        terminalOK();

    });

    terminalRegisterCommand(F("WIFI.OFF"), [](Embedis* e) {
        wifiOff();
        terminalOK();
    });

    terminalRegisterCommand(F("WIFI.WPS"), [](Embedis* e) {
        startWPS();
        terminalOK();
    });

}

void wifiSetup() {

    WiFi.persistent(false);
    wifiConfigure();

    // Message callbacks
	#ifdef DEBUG_PORT
        wifiRegister(_wifiDebugCallback);
    #endif

    // Terminal
    _wifiInitCommand();

    wifiRegister([](justwifi_messages_t code, char * parameter) {

        if (code == MESSAGE_CONNECTING) {
            stateSet(STATE_CONNECTING);
        }

        if (code == MESSAGE_CONNECT_FAILED) {
            stateSet(STATE_ERROR);
        }

        if (code == MESSAGE_CONNECTED) {
            stateSet(STATE_CONNECTED);
            ntpConnect();
            matrixRefresh();
            if (WIFI_OFF_AFTER>0) _wifi_off_ticker.once(WIFI_OFF_AFTER, preOff);
        }

        if (code == MESSAGE_DISCONNECTED) {
            stateSet(STATE_IDLE);
            ntpDisconnect();
            matrixRefresh();
        }

        if (code == MESSAGE_ACCESSPOINT_CREATED) {
            stateSet(STATE_AP);
            matrixRefresh();
            if (WIFI_OFF_AFTER>0) _wifi_off_ticker.once(WIFI_OFF_AFTER, preOff);
        }

        if (code == MESSAGE_ACCESSPOINT_FAILED) {
            stateSet(STATE_ERROR);
        }

        if (code == MESSAGE_ACCESSPOINT_DESTROYED) {
            _wifiUpdateSoftAP();
            stateSet(STATE_IDLE);
        }

        if (code == MESSAGE_WPS_START) {
            _wifi_wps_in_progress = true;
            stateSet(STATE_WPS);
        }

        if (code == MESSAGE_WPS_SUCCESS) {
            _wifiSave();
            _wifi_wps_in_progress = false;
        }

        if (code == MESSAGE_WPS_ERROR) {
            _wifi_wps_in_progress = false;
        }

        if (code == MESSAGE_SMARTCONFIG_START) {
            _wifi_wps_in_progress = true;
            stateSet(STATE_WPS);
        }

        if (code == MESSAGE_SMARTCONFIG_SUCCESS) {
            _wifiSave();
            _wifi_wps_in_progress = false;
        }

        if (code == MESSAGE_SMARTCONFIG_ERROR) {
            _wifi_wps_in_progress = false;
        }

        if (code == MESSAGE_TURNING_OFF) {
            ntpDisconnect();
            stateSet(STATE_IDLE);
        }

    });

    // Enter WPS if button is pressed on boot
    if (buttonState(0)) {
        startWPS();
    }

    _wifi_hours_left = getSetting("timeEvery", TIME_SYNC_EVERY).toInt();
    --_wifi_hours_left;

}

void wifiLoop() {

    jw.loop();

    if (_wifi_sync_mode == 0) return;
    if (jw.networkCount() == 0) return;
    if (_wifi_wps_in_progress) return;
    
    // checks at sharp hours
    RtcDateTime now = rtcGet();
    int currentHour = now.Hour();
    if (currentHour == _wifi_previous_hour) return;
    _wifi_previous_hour = currentHour;

    // Check every X hours
    if (_wifi_sync_mode == 1) {
        if (_wifi_hours_left < 1) {
            wifiOn(false);
            _wifi_hours_left = getSetting("timeEvery", TIME_SYNC_EVERY).toInt();
        }
        --_wifi_hours_left;

    // Check at a certain hour
    } else if (_wifi_sync_mode == 2) {
        if (currentHour == getSetting("timeWhen", TIME_SYNC_WHEN).toInt()) {
            wifiOn(false);
        }
    }

}
