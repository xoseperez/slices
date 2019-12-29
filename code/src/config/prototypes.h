#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <functional>
#include <NtpClientLib.h>
#include <ESPAsyncWebServer.h>
#include <functional>
#include <RtcDateTime.h>

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

#include <JustWifi.h>
using wifi_callback_f = std::function<void(justwifi_messages_t code, char * parameter)>;
void wifiRegister(wifi_callback_f callback);
bool wifiConnected();

#if LWIP_VERSION_MAJOR == 1
#include <netif/etharp.h>
#else // LWIP_VERSION_MAJOR >= 2
#include <lwip/etharp.h>
#endif

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#include <AsyncMqttClient.h>
void mqttRegister(void (*callback)(unsigned int, const char *, const char *));
String mqttSubtopic(char * topic);

// -----------------------------------------------------------------------------
// SETTINGS
// -----------------------------------------------------------------------------

template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);

// -----------------------------------------------------------------------------
// MATRIX
// -----------------------------------------------------------------------------

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <FastLED_Matrix.h>
FastLED_Matrix * getMatrix();
typedef std::function<void(void)> blindCallback;
