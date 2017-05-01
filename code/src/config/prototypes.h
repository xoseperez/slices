#include <Arduino.h>
#include <functional>
#include <NtpClientLib.h>
#include <ESPAsyncWebServer.h>
#include <AsyncMqttClient.h>
#include <functional>
#include <Adafruit_NeoMatrix.h>
#include <RtcDateTime.h>

void mqttRegister(void (*callback)(unsigned int, const char *, const char *));
String mqttSubtopic(char * topic);
template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);
Adafruit_NeoMatrix * getMatrix();
typedef std::function<void(void)> blindCallback;
