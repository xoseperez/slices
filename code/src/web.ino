/*

WEBSERVER MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_WEB

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <vector>

#include "static/index.html.gz.h"

AsyncWebServer * _server;
AsyncWebSocket ws("/ws");
char _last_modified[50];

typedef struct {
    IPAddress ip;
    unsigned long timestamp = 0;
} ws_ticket_t;
ws_ticket_t _ticket[WS_BUFFER_SIZE];

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

void wsSend(const char * payload) {
    if (ws.count() > 0) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Broadcasting '%s'\n"), payload);
        ws.textAll(payload);
    }
}

void wsSend(uint32_t client_id, const char * payload) {
    DEBUG_MSG_P(PSTR("[WEBSOCKET] Sending '%s' to #%ld\n"), payload, client_id);
    ws.text(client_id, payload);
}

#if ENABLE_MQTT
void wsMQTTCallback(unsigned int type, const char * topic, const char * payload) {

    if (type == MQTT_CONNECT_EVENT) {
        wsSend("{\"mqttStatus\": true}");
    }

    if (type == MQTT_DISCONNECT_EVENT) {
        wsSend("{\"mqttStatus\": false}");
    }

}
#endif

void _wsParse(uint32_t client_id, uint8_t * payload, size_t length) {

    // Parse JSON input
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char *) payload);
    if (!root.success()) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Error parsing data\n"));
        ws.text(client_id, "{\"message\": \"Error parsing data!\"}");
        return;
    }

    // Check actions
    if (root.containsKey("action")) {

        String action = root["action"];

        DEBUG_MSG_P(PSTR("[WEBSOCKET] Requested action: %s\n"), action.c_str());

        if (action.equals("reset")) {
            ESP.restart();
        }

        if (action.equals("restore") && root.containsKey("data")) {

            JsonObject& data = root["data"];
            if (!data.containsKey("app") || (data["app"] != APP_NAME)) {
                ws.text(client_id, "{\"message\": \"The file does not look like a valid configuration backup.\"}");
                return;
            }

            resetSettings();

            for (auto element : data) {
                if (strcmp(element.key, "app") == 0) continue;
                if (strcmp(element.key, "version") == 0) continue;
                setSetting(element.key, element.value.as<char*>());
            }

            saveSettings();

            ws.text(client_id, "{\"message\": \"Changes saved. You should reboot your board now.\"}");

        }

        if (action.equals("reconnect")) {

            // Let the HTTP request return and disconnect after 100ms
            deferred.once_ms(100, wifiDisconnect);

        }

        if (action.equals("driver") && root.containsKey("data")) {
            JsonObject& data = root["data"];
            if (data.containsKey("name")) {
                int id = driverFind(data["name"]);
                if (id>=0) driverSet(id);
            }
        }

        #if ENABLE_DRIVER_CANVAS
        if (action.equals("color") && root.containsKey("data")) {
            JsonObject& data = root["data"];
            unsigned char x = data["x"].as<int>();
            unsigned char y = data["y"].as<int>();
            canvasDraw(x, y, data["color"]);
        }
        #endif

    };

    // Check config
    if (root.containsKey("config") && root["config"].is<JsonArray&>()) {

        JsonArray& config = root["config"];
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Parsing configuration data\n"));

        unsigned char webMode = WEB_MODE_NORMAL;

        bool save = false;
        bool changed = false;
        #if ENABLE_MQTT
        bool changedMQTT = false;
        #endif
        unsigned int network = 0;
        String adminPass;

        for (unsigned int i=0; i<config.size(); i++) {

            String key = config[i]["name"];
            String value = config[i]["value"];

            // Skip firmware filename
            if (key.equals("filename")) continue;

            // Web portions
            if (key == "webPort") {
                if ((value.toInt() == 0) || (value.toInt() == 80)) {
                    save = changed = true;
                    delSetting(key);
                    continue;
                }
            }

            if (key == "webMode") {
                webMode = value.toInt();
                continue;
            }

            // Check password
            if (key == "adminPass1") {
                adminPass = value;
                continue;
            }
            if (key == "adminPass2") {
                if (!value.equals(adminPass)) {
                    ws.text(client_id, "{\"message\": \"Passwords do not match!\"}");
                    return;
                }
                if (value.length() == 0) continue;
                ws.text(client_id, "{\"action\": \"reload\"}");
                key = String("adminPass");
            }

            if (key == "ssid") {
                key = key + String(network);
            }
            if (key == "pass") {
                key = key + String(network);
            }
            if (key == "ip") {
                key = key + String(network);
            }
            if (key == "gw") {
                key = key + String(network);
            }
            if (key == "mask") {
                key = key + String(network);
            }
            if (key == "dns") {
                key = key + String(network);
                ++network;
            }

            if (value != getSetting(key)) {
                //DEBUG_MSG_P(PSTR("[WEBSOCKET] Storing %s = %s\n", key.c_str(), value.c_str()));
                setSetting(key, value);
                save = changed = true;
                #if ENABLE_MQTT
                if (key.startsWith("mqtt")) changedMQTT = true;
                #endif
            }

        }

        if (webMode == WEB_MODE_NORMAL) {

            // Clean wifi networks
            unsigned char i = 0;
            while (i < network) {
                if (getSetting("ssid" + String(i)).length() == 0) {
                    delSetting("ssid" + String(i));
                    break;
                }
                if (getSetting("pass" + String(i)).length() == 0) delSetting("pass" + String(i));
                if (getSetting("ip" + String(i)).length() == 0) delSetting("ip" + String(i));
                if (getSetting("gw" + String(i)).length() == 0) delSetting("gw" + String(i));
                if (getSetting("mask" + String(i)).length() == 0) delSetting("mask" + String(i));
                if (getSetting("dns" + String(i)).length() == 0) delSetting("dns" + String(i));
                ++i;
            }
            while (i < WIFI_MAX_NETWORKS) {
                if (getSetting("ssid" + String(i)).length() > 0) {
                    save = changed = true;
                }
                delSetting("ssid" + String(i));
                delSetting("pass" + String(i));
                delSetting("ip" + String(i));
                delSetting("gw" + String(i));
                delSetting("mask" + String(i));
                delSetting("dns" + String(i));
                ++i;
            }

        }

        // Save settings
        if (save) {

            saveSettings();
            wifiConfigure();
            otaConfigure();
            #if ENABLE_MQTT
            buildTopics();
            #endif

            // Check if we should reconfigure MQTT connection
            #if ENABLE_MQTT
            if (changedMQTT) {
                mqttDisconnect();
            }
            #endif
        }

        if (changed) {
            ws.text(client_id, "{\"message\": \"Changes saved\"}");
        } else {
            ws.text(client_id, "{\"message\": \"No changes detected\"}");
        }

    }

}

void _wsStart(uint32_t client_id) {

    char chipid[6];
    sprintf(chipid, "%06X", ESP.getChipId());

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    bool changePassword = false;
    #if FORCE_CHANGE_PASS == 1
        String adminPass = getSetting("adminPass", ADMIN_PASS);
        if (adminPass.equals(ADMIN_PASS)) changePassword = true;
    #endif

    if (changePassword) {

        root["webMode"] = WEB_MODE_PASSWORD;

    } else {

        root["webMode"] = WEB_MODE_NORMAL;

        root["app"] = APP_NAME;
        root["version"] = APP_VERSION;
        root["buildDate"] = __DATE__;
        root["buildTime"] = __TIME__;

        root["manufacturer"] = String(MANUFACTURER);
        root["chipid"] = chipid;
        root["mac"] = WiFi.macAddress();
        root["device"] = String(DEVICE);
        root["hostname"] = getSetting("hostname", HOSTNAME);
        root["network"] = getNetwork();
        root["deviceip"] = getIP();

        #if ENABLE_MQTT
        root["mqttVisible"] = true;
        root["mqttStatus"] = mqttConnected();
        root["mqttServer"] = getSetting("mqttServer", MQTT_SERVER);
        root["mqttPort"] = getSetting("mqttPort", MQTT_PORT);
        root["mqttUser"] = getSetting("mqttUser");
        root["mqttPassword"] = getSetting("mqttPassword");
        root["mqttTopic"] = getSetting("mqttTopic", MQTT_TOPIC);
        #endif

        #if ENABLE_DRIVER_CANVAS
        root["canvasVisible"] = true;
        #endif

        root["matrixWidth"] = MATRIX_WIDTH;
        root["matrixHeight"] = MATRIX_HEIGHT;

        root["webPort"] = getSetting("webPort", WEBSERVER_PORT).toInt();

        root["maxNetworks"] = WIFI_MAX_NETWORKS;
        JsonArray& wifi = root.createNestedArray("wifi");
        for (byte i=0; i<WIFI_MAX_NETWORKS; i++) {
            if (getSetting("ssid" + String(i)).length() == 0) break;
            JsonObject& network = wifi.createNestedObject();
            network["ssid"] = getSetting("ssid" + String(i));
            network["pass"] = getSetting("pass" + String(i));
            network["ip"] = getSetting("ip" + String(i));
            network["gw"] = getSetting("gw" + String(i));
            network["mask"] = getSetting("mask" + String(i));
            network["dns"] = getSetting("dns" + String(i));
        }

    }

    String output;
    root.printTo(output);
    ws.text(client_id, (char *) output.c_str());

}

bool _wsAuth(AsyncWebSocketClient * client) {

    IPAddress ip = client->remoteIP();
    unsigned long now = millis();
    unsigned short index = 0;

    for (index = 0; index < WS_BUFFER_SIZE; index++) {
        if ((_ticket[index].ip == ip) && (now - _ticket[index].timestamp < WS_TIMEOUT)) break;
    }

    if (index == WS_BUFFER_SIZE) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Validation check failed\n"));
        ws.text(client->id(), "{\"message\": \"Session expired, please reload page...\"}");
        return false;
    }

    return true;

}

void _wsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

    static uint8_t * message;

    // Authorize
    #ifndef NOWSAUTH
        if (!_wsAuth(client)) return;
    #endif

    if (type == WS_EVT_CONNECT) {
        IPAddress ip = client->remoteIP();
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u connected, ip: %d.%d.%d.%d, url: %s\n"), client->id(), ip[0], ip[1], ip[2], ip[3], server->url());
        _wsStart(client->id());
    } else if(type == WS_EVT_DISCONNECT) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u disconnected\n"), client->id());
    } else if(type == WS_EVT_ERROR) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u error(%u): %s\n"), client->id(), *((uint16_t*)arg), (char*)data);
    } else if(type == WS_EVT_PONG) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u pong(%u): %s\n"), client->id(), len, len ? (char*) data : "");
    } else if(type == WS_EVT_DATA) {

        AwsFrameInfo * info = (AwsFrameInfo*)arg;

        // First packet
        if (info->index == 0) {
            message = (uint8_t*) malloc(info->len);
        }

        // Store data
        memcpy(message + info->index, data, len);

        // Last packet
        if (info->index + len == info->len) {
            _wsParse(client->id(), message, info->len);
            free(message);
        }

    }

}

// -----------------------------------------------------------------------------
// WEBSERVER
// -----------------------------------------------------------------------------

void webLogRequest(AsyncWebServerRequest *request) {
    DEBUG_MSG_P(PSTR("[WEBSERVER] Request: %s %s\n"), request->methodToString(), request->url().c_str());
}

bool _authenticate(AsyncWebServerRequest *request) {
    String password = getSetting("adminPass", ADMIN_PASS);
    char httpPassword[password.length() + 1];
    password.toCharArray(httpPassword, password.length() + 1);
    return request->authenticate(HTTP_USERNAME, httpPassword);
}

void _onAuth(AsyncWebServerRequest *request) {

    webLogRequest(request);
    if (!_authenticate(request)) return request->requestAuthentication();

    IPAddress ip = request->client()->remoteIP();
    unsigned long now = millis();
    unsigned short index;
    for (index = 0; index < WS_BUFFER_SIZE; index++) {
        if (_ticket[index].ip == ip) break;
        if (_ticket[index].timestamp == 0) break;
        if (now - _ticket[index].timestamp > WS_TIMEOUT) break;
    }
    if (index == WS_BUFFER_SIZE) {
        request->send(429);
    } else {
        _ticket[index].ip = ip;
        _ticket[index].timestamp = now;
        request->send(204);
    }

}

void _onGetConfig(AsyncWebServerRequest *request) {

    webLogRequest(request);
    if (!_authenticate(request)) return request->requestAuthentication();

    AsyncJsonResponse * response = new AsyncJsonResponse();
    JsonObject& root = response->getRoot();

    root["app"] = APP_NAME;
    root["version"] = APP_VERSION;

    unsigned int size = settingsKeyCount();
    for (unsigned int i=0; i<size; i++) {
        String key = settingsKeyName(i);
        String value = getSetting(key);
        root[key] = value;
    }

    char buffer[100];
    sprintf(buffer, "attachment; filename=\"%s-backup.json\"", (char *) getSetting("hostname").c_str());
    response->addHeader("Content-Disposition", buffer);
    response->setLength();
    request->send(response);

}

void _onHome(AsyncWebServerRequest *request) {

    webLogRequest(request);

    if (request->header("If-Modified-Since").equals(_last_modified)) {

        request->send(304);

    } else {

        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Last-Modified", _last_modified);
        request->send(response);

    }

}

void _onUpgrade(AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", Update.hasError() ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    if (!Update.hasError()) {
        deferredReset(100);
    }
    request->send(response);
}

void _onUpgradeData(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        DEBUG_MSG_P(PSTR("[UPGRADE] Start: %s\n"), filename.c_str());
        Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
        }
    }
    if (!Update.hasError()) {
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }
    }
    if (final) {
        if (Update.end(true)){
            DEBUG_MSG_P(PSTR("[UPGRADE] Success:  %u bytes\n"), index + len);
        } else {
            Update.printError(Serial);
        }
    } else {
        DEBUG_MSG_P(PSTR("[UPGRADE] Progress: %u bytes\r"), index + len);
    }
}

void webSetup() {

    // Create server
    _server = new AsyncWebServer(getSetting("webPort", WEBSERVER_PORT).toInt());

    // Setup websocket
    ws.onEvent(_wsEvent);
    #if ENABLE_MQTT
    mqttRegister(wsMQTTCallback);
    #endif

    // Cache the Last-Modifier header value
    sprintf(_last_modified, "%s %s GMT", __DATE__, __TIME__);

    // Setup webserver
    _server->addHandler(&ws);

    // Rewrites
    _server->rewrite("/", "/index.html");

    // Serve home (basic authentication protection)
    _server->on("/index.html", HTTP_GET, _onHome);
    _server->on("/config", HTTP_GET, _onGetConfig);
    _server->on("/auth", HTTP_GET, _onAuth);
    _server->on("/upgrade", HTTP_POST, _onUpgrade, _onUpgradeData);

    // 404
    _server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404);
    });

    // Run server
    _server->begin();
    DEBUG_MSG_P(PSTR("[WEBSERVER] Webserver running on port %d\n"), getSetting("webPort", WEBSERVER_PORT).toInt());

}

#endif // ENABLE_WEB