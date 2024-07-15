#include "SINE_ESC_CAN.h"

SINE_ESC_CAN::SINE_ESC_CAN(uint8_t csPin, long baudrate)
    : mcp(csPin), baudrate(baudrate) {
    speed = 0;
    current = 0;
    busCurrent = 0;
    voltage = 0;
    modulationRatio = 0;
    mosTemperature = 0;
    capacitorTemperature = 0;
    mcuTemperature = 0;
    runningError = 0;
    selfCheckError = 0;
    motorTemperature = 0;
    runningTime = 0;
    errorFlag = false;
    lastError = "";
    hardwareInfoReceived = false;
    memset(serialNumber, 0, sizeof(serialNumber));
}

bool SINE_ESC_CAN::begin() {
    return mcp.begin(baudrate);
}

void SINE_ESC_CAN::update() {
    int packetSize = mcp.parsePacket();
    if (packetSize) {
        uint8_t data[8];
        uint32_t id = mcp.packetId();
        for (int i = 0; i < packetSize; i++) {
            data[i] = mcp.read();
        }
        parsePacket(id, data, packetSize);
    }
}

void SINE_ESC_CAN::parsePacket(uint32_t id, uint8_t* data, uint8_t len) {
    if (id == 0x004E3420) { // ESC Broadcast Status Information
        #ifdef CRC_VERIFICATION
        uint16_t receivedCRC = (data[len-2] << 8) | data[len-1];
        if (!verifyCRC16(data, len-2, receivedCRC)) {
            setError("CRC16 verification failed");
            return;
        }
        #endif

        speed = (int32_t)(data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24));
        current = (int16_t)(data[8] | (data[9] << 8));
        busCurrent = (int16_t)(data[10] | (data[11] << 8));
        voltage = (uint16_t)(data[12] | (data[13] << 8));
        modulationRatio = (uint16_t)(data[14] | (data[15] << 8));
        mosTemperature = (int16_t)(data[16] | (data[17] << 8));
        capacitorTemperature = (int16_t)(data[18] | (data[19] << 8));
        mcuTemperature = (int16_t)(data[20] | (data[21] << 8));
        runningError = (uint16_t)(data[22] | (data[23] << 8));
        motorTemperature = (int16_t)(data[24] | (data[25] << 8));
        runningTime = (uint16_t)(data[26] | (data[27] << 8));
        clearError();
    } else if (id == 0x00D101A0) { // ESC Hardware Information
        hardwareId = (uint16_t)(data[0] | (data[1] << 8));
        bootloaderVersion = (uint16_t)(data[2] | (data[3] << 8));
        appVersion = (uint16_t)(data[4] | (data[5] << 8));
        memcpy(serialNumber, &data[10], 16);
        serialNumber[16] = '\0'; // Ensure null-termination
        hardwareInfoReceived = true;
    }
}

bool SINE_ESC_CAN::sendCommand(uint32_t id, uint8_t* data, uint8_t len) {
    #ifdef CRC_VERIFICATION
    uint16_t crc = calculateCRC16(data, len);
    data[len] = (crc >> 8) & 0xFF;
    data[len+1] = crc & 0xFF;
    len += 2;
    #endif
    
    mcp.beginPacket(id);
    mcp.write(data, len);
    bool success = mcp.endPacket();
    
    if (!success) {
        setError("Failed to send command");
    }
    return success;
}

uint16_t SINE_ESC_CAN::calculateCRC16(uint8_t* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool SINE_ESC_CAN::verifyCRC16(uint8_t* data, uint16_t length, uint16_t receivedCRC) {
    uint16_t calculatedCRC = calculateCRC16(data, length);
    return calculatedCRC == receivedCRC;
}

void SINE_ESC_CAN::setError(const String& error) {
    errorFlag = true;
    lastError = error;
}

void SINE_ESC_CAN::clearError() {
    errorFlag = false;
    lastError = "";
}

bool SINE_ESC_CAN::hasError() {
    return errorFlag;
}

String SINE_ESC_CAN::getLastError() {
    return lastError;
}

// Getter methods (unchanged)
float SINE_ESC_CAN::getSpeed() { return speed; }
float SINE_ESC_CAN::getCurrent() { return current * 0.1f; }
float SINE_ESC_CAN::getBusCurrent() { return busCurrent * 0.1f; }
float SINE_ESC_CAN::getVoltage() { return voltage * 0.1f; }
float SINE_ESC_CAN::getModulationRatio() { return modulationRatio * 0.1f; }
float SINE_ESC_CAN::getMosTemperature() { return mosTemperature * 0.1f; }
float SINE_ESC_CAN::getCapacitorTemperature() { return capacitorTemperature * 0.1f; }
float SINE_ESC_CAN::getMcuTemperature() { return mcuTemperature * 0.1f; }
uint16_t SINE_ESC_CAN::getRunningError() { return runningError; }
uint16_t SINE_ESC_CAN::getSelfCheckError() { return selfCheckError; }
float SINE_ESC_CAN::getMotorTemperature() { return motorTemperature * 0.1f; }
uint16_t SINE_ESC_CAN::getRunningTime() { return runningTime; }

// ESC Hardware Information getters
uint16_t SINE_ESC_CAN::getHardwareId() { return hardwareId; }
uint16_t SINE_ESC_CAN::getBootloaderVersion() { return bootloaderVersion; }
uint16_t SINE_ESC_CAN::getAppVersion() { return appVersion; }
String SINE_ESC_CAN::getSerialNumber() { return String(serialNumber); }
bool SINE_ESC_CAN::isHardwareInfoAvailable() { return hardwareInfoReceived; }

// Setter methods
bool SINE_ESC_CAN::setThrottlePWM(uint16_t pwm_us) {
    uint8_t data[3];
    data[0] = pwm_us & 0xFF;
    data[1] = (pwm_us >> 8) & 0xFF;
    data[2] = 0xC0;
    return sendCommand(0x00D5A081, data, 3);
}

bool SINE_ESC_CAN::setThrottleRPM(int32_t rpm) {
    uint8_t data[5];
    data[0] = rpm & 0xFF;
    data[1] = (rpm >> 8) & 0xFF;
    data[2] = (rpm >> 16) & 0xFF;
    data[3] = (rpm >> 24) & 0xFF;
    data[4] = 0xC0;
    return sendCommand(0x00D4A081, data, 5);
}

bool SINE_ESC_CAN::setMechanicalAngle(int16_t angle) {
    uint8_t data[3];
    data[0] = angle & 0xFF;
    data[1] = (angle >> 8) & 0xFF;
    data[2] = 0xC0;
    return sendCommand(0x00D8A081, data, 3);
}