//------------------------------------------------------------------------------
// CONFIGURATION
//------------------------------------------------------------------------------

#if FIBONACCI

    #undef ENABLE_DRIVER_BASIC
    #define ENABLE_DRIVER_BASIC         0

    #undef ENABLE_DRIVER_BINARY
    #define ENABLE_DRIVER_BINARY        0

    #undef ENABLE_DRIVER_WORD
    #define ENABLE_DRIVER_WORD          0

#endif

#ifndef ENABLE_DRIVER_FIBONACCI
#define ENABLE_DRIVER_FIBONACCI         1
#endif

#ifndef ENABLE_DRIVER_BASIC
#define ENABLE_DRIVER_BASIC             1
#endif

#ifndef ENABLE_DRIVER_BINARY
#define ENABLE_DRIVER_BINARY            1
#endif

#ifndef ENABLE_DRIVER_WORD
#define ENABLE_DRIVER_WORD              1
#endif

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

//--------------------------------------------------------------------------------
// DEBUG
//--------------------------------------------------------------------------------

#ifndef DEBUG_PORT
#define DEBUG_PORT              Serial
#endif

// Uncomment and configure these lines to enable remote debug via udpDebug
// To receive the message son the destination computer use nc:
// nc -ul 8111

//#define DEBUG_UDP_IP            IPAddress(192, 168, 1, 100)
//#define DEBUG_UDP_PORT          8113

//--------------------------------------------------------------------------------
// EEPROM
//--------------------------------------------------------------------------------

#define EEPROM_DRIVER           0
#define EEPROM_DATA_END         1

//--------------------------------------------------------------------------------
// I2C
//--------------------------------------------------------------------------------

#define SDA_PIN                 2
#define SCL_PIN                 0

//--------------------------------------------------------------------------------
// MATRIX
//--------------------------------------------------------------------------------

#define MATRIX_PIN                  4

#define MATRIX_8x8                  1
#define MATRIX_16x16                2
#ifndef MATRIX_SIZE
#define MATRIX_SIZE                 MATRIX_8x8
#endif

#if MATRIX_SIZE == MATRIX_8x8
#define MATRIX_WIDTH                8
#define MATRIX_HEIGHT               8
#define MATRIX_MODE                 NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE
#define MATRIX_TYPE                 NEO_GRB + NEO_KHZ800
#else
#define MATRIX_WIDTH                16
#define MATRIX_HEIGHT               16
#define MATRIX_MODE                 NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG
#define MATRIX_TYPE                 NEO_GRB + NEO_KHZ800
#endif

#define MATRIX_DEFAULT_BRIGHTNESS   40
#define MATRIX_SCROLL_INTERVAL      0.075

//--------------------------------------------------------------------------------
// BUTTON
//--------------------------------------------------------------------------------

#define BUTTON1_PIN                 15
#define BUTTON1_MODE                BUTTON_PUSHBUTTON

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
#define FORCE_CHANGE_PASS       1
#define HTTP_USERNAME           "admin"
#define WS_BUFFER_SIZE          5
#define WS_TIMEOUT              1800000
#define WEBSERVER_PORT          80
#define DNS_PORT                53
#define ENABLE_MDNS             1

#define WEB_MODE_NORMAL         0
#define WEB_MODE_PASSWORD       1

#define AP_MODE                 AP_MODE_ALONE

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
#define NTP_TIME_OFFSET         1
#define NTP_DAY_LIGHT           true
#define NTP_UPDATE_INTERVAL     1800
