# SIRI-Elec Technical Documentation

## CAN Network API Specification

### Message Format

#### Extended CAN ID Structure (29-bit)
```
Bits 28-24: Priority (5 bits) - Lower number = higher priority
Bits 23-16: Source Node ID (8 bits)
Bits 15-8:  Target Node ID (8 bits) - 0xFF for broadcast
Bits 7-0:   Message Type/Register (8 bits)
```

#### Data Frame Structure
```cpp
struct CANMessage {
    uint32_t id;           // Extended CAN ID
    uint8_t data[8];       // Data payload (0-8 bytes)
    uint8_t length;        // Data length (0-8)
};
```

### Core API Functions

#### HAT Base Class Interface
```cpp
class HATInterface {
public:
    // Initialization
    virtual bool initialize(uint8_t nodeId, uint32_t baseAddress) = 0;
    
    // Message handling
    virtual void handleCANMessage(uint32_t canId, uint8_t* data, uint8_t length) = 0;
    virtual void sendCANMessage(uint32_t canId, uint8_t* data, uint8_t length) = 0;
    
    // State machine
    virtual HAT_State_t getCurrentState() = 0;
    virtual bool transitionToState(HAT_State_t newState, uint8_t authority) = 0;
    virtual bool isCommandAllowed(uint8_t commandType) = 0;
    virtual void handleStateTimeout() = 0;
    
    // Telemetry
    virtual void sendTelemetry() = 0;
    virtual void sendHeartbeat() = 0;
    
    // Emergency handling
    virtual void handleEmergency(uint8_t emergencyCode) = 0;
    virtual void enterSafeMode() = 0;
    
    // Component control
    virtual bool setComponentState(uint16_t componentAddr, uint8_t state) = 0;
    virtual uint8_t getComponentState(uint16_t componentAddr) = 0;
};
```

### Message Types

#### Control Commands (0x00-0x0F)
```cpp
#define MSG_TYPE_CONTROL_START      0x00
#define MSG_TYPE_CONTROL_STOP       0x01
#define MSG_TYPE_CONTROL_RESET      0x02
#define MSG_TYPE_CONTROL_ENABLE     0x03
#define MSG_TYPE_CONTROL_DISABLE    0x04
#define MSG_TYPE_CONTROL_SET_PARAM  0x05
```

#### State Machine Commands (0x30-0x3F)
```cpp
#define MSG_TYPE_STATE_UNLOCK       0x30
#define MSG_TYPE_STATE_DISARM       0x31
#define MSG_TYPE_STATE_ARM          0x32
#define MSG_TYPE_STATE_LOCK         0x33
#define MSG_TYPE_STATE_EMERGENCY    0x34
#define MSG_TYPE_STATE_RESET        0x35
#define MSG_TYPE_STATE_QUERY        0x36
#define MSG_TYPE_STATE_RESPONSE     0x37
```

#### Status Requests (0x10-0x1F)
```cpp
#define MSG_TYPE_STATUS_REQUEST     0x10
#define MSG_TYPE_STATUS_RESPONSE    0x11
#define MSG_TYPE_PARAM_REQUEST      0x12
#define MSG_TYPE_PARAM_RESPONSE     0x13
#define MSG_TYPE_DIAGNOSTIC_REQ     0x14
#define MSG_TYPE_DIAGNOSTIC_RESP    0x15
```

#### Telemetry Data (0x20-0x2F)
```cpp
#define MSG_TYPE_TELEMETRY_SENSOR   0x20
#define MSG_TYPE_TELEMETRY_STATUS   0x21
#define MSG_TYPE_TELEMETRY_ERROR    0x22
#define MSG_TYPE_HEARTBEAT          0x23
```

#### Emergency Messages (0xF0-0xFF)
```cpp
#define MSG_TYPE_EMERGENCY_STOP     0xF0
#define MSG_TYPE_EMERGENCY_BATTERY  0xF1
#define MSG_TYPE_EMERGENCY_THERMAL  0xF2
#define MSG_TYPE_EMERGENCY_COMM     0xF3
#define MSG_TYPE_SYSTEM_SHUTDOWN    0xFF
```

### Node-Specific APIs

#### Jetson Master Node (Priority 1, ID 0x01)
```cpp
class JetsonMaster : public HATInterface {
public:
    // Mission control
    bool startMission();
    bool pauseMission();
    bool abortMission();
    
    // Node management
    bool queryNodeStatus(uint8_t nodeId);
    bool configureNode(uint8_t nodeId, uint8_t* config);
    
    // Data aggregation
    void collectTelemetry();
    void logSystemState();
};
```

#### DriveHat API (Priority 2, ID 0x02)
```cpp
class DriveHat : public HATInterface {
public:
    // Drive motor control
    bool setDriveSpeed(uint8_t wheelId, int16_t speed);
    bool setDriveDirection(uint8_t wheelId, uint8_t direction);
    bool enableDriveMotor(uint8_t wheelId);
    bool disableDriveMotor(uint8_t wheelId);
    
    // Steer motor control
    bool setSteerAngle(uint8_t wheelId, float angle);
    bool setSteerSpeed(uint8_t wheelId, int16_t speed);
    bool enableSteerMotor(uint8_t wheelId);
    bool disableSteerMotor(uint8_t wheelId);
    
    // Encoder feedback
    int32_t getDriveEncoderCount(uint8_t wheelId);
    int32_t getSteerEncoderCount(uint8_t wheelId);
    void resetDriveEncoder(uint8_t wheelId);
    void resetSteerEncoder(uint8_t wheelId);
    
    // High-level movement commands
    bool moveForward(uint16_t distance);
    bool moveBackward(uint16_t distance);
    bool turnInPlace(int16_t angle);
    bool steerToAngle(float frontAngle, float rearAngle);
    bool crabWalk(uint8_t direction, uint16_t distance);
    bool stop();
    
    // Wheel definitions
    enum WheelID {
        WHEEL_LEFT_FRONT = 0,
        WHEEL_RIGHT_FRONT = 1,
        WHEEL_LEFT_REAR = 2,
        WHEEL_RIGHT_REAR = 3
    };
};
```

#### BPS API (Priority 3, ID 0x03)
```cpp
class BPS : public HATInterface {
public:
    // Battery monitoring
    float getBatteryVoltage();
    float getBatteryCurrent();
    float getBatteryTemperature();
    uint8_t getStateOfCharge();
    
    // Protection
    bool checkOvervoltage();
    bool checkUndervoltage();
    bool checkOvercurrent();
    bool checkOvertemperature();
    
    // Emergency
    void emergencyShutdown();
    void sendBatteryAlert(uint8_t alertType);
};
```

#### ArmHat API (Priority 4, ID 0x04)
```cpp
class ArmHat : public HATInterface {
public:
    // Joint control
    bool setJointPosition(uint8_t jointId, float position);
    bool setJointVelocity(uint8_t jointId, float velocity);
    float getJointPosition(uint8_t jointId);
    float getJointTorque(uint8_t jointId);
    
    // End effector
    bool openGripper();
    bool closeGripper();
    bool setGripperForce(float force);
    
    // Kinematics
    bool moveToPosition(float x, float y, float z);
    bool followTrajectory(float* waypoints, uint8_t numPoints);
};
```

### Communication Protocols

#### Heartbeat Protocol
```cpp
// Sent every 1 second by each node
struct HeartbeatMessage {
    uint8_t nodeId;
    uint8_t status;        // 0=OK, 1=Warning, 2=Error
    uint8_t errorCode;     // Specific error if status != 0
    uint32_t uptime;       // Milliseconds since boot
    uint8_t reserved;
};
```

#### Telemetry Protocol
```cpp
// Sensor data format
struct TelemetryMessage {
    uint16_t sensorId;
    uint8_t dataType;      // 0=int, 1=float, 2=bool
    uint32_t timestamp;
    union {
        int32_t intValue;
        float floatValue;
        bool boolValue;
    } data;
};
```

### Error Handling

#### Error Codes
```cpp
#define ERROR_NONE              0x00
#define ERROR_COMM_TIMEOUT      0x01
#define ERROR_INVALID_COMMAND   0x02
#define ERROR_HARDWARE_FAULT    0x03
#define ERROR_SENSOR_FAILURE    0x04
#define ERROR_ACTUATOR_FAULT    0x05
#define ERROR_POWER_ISSUE       0x06
#define ERROR_THERMAL_LIMIT     0x07
#define ERROR_EMERGENCY_STOP    0x08
```

#### Timeout Handling
- **Heartbeat Timeout**: 5 seconds - Node considered offline
- **Command Timeout**: 1 second - Resend command
- **Emergency Timeout**: 100ms - Immediate safe mode

### Configuration Parameters

#### CAN Network Settings
```cpp
#define CAN_BAUDRATE            1000000  // 1 Mbps
#define CAN_MAX_NODES           32
#define HEARTBEAT_INTERVAL      1000     // 1 second
#define TELEMETRY_INTERVAL      100      // 100ms
#define EMERGENCY_TIMEOUT       100      // 100ms
```

#### Buffer Sizes
```cpp
#define CAN_TX_BUFFER_SIZE      64
#define CAN_RX_BUFFER_SIZE      128
#define TELEMETRY_BUFFER_SIZE   256
#define LOG_BUFFER_SIZE         1024
```

### CAN Transceiver Selection

#### Hardware Requirements
- **Jetson Backplane**: SN65HVD230 (3.3V compatible with Jetson GPIO)
- **HAT Modules (Teensy)**: TJA1050 (5V tolerant, robust for distributed modules)

#### Transceiver Specifications
```cpp
// Jetson Backplane Configuration
#define JETSON_CAN_TRANSCEIVER "SN65HVD230"
#define JETSON_CAN_VOLTAGE 3.3f

// HAT Module Configuration  
#define HAT_CAN_TRANSCEIVER "TJA1050"
#define HAT_CAN_VOLTAGE 5.0f
```

### Implementation Guidelines

1. **Thread Safety**: All CAN operations must be thread-safe
2. **Real-time Constraints**: Emergency messages have 100ms deadline
3. **Memory Management**: Use static allocation for real-time paths
4. **Error Recovery**: Implement graceful degradation on component failure
5. **Testing**: All HATs must pass CAN network compliance tests
6. **Transceiver Compatibility**: Ensure proper voltage levels for each platform

## State Machine API

### State Definitions
```cpp
typedef enum {
    STATE_POWER_OFF = 0,
    STATE_DISARMED = 1,      // Initial state after boot
    STATE_LOCKED = 2,        // Default operational state
    STATE_UNLOCKED = 3,
    STATE_POWER_ARMED = 4,
    STATE_EMERGENCY_STOP = 5
} HAT_State_t;
```

### Authority Levels
```cpp
typedef enum {
    AUTHORITY_SYSTEM = 0,
    AUTHORITY_BASE_STATION = 1,
    AUTHORITY_OPERATOR = 2,
    AUTHORITY_BPS = 3,
    AUTHORITY_EMERGENCY = 4
} Authority_t;
```

### State Machine Interface
```cpp
class HATStateMachine {
public:
    HAT_State_t getCurrentState();
    bool transitionToState(HAT_State_t newState, Authority_t authority);
    bool isCommandAllowed(uint8_t commandType);
    void handleTimeout();
    void handleEmergency();
    uint32_t getStateUptime();
    const char* getStateName(HAT_State_t state);
    
    // State-specific behaviors
    virtual void onEnterState(HAT_State_t state) = 0;
    virtual void onExitState(HAT_State_t state) = 0;
    virtual bool validateTransition(HAT_State_t from, HAT_State_t to) = 0;
};
```

### Command Authorization Matrix
```cpp
// Commands allowed in each state (bitmask)
#define POWER_OFF_ALLOWED       0x00000000
#define DISARMED_ALLOWED        0x00000003  // Config + Status (initial state)
#define LOCKED_ALLOWED          0x00000001  // Heartbeat only (default state)
#define UNLOCKED_ALLOWED        0x00000007  // + Limited control
#define POWER_ARMED_ALLOWED     0xFFFFFFFF  // All commands
#define EMERGENCY_ALLOWED       0x00000001  // Heartbeat only
```

### Development Tools

#### CAN Network Analyzer
```cpp
class CANAnalyzer {
public:
    void startCapture();
    void stopCapture();
    void analyzeTraffic();
    void generateReport();
    void analyzeStateMachine();  // New: State transition analysis
};
```

#### HAT Simulator
```cpp
class HATSimulator {
public:
    bool loadHATConfig(const char* configFile);
    void simulateHAT(uint8_t nodeId);
    void injectFault(uint8_t faultType);
    void simulateStateTransition(HAT_State_t state);  // New: State testing
    void testEmergencyScenarios();  // New: Emergency testing
};
```

## Arduino IDE Development

### Project Structure
HAT firmware uses Arduino IDE with organized include/src structure:
```
TemplateHAT_Firmware/
├── TemplateHAT_Firmware.ino  # Main sketch
├── include/                  # Header files
│   ├── hat_config.h         # Configuration
│   ├── can_protocol.h       # CAN definitions
│   ├── state_machine.h      # State machine
│   └── ...                  # Other headers
└── src/                     # Implementation files
    ├── can_interface.cpp    # CAN implementation
    ├── state_machine.cpp    # State machine logic
    └── ...                  # Other implementations
```

### Required Libraries
- **FlexCAN_T4**: CAN communication for Teensy 4.1
- **ArduinoJson**: JSON parsing and configuration

### Development Workflow
1. **Open Arduino IDE** with Teensyduino installed
2. **Load sketch**: Open TemplateHAT_Firmware.ino
3. **Install libraries**: Use Library Manager for dependencies
4. **Configure board**: Select Teensy 4.1, 600MHz, Serial USB
5. **Customize**: Edit include/hat_config.h for your HAT
6. **Upload**: Connect Teensy and upload firmware

### Debugging
- **Serial Monitor**: 115200 baud for debug output
- **LED Indicators**: Status, error, and communication LEDs
- **CAN Analyzer**: Use external tools for network debugging