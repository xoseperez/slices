/*

DRIVERS MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <vector>
typedef struct {
    char * name;
    blindCallback startFn = NULL;
    blindCallback loopFn = NULL;
    blindCallback stopFn = NULL;
    paramCallback statusFn = NULL;
    paramCallback progressFn = NULL;
} driver_t;
std::vector<driver_t> _drivers;
unsigned char _currentDriver = 0;

// -----------------------------------------------------------------------------
// DRIVER
// -----------------------------------------------------------------------------

void driverSetup() {
    unsigned char driver = EEPROMr.read(EEPROM_DRIVER);
    if (_drivers.size() <= driver) driver = 0;
    driverSet(driver);
}

unsigned char driverRegister(const char * name, blindCallback startFn, blindCallback loopFn, blindCallback stopFn, paramCallback statusFn, paramCallback progressFn) {
    driver_t driver;
    driver.name = strdup(name);
    driver.startFn = startFn;
    driver.loopFn = loopFn;
    driver.stopFn = stopFn;
    driver.statusFn = statusFn;
    driver.progressFn = progressFn;
    _drivers.push_back(driver);
    DEBUG_MSG_P(PSTR("[DRIVER] Registering: %s\n"), name);
    return _drivers.size() - 1;
}

void driverStart() {
    if (_drivers.size() <= _currentDriver) return;
    if (_drivers[_currentDriver].startFn) (_drivers[_currentDriver].startFn)();
}

void driverLoop() {

    if (_drivers.size() <= _currentDriver) return;
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
    if (_drivers.size() <= _currentDriver) return;
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
    if (_drivers.size() <= i) return;
    driverStop();
    _currentDriver = i; 
    driverStart();
    DEBUG_MSG_P(PSTR("[DRIVER] Set to '%s'\n"), _drivers[_currentDriver].name);
    EEPROMr.write(EEPROM_DRIVER, _currentDriver);
    EEPROMr.commit();
}

unsigned char driverCount() {
    return _drivers.size();
} 

void driverNext() {
    if (_drivers.size() == 0) return;
    unsigned char nextDriver = (_currentDriver + 1) % _drivers.size();
    driverSet(nextDriver);
}

void driverCommonStatus(unsigned char value) {
    CRGB color = stateColor(value);
    matrixSetPixelColor(0, MATRIX_HEIGHT - 1, color);
}

void driverCommonProgress(unsigned char value) {
    long num = MATRIX_WIDTH * value / 100;
    for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
        if (num >= x) matrixSetPixelColor(x, MATRIX_HEIGHT-1, CRGB::Green);
    }
}

void driverStatus(unsigned char value) {
    if (_drivers.size() <= _currentDriver) return;
    if (_drivers[_currentDriver].statusFn) (_drivers[_currentDriver].statusFn)(value);
}

void driverProgress(unsigned char value) {
    if (_drivers.size() <= _currentDriver) return;
    if (_drivers[_currentDriver].progressFn) (_drivers[_currentDriver].progressFn)(value);
}
