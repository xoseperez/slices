/*

UTILS MODULE

Copyright (C) 2017-2019 by Xose Pérez <xose dot perez at gmail dot com>

*/

String getAdminPass() {
    return getSetting("adminPass", ADMIN_PASS);
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

