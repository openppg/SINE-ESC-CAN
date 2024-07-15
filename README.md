# SINE ESC CAN Library

The SINE ESC CAN library simplifies communication with SINE ESCs (Electronic Speed Controllers) using the CAN bus protocol. This library is designed for Arduino-compatible boards and uses the Adafruit MCP2515 CAN controller.

## Features

- Read real-time ESC data including:
  - Motor speed
  - Current
  - Voltage
  - Temperature (MOS, Capacitor, MCU, Motor)
  - Error codes
- Set throttle using PWM or RPM values
- Set mechanical angle
- Read ESC hardware information
- Optional CRC16 verification for enhanced reliability

## Installation

1. Download the SINE_ESC_CAN library ZIP file.
2. In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library, and select the downloaded file.
3. Install the required dependencies:
   - Adafruit MCP2515 library (available through the Arduino Library Manager)

## Usage

Here's a basic example of how to use the SINE ESC CAN library:

```cpp
#include <SINE_ESC_CAN.h>

SINE_ESC_CAN esc(5, 1000000); // CS pin 5, 1Mbps baud rate

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
  }
}

void loop() {
  esc.update();

  if (esc.hasError()) {
    Serial.print("Error: ");
    Serial.println(esc.getLastError());
  } else {
    Serial.print("Speed: ");
    Serial.print(esc.getSpeed());
    Serial.println(" rpm");

    Serial.print("Current: ");
    Serial.print(esc.getCurrent());
    Serial.println(" A");

    Serial.print("Voltage: ");
    Serial.print(esc.getVoltage());
    Serial.println(" V");
  }

  delay(100);
}
```

## Configuration

### CRC Verification

By default, CRC verification is enabled. To disable it for debugging purposes, add the following line before including the SINE_ESC_CAN library:

```cpp
#undef CRC_VERIFICATION
#include <SINE_ESC_CAN.h>
```

## API Reference

### Constructor

- `SINE_ESC_CAN(uint8_t csPin, long baudrate)`

### Methods

- `bool begin()`: Initialize the ESC communication
- `void update()`: Update ESC data (call this regularly in your loop)
- `bool setThrottlePWM(uint16_t pwm_us)`: Set throttle using PWM value
- `bool setThrottleRPM(int32_t rpm)`: Set throttle using RPM value
- `bool setMechanicalAngle(int16_t angle)`: Set mechanical angle
- `bool hasError()`: Check if an error has occurred
- `String getLastError()`: Get the last error message

### Getter Methods

- `float getSpeed()`: Get motor speed in RPM
- `float getCurrent()`: Get motor current in Amperes
- `float getVoltage()`: Get battery voltage in Volts
- `float getMosTemperature()`: Get MOSFET temperature in °C
- ... (and more, see header file for full list)

### ESC Hardware Information

- `uint16_t getHardwareId()`: Get ESC hardware ID
- `uint16_t getBootloaderVersion()`: Get bootloader version
- `uint16_t getAppVersion()`: Get application version
- `String getSerialNumber()`: Get ESC serial number
- `bool isHardwareInfoAvailable()`: Check if hardware info has been received

## Contributing

Contributions to the SINE ESC CAN library are welcome. Please feel free to submit pull requests, create issues or suggest improvements.

## License

This library is released under the MIT License. See the LICENSE file for details.