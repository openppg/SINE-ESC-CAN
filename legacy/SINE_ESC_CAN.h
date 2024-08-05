#ifndef SINE_ESC_CAN_H
#define SINE_ESC_CAN_H

#include <Arduino.h>
#include <Adafruit_MCP2515.h>

// Define CRC_VERIFICATION to enable CRC checks. Comment out to disable.
#define CRC_VERIFICATION

class SINE_ESC_CAN {
public:
    SINE_ESC_CAN(uint8_t csPin, long baudrate);
    bool begin();
    void update();

    // Getter methods
    float getSpeed();
    float getCurrent();
    float getBusCurrent();
    float getVoltage();
    float getModulationRatio();
    float getMosTemperature();
    float getCapacitorTemperature();
    float getMcuTemperature();
    uint16_t getRunningError();
    uint16_t getSelfCheckError();
    float getMotorTemperature();
    uint16_t getRunningTime();

    // ESC Hardware Information
    uint16_t getHardwareId();
    uint16_t getBootloaderVersion();
    uint16_t getAppVersion();
    String getSerialNumber();
    bool isHardwareInfoAvailable();

    // Setter methods
    bool setThrottlePWM(uint16_t pwm_us);
    bool setThrottleRPM(int32_t rpm);
    bool setMechanicalAngle(int16_t angle);

    // Error checking methods
    bool hasError();
    String getLastError();

private:
    Adafruit_MCP2515 mcp;
    long baudrate;

    // ESC data
    int32_t speed;
    int16_t current;
    int16_t busCurrent;
    uint16_t voltage;
    uint16_t modulationRatio;
    int16_t mosTemperature;
    int16_t capacitorTemperature;
    int16_t mcuTemperature;
    uint16_t runningError;
    uint16_t selfCheckError;
    int16_t motorTemperature;
    uint16_t runningTime;

    // ESC Hardware Information
    uint16_t hardwareId;
    uint16_t bootloaderVersion;
    uint16_t appVersion;
    char serialNumber[17];
    bool hardwareInfoReceived;

    // Error handling
    bool errorFlag;
    String lastError;

    void parsePacket(uint32_t id, uint8_t* data, uint8_t len);
    bool sendCommand(uint32_t id, uint8_t* data, uint8_t len);
    uint16_t calculateCRC16(uint8_t* data, uint16_t length);
    bool verifyCRC16(uint8_t* data, uint16_t length, uint16_t receivedCRC);
    void setError(const String& error);
    void clearError();
};

#endif