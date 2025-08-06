# Hardware Interface Mappings

## CAN Network Address Assignments

### Priority-Based Node Addressing
Based on the CAN arbitration system, lower priority numbers have higher precedence on the network.

| Priority | Node ID | HAT Name | Function | Address Range |
|----------|---------|----------|----------|---------------|
| 1 | 0x01 | Jetson | Main Compute Master | 0x100-0x1FF |
| 2 | 0x02 | DriveHat | Drive motors and odometry | 0x200-0x2FF |
| 3 | 0x03 | BPS | Battery Protection System | 0x300-0x3FF |
| 4 | 0x04 | ArmHat | Robotic arm control | 0x400-0x4FF |
| 5 | 0x05 | SciHat | Scientific payload | 0x500-0x5FF |
| 6 | 0x06 | SenseHat | Environmental sensing | 0x600-0x6FF |

## HAT Component Mappings

### DriveHat (Priority 2, Base Address: 0x200)
```
Components:
- Drive Motor Controllers: 0x210-0x217
  - Left Front Drive: 0x210
  - Right Front Drive: 0x211
  - Left Rear Drive: 0x212
  - Right Rear Drive: 0x213
- Steer Motor Controllers: 0x214-0x217
  - Left Front Steer: 0x214
  - Right Front Steer: 0x215
  - Left Rear Steer: 0x216
  - Right Rear Steer: 0x217
- Drive Encoders: 0x220-0x227
  - Left Front Drive Encoder: 0x220
  - Right Front Drive Encoder: 0x221
  - Left Rear Drive Encoder: 0x222
  - Right Rear Drive Encoder: 0x223
- Steer Encoders: 0x224-0x227
  - Left Front Steer Encoder: 0x224
  - Right Front Steer Encoder: 0x225
  - Left Rear Steer Encoder: 0x226
  - Right Rear Steer Encoder: 0x227
- IMU/Odometry: 0x230-0x23F
```

### BPS (Priority 3, Base Address: 0x300)
```
Components:
- Voltage Monitoring: 0x310-0x31F
- Current Monitoring: 0x320-0x32F
- Temperature Sensors: 0x330-0x33F
- State of Charge: 0x340-0x34F
- Emergency Shutdown: 0x3F0-0x3FF
```

### ArmHat (Priority 4, Base Address: 0x400)
```
Components:
- Joint Controllers: 0x410-0x41F
  - Base Joint: 0x410
  - Shoulder Joint: 0x411
  - Elbow Joint: 0x412
  - Wrist Joint: 0x413
  - End Effector: 0x414
- Position Feedback: 0x420-0x42F
- Force/Torque Sensors: 0x430-0x43F
```

### SciHat (Priority 5, Base Address: 0x500)
```
Components:
- Sample Collection: 0x510-0x51F
- Spectrometer: 0x520-0x52F
- Camera Systems: 0x530-0x53F
- Environmental Sensors: 0x540-0x54F
```

### SenseHat (Priority 6, Base Address: 0x600)
```
Components:
- IMU Sensors: 0x610-0x61F
- Temperature Sensors: 0x620-0x62F
- Humidity Sensors: 0x630-0x63F
- Pressure Sensors: 0x640-0x64F
- GPS Module: 0x650-0x65F
```

## Message Structure Format

### Extended CAN ID Format (29-bit)
```
Bits 28-24: Priority (5 bits)
Bits 23-16: Source Node ID (8 bits)
Bits 15-8:  Target Node ID (8 bits) - 0xFF for broadcast
Bits 7-0:   Message Type/Register (8 bits)
```

### Message Types
- 0x00-0x0F: Control Commands
- 0x10-0x1F: Status Requests
- 0x20-0x2F: Telemetry Data
- 0x30-0x3F: State Machine Commands
- 0x40-0x4F: Configuration
- 0xF0-0xFF: Emergency/System Messages

## State Machine Address Mappings

### State Machine Control Addresses
Each HAT reserves addresses for state machine operations:

```
State Machine Base: HAT_BASE_ADDRESS + 0xF0
- State Query: 0xF0
- State Response: 0xF1
- State Transition: 0xF2
- Emergency Stop: 0xF3
- Authority Check: 0xF4
- Timeout Config: 0xF5
```

### State-Specific Component Access

#### DISARMED State (Initial boot state)
```
Accessible Components:
- Heartbeat: All HATs
- Basic Telemetry: All HATs
- Status Queries: All HATs
- All sensors and monitoring
- Configuration interfaces
- Diagnostic functions
Blocked Components:
- Motor controllers
- Actuator commands
- Movement operations
- Mission commands

Communication:
- Expects periodic pings from Jetson
- Auto-transitions to LOCKED (default state)
- Maintains CAN interface for configuration
```

#### LOCKED State (Default operational state)
```
Accessible Components:
- Heartbeat: All HATs
- Basic Status: All HATs
- Emergency protocols
- Monitoring sensors
Blocked Components:
- All control commands
- Motor controllers
- Actuator commands
- Movement operations
- Mission commands
- Configuration interfaces

Communication:
- Expects periodic pings from Jetson
- Auto-timeout to LOCKED if no messages received
- Maintains CAN interface for emergency only
```

#### UNLOCKED State (Ready for activation)
```
Accessible Components:
- All sensors
- Motor unlock/enable
- Calibration routines
- Limited movement commands
Blocked Components:
- Full operational commands
- Mission-critical operations
```

#### POWER_ARMED State (Fully operational)
```
Accessible Components:
- All components fully accessible
- Complete command set
- Real-time control
- Mission operations
```

## HAT Template Structure

Each HAT should implement:
1. **Initialization Routine** - Set up CAN interface and component addresses
2. **State Machine** - Implement standardized state transitions and safety
3. **Message Handler** - Process incoming CAN messages based on address mapping
4. **Telemetry Publisher** - Send periodic status updates
5. **Emergency Handler** - Respond to emergency broadcasts
6. **Component Interface** - Abstract layer for hardware components

## Placeholder Assignments

### Template HAT Structure
```cpp
// Component address assignments (to be filled per HAT)
#define HAT_BASE_ADDRESS    0x000  // Set per HAT type
#define COMPONENT_1_ADDR    (HAT_BASE_ADDRESS + 0x10)
#define COMPONENT_2_ADDR    (HAT_BASE_ADDRESS + 0x20)
// ... additional components

// State machine addresses
#define STATE_QUERY_ADDR    (HAT_BASE_ADDRESS + 0xF0)
#define STATE_RESPONSE_ADDR (HAT_BASE_ADDRESS + 0xF1)
#define STATE_TRANSITION_ADDR (HAT_BASE_ADDRESS + 0xF2)
#define EMERGENCY_STOP_ADDR (HAT_BASE_ADDRESS + 0xF3)

// State machine implementation
typedef enum {
    STATE_POWER_OFF = 0,
    STATE_POWER_IDLE = 1,
    STATE_DISARMED = 2,
    STATE_UNLOCKED = 3,
    STATE_POWER_ARMED = 4,
    STATE_LOCKED = 5,
    STATE_EMERGENCY_STOP = 6
} HAT_State_t;

// Message handlers (to be implemented per HAT)
void handleControlMessage(uint32_t address, uint8_t* data);
void handleStatusRequest(uint32_t address);
void handleStateTransition(HAT_State_t newState, uint8_t authority);
bool isCommandAllowed(uint32_t address, HAT_State_t currentState);
void sendTelemetry();
void handleEmergency();
```

## Mission Use Case Integration

### Lander Egress Scenario Address Usage

#### Pre-Mission (POWER_IDLE → UNLOCKED)
```cpp
// Base station sends unlock command
CAN_ID: 0x01FF0230  // Jetson to All HATs, State Unlock
Data: [UNLOCK_CMD, BASE_STATION_AUTH, 0, 0, 0, 0, 0, 0]

// HATs respond with state confirmation
CAN_ID: 0x02010231  // DriveHat to Jetson, State Response
Data: [STATE_UNLOCKED, 0, 0, 0, 0, 0, 0, 0]
```

#### Mission Activation (UNLOCKED → POWER_ARMED)
```cpp
// Operator presses R2 button - ARM command
CAN_ID: 0x01FF0232  // Jetson to All HATs, State Arm
Data: [ARM_CMD, OPERATOR_AUTH, 0, 0, 0, 0, 0, 0]

// DriveHat enables motor controllers
// Address 0x210-0x213 become accessible for movement commands
```

#### Active Mission (POWER_ARMED operations)
```cpp
// Movement command now allowed
CAN_ID: 0x01020005  // Jetson to DriveHat, Set Motor Speed
Data: [MOTOR_LEFT_FRONT, SPEED_HIGH, DIRECTION_FORWARD, 0, 0, 0, 0, 0]
// This command would be blocked in any state except POWER_ARMED
```

This structure ensures consistent addressing across all HATs while allowing for HAT-specific component implementations and mission-specific state management.