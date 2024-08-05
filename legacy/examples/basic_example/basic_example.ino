#include <SINE_ESC_CAN.h>

// Uncomment the following line to disable CRC verification
// #undef CRC_VERIFICATION

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

SINE_ESC_CAN esc(CS_PIN, 1000000); // CS pin, 1Mbps baud rate

void setup() {
  Serial.begin(115200);
  if (!esc.begin()) {
    Serial.println("Error initializing SINE_ESC_CAN");
    while (1);
  }

  // Set initial throttle to 1100us (typical starting point for paramotors)
  if (esc.setThrottlePWM(1100)) {
    Serial.println("Initial throttle set successfully");
  } else {
    Serial.println("Failed to set initial throttle");
    if (esc.hasError()) {
      Serial.print("Error: ");
      Serial.println(esc.getLastError());
    }
  }
}

void loop() {
  esc.update();

  if (esc.hasError()) {
    Serial.print("Error: ");
    Serial.println(esc.getLastError());
  } else {
    // Print ESC Hardware Information if available
    if (esc.isHardwareInfoAvailable()) {
      Serial.println("ESC Hardware Information:");
      Serial.print("Hardware ID: 0x");
      Serial.println(esc.getHardwareId(), HEX);
      Serial.print("Bootloader Version: ");
      Serial.println(esc.getBootloaderVersion());
      Serial.print("App Version: ");
      Serial.println(esc.getAppVersion());
      Serial.print("Serial Number: ");
      Serial.println(esc.getSerialNumber());
      Serial.println();
    }

    Serial.print("Speed: ");
    Serial.print(esc.getSpeed());
    Serial.println(" rpm");

    Serial.print("Current: ");
    Serial.print(esc.getCurrent());
    Serial.println(" A");

    Serial.print("Voltage: ");
    Serial.print(esc.getVoltage());
    Serial.println(" V");

    Serial.print("MOS Temperature: ");
    Serial.print(esc.getMosTemperature());
    Serial.println(" °C");
  }

  // Example of setting throttle using PWM
  // if (!esc.setThrottlePWM(1200)) { // Increase throttle to 1200us
  //   Serial.print("Failed to set throttle: ");
  //   Serial.println(esc.getLastError());
  // }
}