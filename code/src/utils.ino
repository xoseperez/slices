/*

UTILS MODULE

Copyright (C) 2017-2019 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "utils.h"
#include <Ticker.h>
#include <limits>

String getIdentifier() {
    char buffer[20];
    snprintf_P(buffer, sizeof(buffer), PSTR("%s-%06X"), DEVICE_NAME, ESP.getChipId());
    return String(buffer);
}

String getBoardName() {
    return getSetting("boardName", DEVICE_NAME);
}

String getHostName() {
    return getSetting("hostname", getIdentifier());
}

String getAdminPass() {
    return getSetting("adminPass", ADMIN_PASS);
}

const String& getCoreVersion() {
    static String version;
    if (!version.length()) {
        #ifdef ARDUINO_ESP8266_RELEASE
            version = ESP.getCoreVersion();
            if (version.equals("00000000")) {
                version = String(ARDUINO_ESP8266_RELEASE);
            }
            version.replace("_", ".");
        #else
            #define _GET_COREVERSION_STR(X) #X
            #define GET_COREVERSION_STR(X) _GET_COREVERSION_STR(X)
            version = GET_COREVERSION_STR(ARDUINO_ESP8266_GIT_DESC);
            #undef _GET_COREVERSION_STR
            #undef GET_COREVERSION_STR
        #endif
    }
    return version;
}

const String& getCoreRevision() {
    static String revision;
    if (!revision.length()) {
        #ifdef ARDUINO_ESP8266_GIT_VER
            revision = String(ARDUINO_ESP8266_GIT_VER, 16);
        #else
            revision = "(unspecified)";
        #endif
    }
    return revision;
}

String buildTime() {
    #if NTP_SUPPORT
        return ntpDateTime(__UNIX_TIMESTAMP__);
    #else
        char buffer[20];
        snprintf_P(
            buffer, sizeof(buffer), PSTR("%04d-%02d-%02d %02d:%02d:%02d"),
            __TIME_YEAR__, __TIME_MONTH__, __TIME_DAY__,
            __TIME_HOUR__, __TIME_MINUTE__, __TIME_SECOND__
        );
        return String(buffer);
    #endif
}

unsigned long getUptime() {

    static unsigned long last_uptime = 0;
    static unsigned char uptime_overflows = 0;

    if (millis() < last_uptime) ++uptime_overflows;
    last_uptime = millis();
    unsigned long uptime_seconds = uptime_overflows * (UPTIME_OVERFLOW / 1000) + (last_uptime / 1000);

    return uptime_seconds;

}

void deferredReset(unsigned long ms) {
    deferred.once_ms(ms, []() {
        ESP.restart();
    });
}

// -----------------------------------------------------------------------------
// INFO
// -----------------------------------------------------------------------------

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

unsigned int info_bytes2sectors(size_t size) {
    return (int) (size + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE;
}

unsigned long info_ota_space() {
    return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
}

unsigned long info_filesystem_space() {
    return ((uint32_t)&_SPIFFS_end - (uint32_t)&_SPIFFS_start);
}

unsigned long info_eeprom_space() {
    return EEPROMr.reserved() * SPI_FLASH_SEC_SIZE;
}

void _info_print_memory_layout_line(const char * name, unsigned long bytes, bool reset) {
    static unsigned long index = 0;
    if (reset) index = 0;
    if (0 == bytes) return;
    unsigned int _sectors = info_bytes2sectors(bytes);
    DEBUG_MSG_P(PSTR("[MAIN] %-20s: %8lu bytes / %4d sectors (%4d to %4d)\n"), name, bytes, _sectors, index, index + _sectors - 1);
    index += _sectors;
}

void _info_print_memory_layout_line(const char * name, unsigned long bytes) {
    _info_print_memory_layout_line(name, bytes, false);
}

void infoMemory(const char * name, unsigned int total_memory, unsigned int free_memory) {

    DEBUG_MSG_P(
        PSTR("[MAIN] %-6s: %5u bytes initially | %5u bytes used (%2u%%) | %5u bytes free (%2u%%)\n"),
        name,
        total_memory,
        total_memory - free_memory,
        100 * (total_memory - free_memory) / total_memory,
        free_memory,
        100 * free_memory / total_memory
    );

}

const char* _info_wifi_sleep_mode(WiFiSleepType_t type) {
    switch (type) {
        case WIFI_NONE_SLEEP: return "NONE";
        case WIFI_LIGHT_SLEEP: return "LIGHT";
        case WIFI_MODEM_SLEEP: return "MODEM";
        default: return "UNKNOWN";
    }
}


void info() {

    DEBUG_MSG_P(PSTR("\n\n---8<-------\n\n"));

    // -------------------------------------------------------------------------

    #if defined(APP_REVISION)
        DEBUG_MSG_P(PSTR("[MAIN] " APP_NAME " " APP_VERSION " (" APP_REVISION ")\n"));
    #else
        DEBUG_MSG_P(PSTR("[MAIN] " APP_NAME " " APP_VERSION "\n"));
    #endif
    DEBUG_MSG_P(PSTR("[MAIN] " APP_AUTHOR "\n"));
    DEBUG_MSG_P(PSTR("[MAIN] " APP_WEBSITE "\n\n"));
    DEBUG_MSG_P(PSTR("[MAIN] CPU chip ID: 0x%06X\n"), ESP.getChipId());
    DEBUG_MSG_P(PSTR("[MAIN] CPU frequency: %u MHz\n"), ESP.getCpuFreqMHz());
    DEBUG_MSG_P(PSTR("[MAIN] SDK version: %s\n"), ESP.getSdkVersion());
    DEBUG_MSG_P(PSTR("[MAIN] Core version: %s\n"), getCoreVersion().c_str());
    DEBUG_MSG_P(PSTR("[MAIN] Core revision: %s\n"), getCoreRevision().c_str());
    DEBUG_MSG_P(PSTR("[MAIN] Build time: %lu\n"), __UNIX_TIMESTAMP__);
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    FlashMode_t mode = ESP.getFlashChipMode();
    UNUSED(mode);
    DEBUG_MSG_P(PSTR("[MAIN] Flash chip ID: 0x%06X\n"), ESP.getFlashChipId());
    DEBUG_MSG_P(PSTR("[MAIN] Flash speed: %u Hz\n"), ESP.getFlashChipSpeed());
    DEBUG_MSG_P(PSTR("[MAIN] Flash mode: %s\n"), mode == FM_QIO ? "QIO" : mode == FM_QOUT ? "QOUT" : mode == FM_DIO ? "DIO" : mode == FM_DOUT ? "DOUT" : "UNKNOWN");
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    _info_print_memory_layout_line("Flash size (CHIP)", ESP.getFlashChipRealSize(), true);
    _info_print_memory_layout_line("Flash size (SDK)", ESP.getFlashChipSize(), true);
    _info_print_memory_layout_line("Reserved", 1 * SPI_FLASH_SEC_SIZE, true);
    _info_print_memory_layout_line("Firmware size", ESP.getSketchSize());
    _info_print_memory_layout_line("Max OTA size", info_ota_space());
    _info_print_memory_layout_line("SPIFFS size", info_filesystem_space());
    _info_print_memory_layout_line("EEPROM size", info_eeprom_space());
    _info_print_memory_layout_line("Reserved", 4 * SPI_FLASH_SEC_SIZE);
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    #if SPIFFS_SUPPORT
        FSInfo fs_info;
        bool fs = SPIFFS.info(fs_info);
        if (fs) {
            DEBUG_MSG_P(PSTR("[MAIN] SPIFFS total size   : %8u bytes / %4d sectors\n"), fs_info.totalBytes, info_bytes2sectors(fs_info.totalBytes));
            DEBUG_MSG_P(PSTR("[MAIN]        used size    : %8u bytes\n"), fs_info.usedBytes);
            DEBUG_MSG_P(PSTR("[MAIN]        block size   : %8u bytes\n"), fs_info.blockSize);
            DEBUG_MSG_P(PSTR("[MAIN]        page size    : %8u bytes\n"), fs_info.pageSize);
            DEBUG_MSG_P(PSTR("[MAIN]        max files    : %8u\n"), fs_info.maxOpenFiles);
            DEBUG_MSG_P(PSTR("[MAIN]        max length   : %8u\n"), fs_info.maxPathLength);
        } else {
            DEBUG_MSG_P(PSTR("[MAIN] No SPIFFS partition\n"));
        }
        DEBUG_MSG_P(PSTR("\n"));
    #endif

    // -------------------------------------------------------------------------

    eepromSectorsDebug();
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    // TODO: crashes, don't know why
    //infoMemory("EEPROM", SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE - settingsSize());
    //infoMemory("Stack", CONT_STACKSIZE, getFreeStack());
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG_P(PSTR("[MAIN] Boot version: %d\n"), ESP.getBootVersion());
    DEBUG_MSG_P(PSTR("[MAIN] Boot mode: %d\n"), ESP.getBootMode());
    DEBUG_MSG_P(PSTR("[MAIN] Last reset reason: %s\n"), (char *) ESP.getResetReason().c_str());
    DEBUG_MSG_P(PSTR("[MAIN] Last reset info: %s\n"), (char *) ESP.getResetInfo().c_str());
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG_P(PSTR("[MAIN] Board: %s\n"), getBoardName().c_str());
    DEBUG_MSG_P(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG_P(PSTR("[MAIN] Firmware MD5: %s\n"), (char *) ESP.getSketchMD5().c_str());

    const WiFiSleepType_t sleep_mode = WiFi.getSleepMode();
    if (sleep_mode != WIFI_NONE_SLEEP) {
        DEBUG_MSG_P(PSTR("[MAIN] WiFi Sleep Mode: %s\n"), _info_wifi_sleep_mode(sleep_mode));
    }

    // -------------------------------------------------------------------------

    DEBUG_MSG_P(PSTR("\n\n---8<-------\n\n"));

}

// -----------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------

char * ltrim(char * s) {
    char *p = s;
    while ((unsigned char) *p == ' ') ++p;
    return p;
}

double roundTo(double num, unsigned char positions) {
    double multiplier = 1;
    while (positions-- > 0) multiplier *= 10;
    return round(num * multiplier) / multiplier;
}

void nice_delay(unsigned long ms) {
    unsigned long start = millis();
    while (millis() - start < ms) delay(1);
}

bool isNumber(const char * s) {
    unsigned char len = strlen(s);
    if (0 == len) return false;
    bool decimal = false;
    bool digit = false;
    for (unsigned char i=0; i<len; i++) {
        if (('-' == s[i]) || ('+' == s[i])) {
            if (i>0) return false;
        } else if (s[i] == '.') {
            if (!digit) return false;
            if (decimal) return false;
            decimal = true;
        } else if (!isdigit(s[i])) {
            return false;
        } else {
            digit = true;
        }
    }
    return digit;
}

// ref: lwip2 lwip_strnstr with strnlen
char* strnstr(const char* buffer, const char* token, size_t n) {
  size_t token_len = strnlen(token, n);
  if (token_len == 0) {
    return const_cast<char*>(buffer);
  }

  for (const char* p = buffer; *p && (p + token_len <= buffer + n); p++) {
    if ((*p == *token) && (strncmp(p, token, token_len) == 0)) {
      return const_cast<char*>(p);
    }
  }

  return nullptr;
}

// TODO: force getSetting return type to handle settings
uint32_t u32fromString(const String& string, int base) {

    const char *ptr = string.c_str();
    char *value_endptr = nullptr;

    // invalidate the whole string when invalid chars are detected
    const auto value = strtoul(ptr, &value_endptr, base);
    if (value_endptr == ptr || value_endptr[0] != '\0') {
        return 0;
    }

    return value;

}

uint32_t u32fromString(const String& string) {
    if (!string.length()) {
        return 0;
    }

    int base = 10;
    if (string.length() > 2) {
        if (string.startsWith("0b")) {
            base = 2;
        } else if (string.startsWith("0o")) {
            base = 8;
        } else if (string.startsWith("0x")) {
            base = 16;
        }
    }

    return u32fromString((base == 10) ? string : string.substring(2), base);
}

String u32toString(uint32_t value, int base) {
    String result;
    result.reserve(32 + 2);

    if (base == 2) {
        result += "0b";
    } else if (base == 8) {
        result += "0o";
    } else if (base == 16) {
        result += "0x";
    }

    char buffer[33] = {0};
    ultoa(value, buffer, base);
    result += buffer;

    return result;
}
