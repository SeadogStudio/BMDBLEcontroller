#ifndef BMDBLECONTROLLER_H
#define BMDBLECONTROLLER_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// Blackmagic Camera Service and Characteristic UUIDs (from the documentation)
#define BLACKMAGIC_CAMERA_SERVICE_UUID  "291d567a-6d75-11e6-8b77-86f30ca893d3"
#define OUTGOING_CHARACTERISTIC_UUID    "5dd3465f-1aee-4299-8493-d2eca2f8e1bb"
#define INCOMING_CHARACTERISTIC_UUID    "b864e140-76a0-416a-bf30-5876504537d9"
#define CAMERA_STATUS_CHARACTERISTIC_UUID "7fe8691d-95dc-4fc5-8abd-ca74339b51b9"

class BMDBLEController {
public:
    // Constructor
    BMDBLEController();

    // Initialize BLE
    bool begin();

    // Connect to a discovered Blackmagic camera (asynchronous)
    bool connectToCamera(NimBLEAdvertisedDevice* device);

    // Disconnect from the camera
    void disconnect();

    // Check if connected to a camera
    bool isConnected();

    // Check if bonded (paired) with the camera
    bool isBonded();

    // Send a raw command (byte array) - Lower-level, more flexible
    bool sendCommand(uint8_t* command, size_t length);

    // Send a command using individual parameters - Higher-level, easier to use
    bool sendCommand(uint8_t destination, uint8_t commandId, uint8_t category,
                     uint8_t parameter, uint8_t dataType, uint8_t operationType,
                     uint8_t* data, size_t dataLength);

    // --- Example Convenience Functions (Add more for other commands) ---

    // Lens Control
    bool setAperture(float fStop);
    bool autoFocus();

    // Video Control
    bool setISO(int iso);
    bool setWhiteBalance(int kelvin, int tint);
    bool setShutterAngle(float angle); // in degrees
    bool setNDFilter(float stop); // e.g., 0.0, 0.6, 1.2, 1.8, 2.4
    bool autoExposure(uint8_t mode); // 0=Off, 1=Iris, 2=Shutter, 3=Iris+Shutter, 4=Shutter+Iris

    //Media Control
    bool setRecordingFormat(uint8_t fileFrameRate, uint8_t sensorFrameRate, uint16_t frameWidth, uint16_t frameHeight, uint8_t flags);
    bool setTransportMode(uint8_t mode, int8_t speed); //mode: 0=Preview, 1=Play, 2=Record
    bool setCodec(uint8_t basicCodec, uint8_t codeVariant); //basicCodec: 0=RAW, 1=DNxHD, 2=ProRes, 3=Blackmagic RAW

    // Callback function types (for user implementation)
    typedef void (*IncomingDataCallback)(uint8_t* data, size_t length);
    typedef void (*StatusCallback)(uint8_t status);

    // Set callback functions
    void setIncomingDataCallback(IncomingDataCallback callback);
    void setStatusCallback(StatusCallback callback);

private:
    NimBLEClient* _pClient = nullptr;
    NimBLERemoteCharacteristic* _pOutgoingCharacteristic = nullptr;
    NimBLERemoteCharacteristic* _pIncomingCharacteristic = nullptr;
    NimBLERemoteCharacteristic* _pStatusCharacteristic = nullptr;
    bool _connected = false;
    bool _bonded = false;
    bool _callbacksSet = false;

    IncomingDataCallback _incomingDataCallback = nullptr;
    StatusCallback _statusCallback = nullptr;

    // Static callback functions (required by NimBLE library)
    static void _incomingDataNotifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    static void _statusNotifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    static void _onConnectCallback(NimBLEClient* pClient); // Corrected callback
    //static void _onConfirmPIN(uint32_t pin);           // Corrected callback

    // Helper function for fixed-point conversion
    uint16_t _floatToFixed16(float value);
    //static BMDBLEController* _instance; //for static callback to work.
    //uint32_t _passkey = 0; // To store the passkey
};

#endif
