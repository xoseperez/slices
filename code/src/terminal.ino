/*

TERMINAL MODULE

Copyright (C) 2016-2024 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if TERMINAL_SUPPORT

#include "libs/EmbedisWrap.h"
#include "libs/StreamInjector.h"

#include <vector>
#include <Stream.h>

StreamInjector _serial = StreamInjector(TERMINAL_BUFFER_SIZE);
EmbedisWrap embedis(_serial, TERMINAL_BUFFER_SIZE);

// -----------------------------------------------------------------------------
// Commands
// -----------------------------------------------------------------------------

void _terminalHelpCommand() {

    // Get sorted list of commands
    std::vector<String> commands;
    unsigned char size = embedis.getCommandCount();
    for (unsigned int i=0; i<size; i++) {

        String command = embedis.getCommandName(i);
        bool inserted = false;
        for (unsigned char j=0; j<commands.size(); j++) {

            // Check if we have to insert it before the current element
            if (commands[j].compareTo(command) > 0) {
                commands.insert(commands.begin() + j, command);
                inserted = true;
                break;
            }

        }

        // If we could not insert it, just push it at the end
        if (!inserted) commands.push_back(command);

    }

    // Output the list
    DEBUG_MSG_P(PSTR("Available commands:\n"));
    for (unsigned char i=0; i<commands.size(); i++) {
        DEBUG_MSG_P(PSTR("> %s\n"), (commands[i]).c_str());
    }

}

void _terminalKeysCommand() {

    // Get sorted list of keys
    std::vector<String> keys = _settingsKeys();

    // Write key-values
    DEBUG_MSG_P(PSTR("Current settings:\n"));
    for (unsigned int i=0; i<keys.size(); i++) {
        String value = getSetting(keys[i]);
        DEBUG_MSG_P(PSTR("> %s => \"%s\"\n"), (keys[i]).c_str(), value.c_str());
    }

    unsigned long freeEEPROM = SPI_FLASH_SEC_SIZE - settingsSize();
    UNUSED(freeEEPROM);
    DEBUG_MSG_P(PSTR("Number of keys: %d\n"), keys.size());
    DEBUG_MSG_P(PSTR("Free EEPROM: %d bytes (%d%%)\n"), freeEEPROM, 100 * freeEEPROM / SPI_FLASH_SEC_SIZE);

}


void _terminalInitCommand() {

    terminalRegisterCommand(F("COMMANDS"), [](Embedis* e) {
        _terminalHelpCommand();
        terminalOK();
    });

    terminalRegisterCommand(F("FACTORY.RESET"), [](Embedis* e) {
        resetSettings();
        terminalOK();
    });

    terminalRegisterCommand(F("STACK"), [](Embedis* e) {
        infoMemory("Stack", CONT_STACKSIZE, getFreeStack());
        terminalOK();
    });

    terminalRegisterCommand(F("HELP"), [](Embedis* e) {
        _terminalHelpCommand();
        terminalOK();
    });

    terminalRegisterCommand(F("INFO"), [](Embedis* e) {
        info();
        terminalOK();
    });

    terminalRegisterCommand(F("KEYS"), [](Embedis* e) {
        _terminalKeysCommand();
        terminalOK();
    });

    terminalRegisterCommand(F("GET"), [](Embedis* e) {
        if (e->argc < 2) {
            terminalError(F("Wrong arguments"));
            return;
        }

        for (unsigned char i = 1; i < e->argc; i++) {
            String key = String(e->argv[i]);
            String value;
            if (!Embedis::get(key, value)) {
                DEBUG_MSG_P(PSTR("> %s =>\n"), key.c_str());
                continue;
            }

            DEBUG_MSG_P(PSTR("> %s => \"%s\"\n"), key.c_str(), value.c_str());
        }

        terminalOK();
    });

    terminalRegisterCommand(F("RESET"), [](Embedis* e) {
        terminalOK();
        deferredReset(100);
    });

    terminalRegisterCommand(F("UPTIME"), [](Embedis* e) {
        DEBUG_MSG_P(PSTR("Uptime: %d seconds\n"), getUptime());
        terminalOK();
    });

    #if not SETTINGS_AUTOSAVE
        terminalRegisterCommand(F("SAVE"), [](Embedis* e) {
            eepromCommit();
            terminalOK();
        });
    #endif

}

// -----------------------------------------------------------------------------
// Pubic API
// -----------------------------------------------------------------------------

void terminalInject(void *data, size_t len) {
    _serial.inject((char *) data, len);
}

void terminalInject(char ch) {
    _serial.inject(ch);
}

Stream & terminalSerial() {
    return (Stream &) _serial;
}

void terminalRegisterCommand(const String& name, void (*call)(Embedis*)) {
    Embedis::command(name, call);
};

void terminalOK() {
    DEBUG_MSG_P(PSTR("+OK\n"));
}

void terminalError(const String& error) {
    DEBUG_MSG_P(PSTR("-ERROR: %s\n"), error.c_str());
}

void terminalSetup() {

    _serial.callback([](uint8_t ch) {
        DEBUG_PORT.write(ch);
    });

    _terminalInitCommand();

}

void terminalLoop() {
    
    while (DEBUG_PORT.available()) {
        _serial.inject(DEBUG_PORT.read());
    }
    
    embedis.process();
    
}

#endif