# HAT State Machine Documentation

## State Machine Overview

All HATs in the SIRI-Elec system implement a standardized state machine for power management, safety, and operational control. This ensures consistent behavior across all subsystems and provides multiple layers of safety for mission-critical operations.

## State Machine Diagram

```mermaid
stateDiagram-v2
    [*] --> POWER_OFF
    
    POWER_OFF --> POWER_IDLE : Power On / System Boot
    
    POWER_IDLE --> DISARMED : Base Station Command
    POWER_IDLE --> UNLOCKED : Base Station Command
    
    DISARMED --> LOCKED : Timeout / Safety Command
    DISARMED --> POWER_IDLE : Reset Command
    
    UNLOCKED --> POWER_ARMED : Operator Command (R2 Button)
    UNLOCKED --> POWER_IDLE : Reset Command
    UNLOCKED --> DISARMED : Safety Command
    
    POWER_ARMED --> UNLOCKED : Release Command
    POWER_ARMED --> EMERGENCY_STOP : Emergency Trigger
    POWER_ARMED --> POWER_IDLE : Mission Complete
    
    LOCKED --> DISARMED : Unlock Command
    LOCKED --> POWER_IDLE : Reset Command
    
    EMERGENCY_STOP --> POWER_IDLE : System Reset
    EMERGENCY_STOP --> POWER_OFF : Critical Shutdown
    
    note right of POWER_OFF
        System completely off
        No power to subsystems
    end note
    
    note right of POWER_IDLE
        All subsystems powered
        Default state after boot
        Heartbeat active
        Commands ignored
    end note
    
    note right of DISARMED
        Software interface active
        Motors/actuators locked
        Sensors active
        Commands ignored
    end note
    
    note right of UNLOCKED
        Ready for activation
        Motors/actuators unlocked
        Awaiting arm command
        Limited commands accepted
    end note
    
    note right of POWER_ARMED
        Fully operational
        All commands accepted
        Active mission state
        Full telemetry
    end note
    
    note right of LOCKED
        Safety lock engaged
        All commands blocked
        Motors mechanically locked
        Emergency protocols active
    end note
    
    note right of EMERGENCY_STOP
        Emergency state
        All operations halted
        Safety protocols active
        Requires manual reset
    end note
```

## State Definitions

### POWER_OFF
- **Description**: System completely powered down
- **Characteristics**:
  - No power to HAT subsystems
  - CAN interface inactive
  - No heartbeat transmission
- **Entry Conditions**: System shutdown, power loss
- **Exit Conditions**: Power button pressed, external power applied

### POWER_IDLE
- **Description**: Default operational state after system boot
- **Characteristics**:
  - All electrical systems powered
  - CAN interface active
  - Heartbeat transmission active
  - Commands ignored (safety feature)
  - Telemetry transmission active
- **Entry Conditions**: System boot, reset commands, mission completion
- **Exit Conditions**: Base station unlock/disarm commands

### DISARMED
- **Description**: Software interface active but hardware locked
- **Characteristics**:
  - Software interfaces responsive
  - Motors/actuators electrically disabled
  - Sensors fully operational
  - Configuration commands accepted
  - Movement commands ignored
- **Entry Conditions**: Base station disarm command, safety timeout
- **Exit Conditions**: Lock command, reset command, unlock command

### UNLOCKED
- **Description**: Ready for activation, awaiting operator command
- **Characteristics**:
  - Motors/actuators unlocked and ready
  - Limited command set accepted
  - Calibration procedures available
  - Awaiting arm command from operator
- **Entry Conditions**: Base station unlock command
- **Exit Conditions**: Operator arm command (R2), safety command, reset

### POWER_ARMED
- **Description**: Fully operational mission state
- **Characteristics**:
  - All systems fully operational
  - Complete command set accepted
  - Active mission execution
  - Full telemetry and feedback
  - Real-time control active
- **Entry Conditions**: Operator arm command (R2 button press)
- **Exit Conditions**: Mission complete, emergency stop, operator disarm

### LOCKED
- **Description**: Safety lock state with mechanical/electrical locks
- **Characteristics**:
  - All commands blocked
  - Motors mechanically/electrically locked
  - Emergency protocols active
  - Requires explicit unlock command
- **Entry Conditions**: Safety timeout, emergency condition, explicit lock command
- **Exit Conditions**: Authorized unlock command, system reset

### EMERGENCY_STOP
- **Description**: Emergency halt state
- **Characteristics**:
  - All operations immediately halted
  - Safety protocols activated
  - Emergency telemetry transmission
  - Requires manual intervention
- **Entry Conditions**: Emergency button, critical fault, BPS alert
- **Exit Conditions**: Manual reset, critical shutdown

## State Transitions

### Command-Based Transitions
| From State | Command | To State | Authority |
|------------|---------|----------|-----------|
| POWER_IDLE | UNLOCK | UNLOCKED | Base Station |
| POWER_IDLE | DISARM | DISARMED | Base Station |
| DISARMED | LOCK | LOCKED | Base Station/Timeout |
| DISARMED | RESET | POWER_IDLE | Base Station |
| UNLOCKED | ARM | POWER_ARMED | Operator (R2) |
| UNLOCKED | DISARM | DISARMED | Base Station |
| POWER_ARMED | DISARM | UNLOCKED | Operator |
| LOCKED | UNLOCK | DISARMED | Base Station |
| Any State | EMERGENCY | EMERGENCY_STOP | Any Source |

### Automatic Transitions
- **Safety Timeout**: DISARMED → LOCKED (after 5 minutes of inactivity)
- **Mission Complete**: POWER_ARMED → POWER_IDLE (task completion)
- **Communication Loss**: Any Active State → LOCKED (after 10 seconds)
- **Power Loss**: Any State → POWER_OFF (immediate)

## Mission Use Case: Lander Egress

### Scenario: Descend down egress ramp on the Lander

#### Pre-Mission Setup
1. **POWER_OFF → POWER_IDLE**
   - Operator presses power button on rover
   - All hardware electrical systems receive power
   - Jetson boots and initializes
   - All HATs enter POWER_IDLE state
   - Radio communications established

2. **Base Station Connection**
   - Operator SSH into rover from base station
   - Launch ROS2 on rover via SSH
   - Launch ROS2 on base station
   - Confirm communication link

#### Mission Activation Sequence
3. **POWER_IDLE → UNLOCKED**
   - Base station operator sends unlock command
   - DriveHat: Motors unlocked, ready for commands
   - SenseHat: Cameras and sensors active
   - ArmHat: Remains in safe position
   - SciHat: Payload systems ready but inactive

4. **UNLOCKED → POWER_ARMED**
   - Operator turns on joystick
   - Operator presses R2 button (ARM command)
   - DriveHat: Fully operational, accepting movement commands
   - SenseHat: Full telemetry streaming to base station
   - All systems ready for mission execution

#### Mission Execution
5. **Active Operation (POWER_ARMED)**
   - Operator uses joystick to move rover forward
   - Real-time camera feed to base station
   - SLAM/IMU data for position tracking
   - Continuous telemetry for "all wheels contact ground" confirmation
   - RVIZ visualization of rover pose

#### Safety Considerations
- **Emergency Stop**: Any critical condition triggers EMERGENCY_STOP
- **Communication Loss**: 10-second timeout triggers LOCKED state
- **Battery Critical**: BPS triggers emergency protocols
- **Operator Override**: R2 release returns to UNLOCKED state

## Implementation Requirements

### State Machine Messages
```cpp
// State machine control messages
#define MSG_TYPE_STATE_UNLOCK       0x30
#define MSG_TYPE_STATE_DISARM       0x31
#define MSG_TYPE_STATE_ARM          0x32
#define MSG_TYPE_STATE_LOCK         0x33
#define MSG_TYPE_STATE_EMERGENCY    0x34
#define MSG_TYPE_STATE_RESET        0x35
#define MSG_TYPE_STATE_QUERY        0x36
#define MSG_TYPE_STATE_RESPONSE     0x37
```

### State Enumeration
```cpp
typedef enum {
    STATE_POWER_OFF = 0,
    STATE_POWER_IDLE = 1,
    STATE_DISARMED = 2,
    STATE_UNLOCKED = 3,
    STATE_POWER_ARMED = 4,
    STATE_LOCKED = 5,
    STATE_EMERGENCY_STOP = 6
} HAT_State_t;
```

### State Machine Interface
```cpp
class HATStateMachine {
public:
    HAT_State_t getCurrentState();
    bool transitionToState(HAT_State_t newState, uint8_t authority);
    bool isCommandAllowed(uint8_t commandType);
    void handleTimeout();
    void handleEmergency();
    uint32_t getStateUptime();
};
```

## Safety Features

### Timeout Protection
- **DISARMED Timeout**: 5 minutes → LOCKED
- **Communication Timeout**: 10 seconds → LOCKED
- **Heartbeat Timeout**: 5 seconds → EMERGENCY_STOP

### Authority Levels
- **Base Station**: Can unlock, disarm, lock, reset
- **Operator**: Can arm, disarm (from armed state)
- **System**: Can trigger emergency stop, timeouts
- **BPS**: Can trigger emergency shutdown

### Fail-Safe Mechanisms
- Power loss → Immediate safe state
- Communication loss → Automatic lock
- Critical faults → Emergency stop
- Unknown commands → Ignored in safe states