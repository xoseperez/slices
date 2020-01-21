#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <functional>
#include <NtpClientLib.h>
#include <ESPAsyncWebServer.h>
#include <functional>
#include <RtcDateTime.h>
#include <ArduinoJson.h>

#define UNUSED(x) (void)(x)

// Core version 2.4.2 and higher changed the cont_t structure to a pointer:
// https://github.com/esp8266/Arduino/commit/5d5ea92a4d004ab009d5f642629946a0cb8893dd#diff-3fa12668b289ccb95b7ab334833a4ba8L35
// Core version 2.5.0 introduced EspClass helper method:
// https://github.com/esp8266/Arduino/commit/0e0e34c614fe8a47544c9998201b1d9b3c24eb18
extern "C" {
#include <cont.h>
extern cont_t* g_pcont;
#define getFreeStack() cont_get_free_stack(g_pcont)
}

#include <Ticker.h>
Ticker deferred;

// -----------------------------------------------------------------------------
// EEPROM_ROTATE
// -----------------------------------------------------------------------------

#include <EEPROM_Rotate.h>
EEPROM_Rotate EEPROMr;

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

#include <JustWifi.h>
using wifi_callback_f = std::function<void(justwifi_messages_t code, char * parameter)>;
void wifiRegister(wifi_callback_f callback);
bool wifiConnected();

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#include <AsyncMqttClient.h>
void mqttRegister(void (*callback)(unsigned int, const char *, const char *));
String mqttSubtopic(char * topic);

// -----------------------------------------------------------------------------
// SETTINGS
// -----------------------------------------------------------------------------

#include <Embedis.h>

template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);

// -----------------------------------------------------------------------------
// Terminal
// -----------------------------------------------------------------------------

void terminalRegisterCommand(const String& name, void (*call)(Embedis*));
void terminalInject(void *data, size_t len);
Stream & terminalSerial();

// -----------------------------------------------------------------------------
// MATRIX
// -----------------------------------------------------------------------------

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <FastLED_Matrix.h>
FastLED_Matrix * getMatrix();
typedef std::function<void(void)> blindCallback;
typedef std::function<void(unsigned char)> paramCallback;

// -----------------------------------------------------------------------------
// DRIVERS
// -----------------------------------------------------------------------------

struct clockword {
    byte row;
    unsigned int positions;
};

struct ray_struct {
    byte x;
    byte y;
    byte speed;
    byte length;
    byte life;
};