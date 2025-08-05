# SIRI-Elec
Electrical systems for SIRI rover - A modular hardware interface framework

## Overview

SIRI-Elec provides a comprehensive hardware interface system for the SIRI rover, built around a high-speed CAN network backbone that connects all rover subsystems. The system uses a modular HAT-based architecture for scalability and maintainability.

## Architecture

### System Overview Diagram

```mermaid
graph TB
    subgraph "SIRI Rover Electrical System"
        subgraph "Backplane"
            Jetson[Jetson Orin Nano<br/>Main Compute]
            CAN1[CAN Transceiver 1<br/>TJA1050]
            CAN2[CAN Transceiver 2<br/>TJA1050 - Redundant]
            Power[Power Distribution]
            
            Jetson --- CAN1
            Jetson --- CAN2
            Power --- Jetson
        end
        
        subgraph "CAN Network Bus"
            CANBus[CAN High/Low + Ground<br/>1 Mbps, Multi-drop]
        end
        
        subgraph "HAT Modules"
            subgraph "DriveHat - Priority 2"
                D_MCU[Microcontroller]
                D_CAN[TJA1050]
                D_Motors[Motor Controllers<br/>0x210-0x213]
                D_Encoders[Encoders<br/>0x220-0x223]
                D_IMU[IMU/Odometry<br/>0x230-0x23F]
                
                D_MCU --- D_CAN
                D_MCU --- D_Motors
                D_MCU --- D_Encoders
                D_MCU --- D_IMU
            end
            
            subgraph "ArmHat - Priority 4"
                A_MCU[Microcontroller]
                A_CAN[TJA1050]
                A_Joints[Joint Controllers<br/>0x410-0x414]
                A_Feedback[Position Feedback<br/>0x420-0x42F]
                A_Force[Force/Torque<br/>0x430-0x43F]
                
                A_MCU --- A_CAN
                A_MCU --- A_Joints
                A_MCU --- A_Feedback
                A_MCU --- A_Force
            end
            
            subgraph "BPS - Priority 3"
                B_MCU[Microcontroller]
                B_CAN[TJA1050]
                B_Voltage[Voltage Monitor<br/>0x310-0x31F]
                B_Current[Current Monitor<br/>0x320-0x32F]
                B_Temp[Temperature<br/>0x330-0x33F]
                B_Emergency[Emergency Stop<br/>0x3F0-0x3FF]
                
                B_MCU --- B_CAN
                B_MCU --- B_Voltage
                B_MCU --- B_Current
                B_MCU --- B_Temp
                B_MCU --- B_Emergency
            end
            
            subgraph "SenseHat - Priority 6"
                S_MCU[Microcontroller]
                S_CAN[TJA1050]
                S_IMU[IMU Sensors<br/>0x610-0x61F]
                S_Env[Environmental<br/>0x620-0x64F]
                S_GPS[GPS Module<br/>0x650-0x65F]
                
                S_MCU --- S_CAN
                S_MCU --- S_IMU
                S_MCU --- S_Env
                S_MCU --- S_GPS
            end
            
            subgraph "SciHat - Priority 5"
                SC_MCU[Microcontroller]
                SC_CAN[TJA1050]
                SC_Sample[Sample Collection<br/>0x510-0x51F]
                SC_Spec[Spectrometer<br/>0x520-0x52F]
                SC_Cam[Cameras<br/>0x530-0x53F]
                
                SC_MCU --- SC_CAN
                SC_MCU --- SC_Sample
                SC_MCU --- SC_Spec
                SC_MCU --- SC_Cam
            end
        end
        
        subgraph "External Interfaces"
            BaseStation[Base Station<br/>WiFi/SSH]
            Operator[Operator Controller<br/>R2 Button/Joystick]
            Battery[Battery Pack]
        end
    end
    
    %% Connections
    CAN1 --- CANBus
    CAN2 --- CANBus
    D_CAN --- CANBus
    A_CAN --- CANBus
    B_CAN --- CANBus
    S_CAN --- CANBus
    SC_CAN --- CANBus
    
    Power --- Battery
    Power --- D_MCU
    Power --- A_MCU
    Power --- B_MCU
    Power --- S_MCU
    Power --- SC_MCU
    
    Jetson -.->|WiFi| BaseStation
    BaseStation -.->|Commands| Operator
    
    %% Styling
    classDef backplane fill:#e1f5fe
    classDef hat fill:#f3e5f5
    classDef external fill:#e8f5e8
    classDef canbus fill:#fff3e0
    
    class Jetson,CAN1,CAN2,Power backplane
    class D_MCU,A_MCU,B_MCU,S_MCU,SC_MCU,D_CAN,A_CAN,B_CAN,S_CAN,SC_CAN hat
    class BaseStation,Operator,Battery external
    class CANBus canbus
```

### Hardware Interface Framework
The core of SIRI-Elec is a **Common Hardware Interface** that standardizes communication between:
- **Backplane**: Central CAN network hub with compute module
- **HAT Modules**: Distributed control subsystems that plug into the backplane

### Physical Architecture

```mermaid
graph LR
    subgraph "Backplane PCB"
        subgraph "Compute Section"
            Jetson[Jetson Orin Nano<br/>ARM Cortex-A78AE<br/>2048 CUDA Cores]
            Memory[8GB LPDDR5<br/>128GB eUFS]
        end
        
        subgraph "CAN Interface"
            CAN_Primary[Primary CAN<br/>TJA1050]
            CAN_Backup[Backup CAN<br/>TJA1050]
            CAN_Term[120Ω Termination]
        end
        
        subgraph "HAT Connectors"
            Slot1[HAT Slot 1<br/>40-pin GPIO]
            Slot2[HAT Slot 2<br/>40-pin GPIO]
            Slot3[HAT Slot 3<br/>40-pin GPIO]
            Slot4[HAT Slot 4<br/>40-pin GPIO]
            Slot5[HAT Slot 5<br/>40-pin GPIO]
            Slot6[HAT Slot 6<br/>40-pin GPIO]
        end
        
        subgraph "Power Management"
            PowerReg[5V/3.3V Regulators]
            PowerDist[Power Distribution]
            PowerMon[Power Monitoring]
        end
    end
    
    subgraph "HAT Cards (Pluggable)"
        DriveCard[DriveHat<br/>━━━━━━━━━━<br/>Motor Controllers<br/>Encoder Interfaces<br/>IMU Module]
        
        ArmCard[ArmHat<br/>━━━━━━━━━━<br/>Servo Controllers<br/>Position Feedback<br/>Force Sensors]
        
        BPSCard[BPS Hat<br/>━━━━━━━━━━<br/>Battery Monitor<br/>Current Sensing<br/>Emergency Stop]
        
        SenseCard[SenseHat<br/>━━━━━━━━━━<br/>Environmental<br/>IMU/GPS<br/>Temperature]
        
        SciCard[SciHat<br/>━━━━━━━━━━<br/>Sample Collection<br/>Spectrometer<br/>Camera Interface]
        
        TemplateCard[TemplateHat<br/>━━━━━━━━━━<br/>Base Template<br/>CAN Interface<br/>State Machine]
    end
    
    %% Physical connections
    Slot1 ---|40-pin| DriveCard
    Slot2 ---|40-pin| ArmCard
    Slot3 ---|40-pin| BPSCard
    Slot4 ---|40-pin| SenseCard
    Slot5 ---|40-pin| SciCard
    Slot6 ---|40-pin| TemplateCard
    
    %% CAN bus connections
    CAN_Primary --- Slot1
    CAN_Primary --- Slot2
    CAN_Primary --- Slot3
    CAN_Primary --- Slot4
    CAN_Primary --- Slot5
    CAN_Primary --- Slot6
    
    %% Power connections
    PowerDist --- Slot1
    PowerDist --- Slot2
    PowerDist --- Slot3
    PowerDist --- Slot4
    PowerDist --- Slot5
    PowerDist --- Slot6
    
    %% Styling
    classDef backplane fill:#e3f2fd,stroke:#1976d2,stroke-width:3px
    classDef hat fill:#f1f8e9,stroke:#388e3c,stroke-width:2px
    classDef compute fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    classDef power fill:#fce4ec,stroke:#c2185b,stroke-width:2px
    
    class Jetson,Memory compute
    class CAN_Primary,CAN_Backup,CAN_Term,Slot1,Slot2,Slot3,Slot4,Slot5,Slot6 backplane
    class PowerReg,PowerDist,PowerMon power
    class DriveCard,ArmCard,BPSCard,SenseCard,SciCard,TemplateCard hat
```

### Key Components

#### Backplane
- **Central compute node**: Jetson Orin Nano with ARM Cortex-A78AE
- **Dual CAN transceivers**: TJA1050 with redundancy for fault tolerance
- **Multiple HAT slots**: 6x 40-pin GPIO connectors for expansion
- **Power distribution**: Regulated 5V/3.3V supply to all HATs
- **Broadcast communication**: Priority-based CAN arbitration

#### HAT Modules
- **TemplateHAT**: Base template for creating new HAT modules
- **ArmHAT**: Robotic arm control and actuation
- **DriveHat**: Motor control and odometry
- **BPS**: Battery protection and power management
- **SciHat**: Scientific payload control
- **SenseHat**: Environmental and onboard sensing

### Modular Design Benefits
- **Hot-swappable**: HATs can be replaced without system shutdown
- **Scalable**: Add new functionality by creating new HAT modules
- **Fault isolation**: Individual HAT failures don't affect other systems
- **Standardized interface**: All HATs use same 40-pin connector and CAN protocol
- **Development friendly**: Test individual HATs independently

## Network Architecture

### CAN Network Specifications
- **Speed**: Up to 1 Mbps
- **Topology**: Multi-drop bus supporting 32+ nodes
- **Protocol**: Extended CAN IDs with priority-based arbitration
- **Reliability**: Built-in error checking and collision handling

### Priority System
1. **Jetson** (Priority 1) - Main compute and coordination
2. **DriveHat** (Priority 2) - Low-latency motor control
3. **BPS** (Priority 3) - Critical battery monitoring
4. **ArmHat** (Priority 4) - Robotic arm control
5. **SciHat** (Priority 5) - Scientific operations
6. **SenseHat** (Priority 6) - Environmental sensing

## Project Structure

```
SIRI-Elec/
├── TemplateHAT/          # Base HAT template (submodule)
├── ArmHAT/               # Arm control HAT (submodule)
├── HwInf/                # Hardware interface implementations
├── hw_inf_mappings.md    # Address and component mappings
├── docs.md               # Technical API documentation
└── README.md             # This overview
```

## Getting Started

1. **Clone with submodules**:
   ```bash
   git clone --recursive https://github.com/your-org/SIRI-Elec.git
   ```

2. **Initialize submodules** (if already cloned):
   ```bash
   git submodule update --init --recursive
   ```

3. **Review hardware mappings**: See `hw_inf_mappings.md` for CAN addresses and component assignments

4. **Check technical documentation**: See `docs.md` for detailed API specifications

## Development

### Creating New HATs
1. Use `TemplateHAT/` as starting point
2. Assign addresses from `hw_inf_mappings.md`
3. Implement required interface methods
4. Test on CAN network before integration

### Testing
CAN network testing tools are available in `HwInf/Testing/`

## State Machine Architecture

All HATs implement a standardized state machine for safety and operational control:

```mermaid
stateDiagram-v2
    [*] --> POWER_OFF
    POWER_OFF --> POWER_IDLE : Power On
    POWER_IDLE --> DISARMED : Base Station Command
    POWER_IDLE --> UNLOCKED : Base Station Command
    DISARMED --> LOCKED : Timeout/Safety
    UNLOCKED --> POWER_ARMED : Operator Command (R2)
    POWER_ARMED --> UNLOCKED : Release Command
    LOCKED --> DISARMED : Unlock Command
    POWER_ARMED --> EMERGENCY_STOP : Emergency
    EMERGENCY_STOP --> POWER_IDLE : Reset
```

### State Definitions
- **POWER_OFF**: System completely powered down
- **POWER_IDLE**: Default state after boot, commands ignored for safety
- **DISARMED**: Software active, hardware locked, sensors operational
- **UNLOCKED**: Ready for activation, motors unlocked, awaiting arm command
- **POWER_ARMED**: Fully operational mission state, all commands accepted
- **LOCKED**: Safety lock engaged, all commands blocked
- **EMERGENCY_STOP**: Emergency halt, requires manual reset

## Features

- **Modular Design**: Easy to add/remove HAT modules
- **Fault Tolerant**: Redundant CAN transceivers and error handling
- **Scalable**: Support for additional HATs without network redesign
- **Real-time**: Priority-based message arbitration for time-critical operations
- **Standardized**: Common interface across all HAT modules
- **Safety-First**: Multi-layer state machine with fail-safe mechanisms
- **Mission-Ready**: Designed for rover operations with operator controls

## Documentation

- **Overview**: This README
- **Architecture**: `architecture.md` - Detailed system diagrams and design
- **Technical APIs**: `docs.md`
- **Hardware Mappings**: `hw_inf_mappings.md`
- **State Machine**: `state_machine.md`
- **Individual HATs**: See respective submodule documentation
