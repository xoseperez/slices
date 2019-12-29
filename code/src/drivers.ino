/*

DRIVERS MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <EEPROM.h>
#include <vector>
typedef struct {
    char * name;
    blindCallback startFn = NULL;
    blindCallback loopFn = NULL;
    blindCallback stopFn = NULL;
} driver_t;
std::vector<driver_t> _drivers;
unsigned char _currentDriver = 0;

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void driverSetup() {
    unsigned char driver = EEPROM.read(EEPROM_DRIVER);
    driverSet(driver);
}

unsigned char driverRegister(const char * name, blindCallback startFn, blindCallback loopFn, blindCallback stopFn) {
    driver_t driver;
    driver.name = strdup(name);
    driver.startFn = startFn;
    driver.loopFn = loopFn;
    driver.stopFn = stopFn;
    _drivers.push_back(driver);
    DEBUG_MSG_P(PSTR("[DRIVER] Registering: %s\n"), name);
    return _drivers.size() - 1;
}

void driverStart() {
    if (_drivers[_currentDriver].startFn) (_drivers[_currentDriver].startFn)();
}

void driverLoop() {

    if (_drivers[_currentDriver].loopFn) (_drivers[_currentDriver].loopFn)();

    // Clock code
    #if ENABLE_DRIVER_CIRCLE
        //circleClockLoop();
        //matrixRefresh();
    #endif

}

void driverStop() {
    matrixStopScroll();
    matrixClear();
    matrixRefresh();
    if (_drivers[_currentDriver].stopFn) (_drivers[_currentDriver].stopFn)();
}

int driverFind(const char * name) {
    for (unsigned char i=0; i<_drivers.size(); i++) {
        if (strcmp(_drivers[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void driverSet(unsigned char i) {
    if (i <= _drivers.size() - 1) {
        driverStop();
        _currentDriver = i;
        driverStart();
        DEBUG_MSG_P(PSTR("[DRIVER] Set to '%s'\n"), _drivers[_currentDriver].name);
        EEPROM.write(EEPROM_DRIVER, _currentDriver);
        EEPROM.commit();
    }
}

void driverNext() {
    unsigned char nextDriver = (_currentDriver + 1) % _drivers.size();
    driverSet(nextDriver);
}
