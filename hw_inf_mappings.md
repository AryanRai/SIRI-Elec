# Hardware Interface Mappings

## CAN Network Address Assignments

### Priority-Based Node Addressing
Based on the CAN arbitration system, lower priority numbers have higher precedence on the network.

| Priority | Node ID | HAT Name | Function | Address Range |
|----------|---------|----------|----------|---------------|
| 0 | 0x0 | N/A    | High Priority Messages | 0x000-0x0FF|
| 1 | 0x1 | Jetson | Main Compute Master | 0x100-0x1FF |
| 2 | 0x2 | DriveHat | Drive motors and odometry | 0x200-0x2FF |
| 3 | 0x3 | ArmHat | Robotic arm control | 0x300-0x3FF |
| 4 | 0x4 | SciHat  | Scientific payload| 0x400-0x4FF |
| 5 | 0x5 | BPS | Battery Protection System  | 0x500-0x5FF |
| 6 | 0x6 | SenseHat | Environmental sensing | 0x600-0x6FF |

For all address ranges, the main messages all start from address 0x\_10, this is to reserve the first 16 messages for state machine operations.

## HAT Component Mappings

### Priority Zero
Priority zero is reserved for high priority messages. This includes emergency stop.
```
Components:
- Emergency Stop: 0x010
- Emergency Stop Response: 0x011-0x015
```
The address of an Estop response varies depending on which Hat has responded. For instance if the DriveHat acknowledges the Estop response, it will take the address 0x012.

### Jetson
Currently, no messages have priority Jetson. 

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

### ArmHat (Priority 3, Base Address: 0x300)
```
Components:
- Joint Controllers: 0x310-0x31F
  - Base Joint: 0x310
  - Shoulder Joint: 0x311
  - Elbow Joint: 0x312
  - Wrist Joint: 0x313
  - End Effector: 0x314
- Position Feedback: 0x320-0x32F
- Force/Torque Sensors: 0x330-0x33F
```

### SciHat (Priority 4, Base Address: 0x500)
```
Components:
- Sample Collection: 0x410-0x41F
- Spectrometer: 0x420-0x42F
- Camera Systems: 0x430-0x43F
- Environmental Sensors: 0x440-0x44F
```

### BPS (Priority 5, Base Address: 0x500)
```
Components:
- Voltage Monitoring: 0x510-0x51F
- Current Monitoring: 0x520-0x52F
- Temperature Sensors: 0x530-0x53F
- State of Charge: 0x540-0x54F
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

### CAN ID Format (11 bits)
```
Bits 11-9: Priority (3 bits)
Bits 8-0: Message Type (8 bits) 
```


## State Machine Address Mappings

### State Machine Control Addresses
Each HAT reserves addresses for state machine operations:

```
State Machine Base: HAT_BASE_ADDRESS
- Heartbeat Request - 0x06
- Heartbeat Response - 0x07
- State Query: 0x08
- State Response: 0x09
- State Transition: 0x0A
- Authority Check: 0x0B
- Timeout Config: 0x0C
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
- Auto-timeout to DISARMED if no messages received (then DISARMED → LOCKED)
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

## Mapping Implementation
The mappings can be found in the following [file](https://github.sydney.edu.au/Sydney-Interplanetary-Rover-Initiative/sirius-hardware/blob/main/src/sirius_can/include/sirius_can/can_map.hpp) for the Jetson.
