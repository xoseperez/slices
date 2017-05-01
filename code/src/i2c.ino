/*

I2C MODULE

Copyright (C) 2016 by Xose Pérez <xose at tarpunacoop dot org>

*/

// -----------------------------------------------------------------------------
// I2C UTILS
// -----------------------------------------------------------------------------

void i2c_scanner() {

    byte error, address;
    int nDevices;

    nDevices = 0;
    for(address = 1; address < 127; address++ ) {

        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.printf("[I2C] Device found at address 0x%2X\n", address);
            nDevices++;
        } else if (error==4) {
            Serial.printf("[I2C] Unknown error at address 0x%2X\n", address);
        }
    }

    if (nDevices == 0) Serial.println("[I2C] No devices found");

}
