
//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200
#define DEVICE                  "SLICES"
#define MANUFACTURER            "TINKERMAN"
#define HOSTNAME                DEVICE
#define BUFFER_SIZE             1024
#define HEARTBEAT_INTERVAL      300000
#define UPTIME_OVERFLOW         4294967295
#define ONE_SECOND              1000
#define SETTINGS_AUTOSAVE       1
#define USE_PASSWORD            0
#define SETTINGS_MAX_LIST_COUNT 10

#define WIFI_OFF_AFTER          120         // in seconds
#define TIME_SYNC_MODE          1           // 0: manual, 1: every, 2: when
#define TIME_SYNC_EVERY         6           // in hours (can be > 24)
#define TIME_SYNC_WHEN          20          // at 20:00

//--------------------------------------------------------------------------------
// DEBUG
//--------------------------------------------------------------------------------

#ifndef DEBUG_PORT
#define DEBUG_PORT              Serial
#endif

// Uncomment and configure these lines to enable remote debug via udpDebug
// To receive the message son the destination computer use nc:
// nc -ul 8111

#define DEBUG_UDP_IP            IPAddress(192, 168, 1, 100)
#define DEBUG_UDP_PORT          8113

//------------------------------------------------------------------------------
// EEPROM
//------------------------------------------------------------------------------

#define EEPROM_SIZE             SPI_FLASH_SEC_SIZE  // EEPROM size in bytes (1 sector = 4096 bytes)

//#define EEPROM_RORATE_SECTORS   2             // Number of sectors to use for EEPROM rotation
                                                // If not defined the firmware will use a number based
                                                // on the number of available sectors

#define EEPROM_DRIVER           0
#define EEPROM_ROTATE_DATA      1               // Reserved for the EEPROM_ROTATE library (3 bytes)
#define EEPROM_DATA_END         4               // End of custom EEPROM data block

//--------------------------------------------------------------------------------
// I2C
//--------------------------------------------------------------------------------

#define SDA_PIN                 2
#define SCL_PIN                 0

//------------------------------------------------------------------------------
// DRIVERS
//------------------------------------------------------------------------------

#define ENABLE_DRIVER_FIBONACCI         1
#define ENABLE_DRIVER_BASIC             1
#define ENABLE_DRIVER_BINARY            1
#define ENABLE_DRIVER_WORD              1
#define ENABLE_DRIVER_GAME_OF_LIFE      1
#define ENABLE_DRIVER_CANVAS            1
#define ENABLE_DRIVER_CIRCLE            1

#if FIBONACCI

    #undef ENABLE_DRIVER_BASIC
    #define ENABLE_DRIVER_BASIC         0

    #undef ENABLE_DRIVER_BINARY
    #define ENABLE_DRIVER_BINARY        0

    #undef ENABLE_DRIVER_WORD
    #define ENABLE_DRIVER_WORD          0

    #undef ENABLE_DRIVER_GAME_OF_LIFE
    #define ENABLE_DRIVER_GAME_OF_LIFE  0

    #undef ENABLE_DRIVER_CANVAS
    #define ENABLE_DRIVER_CANVAS        0

    #define DEVICE_NAME                 "FIBONACCI"

#endif

#if CIRCLE_SIZE > 0

    #undef ENABLE_DRIVER_GAME_OF_LIFE
    #define ENABLE_DRIVER_GAME_OF_LIFE  0

#endif

#if CIRCLE_SIZE == 0

    #undef ENABLE_DRIVER_CIRCLE
    #define ENABLE_DRIVER_CIRCLE        0

#endif

//--------------------------------------------------------------------------------
// MATRIX
//--------------------------------------------------------------------------------

#define MATRIX_PIN                  4

#define MATRIX_8x8                  1
#define MATRIX_16x16                2
#define MATRIX_16x8                 3

#ifndef MATRIX_SIZE
#define MATRIX_SIZE                 MATRIX_8x8
#endif

#ifndef CIRCLE_SIZE
#define CIRCLE_SIZE                 0
#endif

#if MATRIX_SIZE == MATRIX_8x8
#define MATRIX_WIDTH                8
#define MATRIX_HEIGHT               8
#ifndef DEVICE_NAME
    #define DEVICE_NAME             "SLICES-SMALL"
#endif
#elif MATRIX_SIZE == MATRIX_16x16
#define MATRIX_WIDTH                16
#define MATRIX_HEIGHT               16
#ifndef DEVICE_NAME
    #define DEVICE_NAME             "SLICES-BIG"
#endif
#elif MATRIX_SIZE == MATRIX_16x8
#define MATRIX_WIDTH                16
#define MATRIX_HEIGHT               8
#endif

#ifndef DEVICE_NAME
    #define DEVICE_NAME             "SLICES-CLOCK"
#endif

#define MATRIX_CHIPSET              WS2812B
#define MATRIX_COLOR_ORDER          GRB
#define CIRCLE_START                (MATRIX_HEIGHT * MATRIX_WIDTH)
#define MATRIX_LEDS                 (MATRIX_WIDTH * MATRIX_HEIGHT + CIRCLE_SIZE)

#define MATRIX_DEFAULT_BRIGHTNESS   60
#define MATRIX_SCROLL_INTERVAL      75

//--------------------------------------------------------------------------------
// BUTTON
//--------------------------------------------------------------------------------

#define BUTTON1_PIN                 16
#define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH 

#define BUTTON_LNGCLICK_LENGTH      1000
#define BUTTON_LNGLNGCLICK_LENGTH   10000

#define BUTTON_EVENT_NONE           0
#define BUTTON_EVENT_PRESSED        1
#define BUTTON_EVENT_CLICK          2
#define BUTTON_EVENT_DBLCLICK       3
#define BUTTON_EVENT_LNGCLICK       4
#define BUTTON_EVENT_LNGLNGCLICK    5

// -----------------------------------------------------------------------------
// SOUND
// -----------------------------------------------------------------------------

#define BUZZER_PIN                  5

// -----------------------------------------------------------------------------
// WIFI & WEB
// -----------------------------------------------------------------------------

#define WIFI_RECONNECT_INTERVAL 120000
#define WIFI_MAX_NETWORKS       5
#define ADMIN_PASS              "fibonacci"
#define FORCE_CHANGE_PASS       0
#define HTTP_USERNAME           "admin"

#define ENABLE_WEB              1
#define WS_BUFFER_SIZE          5
#define WS_TIMEOUT              1800000
#define WEBSERVER_PORT          80
#define DNS_PORT                53
#define ENABLE_MDNS             1

#define WEB_MODE_NORMAL         0
#define WEB_MODE_PASSWORD       1

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

#define OTA_PORT                8266

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#define ENABLE_MQTT             0

#ifndef MQTT_USE_ASYNC
#define MQTT_USE_ASYNC          1
#endif

#define MQTT_SERVER             ""
#define MQTT_PORT               1883
#define MQTT_TOPIC              "/test/switch/{identifier}"
#define MQTT_RETAIN             true
#define MQTT_QOS                0
#define MQTT_KEEPALIVE          30
#define MQTT_RECONNECT_DELAY    10000
#define MQTT_TRY_INTERVAL       30000
#define MQTT_MAX_TRIES          12
#define MQTT_SKIP_RETAINED      1
#define MQTT_SKIP_TIME          1000

#define MQTT_TOPIC_ACTION       "action"
#define MQTT_TOPIC_MESSAGE      "message"

#define MQTT_TOPIC_IP           "ip"
#define MQTT_TOPIC_VERSION      "version"
#define MQTT_TOPIC_UPTIME       "uptime"
#define MQTT_TOPIC_FREEHEAP     "freeheap"
#define MQTT_TOPIC_STATUS       "status"
#define MQTT_TOPIC_MAC          "mac"
#define MQTT_TOPIC_RSSI         "rssi"
#define MQTT_TOPIC_APP          "app"
#define MQTT_TOPIC_INTERVAL     "interval"
#define MQTT_TOPIC_HOSTNAME     "hostname"

// Periodic reports
#define MQTT_REPORT_IP          1
#define MQTT_REPORT_VERSION     1
#define MQTT_REPORT_UPTIME      1
#define MQTT_REPORT_FREEHEAP    1
#define MQTT_REPORT_STATUS      1
#define MQTT_REPORT_MAC         1
#define MQTT_REPORT_RSSI        1
#define MQTT_REPORT_APP         1
#define MQTT_REPORT_INTERVAL    0
#define MQTT_REPORT_HOSTNAME    1

#define MQTT_STATUS_ONLINE      "1"
#define MQTT_STATUS_OFFLINE     "0"

#define MQTT_ACTION_RESET       "reset"

#define MQTT_CONNECT_EVENT      0
#define MQTT_DISCONNECT_EVENT   1
#define MQTT_MESSAGE_EVENT      2

// Custom get and set postfixes
// Use something like "/status" or "/set", with leading slash
#define MQTT_USE_GETTER         ""
#define MQTT_USE_SETTER         ""

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#define NTP_SERVER              "pool.ntp.org"
#define NTP_TIME_OFFSET         60              // in minutes
#define NTP_DAY_LIGHT           1               // 0 or 1            

//------------------------------------------------------------------------------
// TERMINAL
//------------------------------------------------------------------------------

#ifndef TERMINAL_SUPPORT
#define TERMINAL_SUPPORT         1              // Enable terminal commands (0.97Kb)
#endif

#define TERMINAL_BUFFER_SIZE     128            // Max size for commands commands
