#include <SineEsc.h>
#include <CanardAdapter.h>
#include <Adafruit_MCP2515.h>
#include <Arduino.h>

#include <stdint.h>


#ifdef ESP8266
   #define CS_PIN    2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
   #define CS_PIN    14
#elif defined(TEENSYDUINO)
   #define CS_PIN    8
#elif defined(ARDUINO_STM32_FEATHER)
   #define CS_PIN    PC5
#elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
   #define CS_PIN    27
#elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define CS_PIN    P3_2
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
   #define CS_PIN    7
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN)
   #define CS_PIN    PIN_CAN_CS
#elif defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W) // PiCowbell CAN Bus
   #define CS_PIN    20
#else
    // Anything else, defaults!
   #define CS_PIN    5
#endif


/* Fixed node ID. A production application should use DNA instead */
#define LOCAL_NODE_ID 97


static Adafruit_MCP2515 mcp(CS_PIN);
static CanardAdapter adapter(mcp);
static uint8_t memory_pool[1024] __attribute__((aligned(8)));

static SineEsc esc(adapter);

void setup(void)
{
    Serial.begin(115200);
    mcp.begin(1000000);

    adapter.begin(memory_pool, sizeof(memory_pool));
    adapter.setLocalNodeId(LOCAL_NODE_ID);

    esc.begin(0x20); // Default ID for the ESC
}

void loop(void)
{
    adapter.processTxRxOnce();

    static bool hasReceivedFirstStatusInfo = false;
    if (!hasReceivedFirstStatusInfo) {
        const SineEscModel &model = esc.getModel();
        if (model.hasBroadcastStatusInfo) {
            hasReceivedFirstStatusInfo = true;
            Serial.println("Received first broadcast status message from ESC");

            Serial.println("Setting throttle PWM...");
            esc.setThrottleSettings2(1100);
        }
    }

    static unsigned long lastMillis = millis();
    unsigned long now = millis();
    if ((now - lastMillis) >= 1000) {
        lastMillis = now;

        const SineEscModel &model = esc.getModel();

        if (model.hasBroadcastStatusInfo) {
            const sine_esc_BroadcastStatusInfo *b = &model.broadcastStatusInfo;
            Serial.println("Got broadcast status info");

            Serial.print("Speed: ");
            Serial.print(b->speed);
            Serial.println(" rpm");

            Serial.print("Current: ");
            Serial.print(b->current);
            Serial.println(" A");

            Serial.print("Voltage: ");
            Serial.print(b->voltage);
            Serial.println(" V");

            Serial.print("MOS Temperature: ");
            Serial.print(b->mos_temp);
            Serial.println(" °C");
        }

        if (model.hasSetThrottleSettings2Response) {
            Serial.println("Got SetThrottleSettings2 response");
        }

        if (model.hasSetRotationSpeedSettingsResponse) {
            Serial.println("Got SetRotationSpeedSettings response");
        }

        esc.clearModelReceiveFlags();
    }
}
