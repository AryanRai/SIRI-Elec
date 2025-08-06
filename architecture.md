# SIRI-Elec Architecture Documentation

## System Architecture Overview

The SIRI-Elec system implements a modular, distributed architecture centered around a CAN network backplane. This design provides scalability, fault tolerance, and ease of development while maintaining real-time performance for rover operations.

## Physical Layer Architecture

### Backplane Design

**CAN Transceiver Selection:**
- **Jetson Backplane**: SN65HVD230 (3.3V compatible with Jetson GPIO)
- **HAT Modules (Teensy)**: TJA1050 (5V tolerant, robust for distributed modules)

```mermaid
graph TB
    subgraph "Backplane PCB Layout"
        subgraph "Top Section - Compute"
            Jetson[Jetson Orin Nano<br/>40-pin GPIO Header<br/>USB-C Power<br/>Ethernet/WiFi]
            SODIMM[SO-DIMM Connector<br/>8GB LPDDR5]
            eMMC[eUFS Storage<br/>128GB]
        end
        
        subgraph "Middle Section - Communication"
            CAN_IC1[SN65HVD230 #1<br/>Primary CAN]
            CAN_IC2[SN65HVD230 #2<br/>Backup CAN]
            CAN_Conn[CAN Bus Connector<br/>CANH/CANL/GND]
            Term_R[120Ω Termination<br/>Jumper Selectable]
        end
        
        subgraph "Bottom Section - HAT Interfaces"
            HAT1[HAT Slot 1<br/>40-pin Female<br/>DriveHat]
            HAT2[HAT Slot 2<br/>40-pin Female<br/>ArmHat]
            HAT3[HAT Slot 3<br/>40-pin Female<br/>BPS]
            HAT4[HAT Slot 4<br/>40-pin Female<br/>SenseHat]
            HAT5[HAT Slot 5<br/>40-pin Female<br/>SciHat]
            HAT6[HAT Slot 6<br/>40-pin Female<br/>Expansion]
        end
        
        subgraph "Power Section"
            PowerIn[24V Input<br/>XT60 Connector]
            Buck1[5V Buck Converter<br/>10A Capacity]
            Buck2[3.3V Buck Converter<br/>5A Capacity]
            PowerLED[Power Status LEDs]
        end
    end
    
    Jetson --- CAN_IC1
    Jetson --- CAN_IC2
    CAN_IC1 --- CAN_Conn
    CAN_IC2 --- CAN_Conn
    
    PowerIn --- Buck1
    Buck1 --- Buck2
    Buck1 --- Jetson
    Buck1 --- HAT1
    Buck1 --- HAT2
    Buck1 --- HAT3
    Buck1 --- HAT4
    Buck1 --- HAT5
    Buck1 --- HAT6
    
    Buck2 --- CAN_IC1
    Buck2 --- CAN_IC2
    
    CAN_Conn --- HAT1
    CAN_Conn --- HAT2
    CAN_Conn --- HAT3
    CAN_Conn --- HAT4
    CAN_Conn --- HAT5
    CAN_Conn --- HAT6
```

### HAT Module Structure

```mermaid
graph LR
    subgraph "Generic HAT PCB"
        subgraph "Interface Section"
            Connector[40-pin Male Header<br/>GPIO/Power/CAN]
            MCU[Microcontroller<br/>STM32/ESP32]
            CAN_Trans[TJA1050<br/>CAN Transceiver]
        end
        
        subgraph "Control Section"
            StateMachine[State Machine<br/>Implementation]
            MessageHandler[CAN Message<br/>Handler]
            ComponentCtrl[Component<br/>Controllers]
        end
        
        subgraph "Hardware Section"
            Actuators[Motors/Servos<br/>Actuators]
            Sensors[Sensors<br/>Feedback]
            PowerMgmt[Local Power<br/>Management]
        end
        
        subgraph "Status Section"
            StatusLEDs[Status LEDs<br/>State/Error]
            TestPoints[Test Points<br/>Debug Access]
        end
    end
    
    Connector --- MCU
    MCU --- CAN_Trans
    MCU --- StateMachine
    MCU --- MessageHandler
    MCU --- ComponentCtrl
    ComponentCtrl --- Actuators
    ComponentCtrl --- Sensors
    MCU --- PowerMgmt
    MCU --- StatusLEDs
```

## Network Architecture

### CAN Network Topology

```mermaid
graph TB
    subgraph "CAN Network - 1 Mbps"
        subgraph "Priority 1 - Jetson Master"
            J[Jetson Orin Nano<br/>Node ID: 0x01<br/>Address: 0x100-0x1FF]
        end
        
        subgraph "Priority 2 - DriveHat"
            D[DriveHat MCU<br/>Node ID: 0x02<br/>Address: 0x200-0x2FF]
            D_Drive[4x Drive Motors<br/>0x210-0x213]
            D_Steer[4x Steer Motors<br/>0x214-0x217]
            D_DriveEnc[4x Drive Encoders<br/>0x220-0x223]
            D_SteerEnc[4x Steer Encoders<br/>0x224-0x227]
            D_IMU[IMU/Odometry<br/>0x230-0x23F]
        end
        
        subgraph "Priority 3 - BPS"
            B[BPS MCU<br/>Node ID: 0x03<br/>Address: 0x300-0x3FF]
            B_Voltage[Voltage Monitor<br/>0x310-0x31F]
            B_Current[Current Monitor<br/>0x320-0x32F]
            B_Temp[Temperature<br/>0x330-0x33F]
            B_Emergency[Emergency<br/>0x3F0-0x3FF]
        end
        
        subgraph "Priority 4 - ArmHat"
            A[ArmHat MCU<br/>Node ID: 0x04<br/>Address: 0x400-0x4FF]
            A_Joints[5x Joint Controllers<br/>0x410-0x414]
            A_Feedback[Position Feedback<br/>0x420-0x42F]
            A_Force[Force/Torque<br/>0x430-0x43F]
        end
        
        subgraph "Priority 5 - SciHat"
            S[SciHat MCU<br/>Node ID: 0x05<br/>Address: 0x500-0x5FF]
            S_Sample[Sample Collection<br/>0x510-0x51F]
            S_Spec[Spectrometer<br/>0x520-0x52F]
            S_Camera[Cameras<br/>0x530-0x53F]
        end
        
        subgraph "Priority 6 - SenseHat"
            SE[SenseHat MCU<br/>Node ID: 0x06<br/>Address: 0x600-0x6FF]
            SE_IMU[IMU Sensors<br/>0x610-0x61F]
            SE_Env[Environmental<br/>0x620-0x64F]
            SE_GPS[GPS Module<br/>0x650-0x65F]
        end
        
        subgraph "CAN Bus Physical"
            CANBus[CAN High/Low + Ground<br/>120Ω Termination<br/>Twisted Pair Cable]
        end
    end
    
    J --- CANBus
    D --- CANBus
    B --- CANBus
    A --- CANBus
    S --- CANBus
    SE --- CANBus
    
    D --- D_Drive
    D --- D_Steer
    D --- D_DriveEnc
    D --- D_SteerEnc
    D --- D_IMU
    
    B --- B_Voltage
    B --- B_Current
    B --- B_Temp
    B --- B_Emergency
    
    A --- A_Joints
    A --- A_Feedback
    A --- A_Force
    
    S --- S_Sample
    S --- S_Spec
    S --- S_Camera
    
    SE --- SE_IMU
    SE --- SE_Env
    SE --- SE_GPS
```

## Message Flow Architecture

### Command and Control Flow

```mermaid
sequenceDiagram
    participant BS as Base Station
    participant J as Jetson
    participant D as DriveHat
    participant A as ArmHat
    participant B as BPS
    participant S as SenseHat
    
    Note over BS,S: System Startup Sequence
    BS->>J: SSH Connection
    J->>J: Boot Complete
    J->>D: Heartbeat Request
    J->>A: Heartbeat Request
    J->>B: Heartbeat Request
    J->>S: Heartbeat Request
    
    D-->>J: Heartbeat Response (STATE_LOCKED)
    A-->>J: Heartbeat Response (STATE_LOCKED)
    B-->>J: Heartbeat Response (STATE_LOCKED)
    S-->>J: Heartbeat Response (STATE_LOCKED)
    
    Note over BS,S: Mission Activation
    BS->>J: Launch ROS2 Command
    J->>D: State Transition (UNLOCK)
    J->>A: State Transition (UNLOCK)
    J->>S: State Transition (UNLOCK)
    
    D-->>J: State Confirm (UNLOCKED)
    A-->>J: State Confirm (UNLOCKED)
    S-->>J: State Confirm (UNLOCKED)
    
    Note over BS,S: Operator Control
    BS->>J: Operator R2 Button Press
    J->>D: State Transition (ARM)
    J->>A: State Transition (ARM)
    J->>S: State Transition (ARM)
    
    D-->>J: State Confirm (POWER_ARMED)
    A-->>J: State Confirm (POWER_ARMED)
    S-->>J: State Confirm (POWER_ARMED)
    
    Note over BS,S: Active Mission
    BS->>J: Movement Command
    J->>D: Motor Control (Forward)
    D->>D: Execute Movement
    D-->>J: Telemetry (Position/Speed)
    J-->>BS: Status Update
    
    S->>S: Capture Images
    S-->>J: Camera Data
    J-->>BS: Image Stream
    
    Note over BS,S: Emergency Scenario
    B->>B: Battery Critical Detected
    B->>J: Emergency Alert (Priority 1)
    J->>D: Emergency Stop
    J->>A: Emergency Stop
    J->>S: Emergency Stop
    
    D-->>J: Emergency Confirm
    A-->>J: Emergency Confirm
    S-->>J: Emergency Confirm
```

## State Machine Integration

### System-Wide State Coordination

```mermaid
stateDiagram-v2
    [*] --> SystemOff
    
    state SystemOff {
        [*] --> AllHATsOff
        AllHATsOff : All HATs in POWER_OFF
        AllHATsOff : No CAN traffic
        AllHATsOff : No power consumption
    }
    
    SystemOff --> SystemDisarmed : Power Button
    
    state SystemDisarmed {
        [*] --> JetsonBoot
        JetsonBoot --> HATsDisarmed : Boot Complete
        HATsDisarmed : All HATs in DISARMED (brief)
        HATsDisarmed : Periodic Jetson pings
        HATsDisarmed : Hardware disabled
        HATsDisarmed : Auto-transition to LOCKED
    }
    
    SystemDisarmed --> SystemLocked : Auto Transition
    
    state SystemLocked {
        [*] --> HATsLocked
        HATsLocked : All HATs in LOCKED (default)
        HATsLocked : Periodic Jetson pings
        HATsLocked : Hardware locked
        HATsLocked : All commands blocked
    }
    
    SystemLocked --> SystemReady : Base Station Unlock
    
    state SystemReady {
        [*] --> HATsUnlocked
        HATsUnlocked : HATs in UNLOCKED state
        HATsUnlocked : Motors enabled
        HATsUnlocked : Sensors active
        HATsUnlocked : Limited commands accepted
    }
    
    SystemReady --> SystemArmed : Operator R2 Press
    
    state SystemArmed {
        [*] --> FullyOperational
        FullyOperational : All HATs in POWER_ARMED
        FullyOperational : Complete functionality
        FullyOperational : Mission execution
        FullyOperational : Real-time control
    }
    
    SystemArmed --> SystemReady : Operator Release
    SystemReady --> SystemLocked : Safety Command
    SystemReady --> SystemDisarmed : Jetson Ping Timeout
    SystemArmed --> SystemDisarmed : Jetson Ping Timeout
    SystemLocked --> SystemDisarmed : Jetson Ping Timeout
    SystemArmed --> EmergencyState : Emergency Trigger
    SystemReady --> EmergencyState : Emergency Trigger
    SystemLocked --> EmergencyState : Emergency Trigger
    
    state EmergencyState {
        [*] --> AllHATsEmergency
        AllHATsEmergency : All HATs in EMERGENCY_STOP
        AllHATsEmergency : All operations halted
        AllHATsEmergency : Safety protocols active
        AllHATsEmergency : Manual reset required
    }
    
    EmergencyState --> SystemLocked : Manual Reset
    EmergencyState --> SystemOff : Critical Shutdown
```

## Power Architecture

### Power Distribution System

```mermaid
graph TB
    subgraph "Power Input"
        Battery[24V Li-Ion Battery Pack<br/>20Ah Capacity<br/>BMS Integrated]
        MainSwitch[Main Power Switch<br/>Emergency Cutoff]
        Fuse[30A Fuse<br/>Overcurrent Protection]
    end
    
    subgraph "Backplane Power"
        PrimaryReg[Primary 5V Regulator<br/>Buck Converter<br/>10A Capacity]
        SecondaryReg[Secondary 3.3V Regulator<br/>Buck Converter<br/>5A Capacity]
        PowerMon[Power Monitoring<br/>Voltage/Current Sensing]
    end
    
    subgraph "HAT Power Distribution"
        HAT1_5V[HAT1 - 5V Rail<br/>2A Max]
        HAT2_5V[HAT2 - 5V Rail<br/>2A Max]
        HAT3_5V[HAT3 - 5V Rail<br/>2A Max]
        HAT4_5V[HAT4 - 5V Rail<br/>1A Max]
        HAT5_5V[HAT5 - 5V Rail<br/>1A Max]
        HAT6_5V[HAT6 - 5V Rail<br/>1A Max]
        
        Logic_3V3[Logic 3.3V Rail<br/>Shared - 3A Max]
    end
    
    subgraph "Protection Circuits"
        OVP[Overvoltage Protection<br/>28V Cutoff]
        UVP[Undervoltage Protection<br/>18V Cutoff]
        OCP[Overcurrent Protection<br/>Per-HAT Limiting]
        ThermalProt[Thermal Protection<br/>85°C Cutoff]
    end
    
    Battery --- MainSwitch
    MainSwitch --- Fuse
    Fuse --- OVP
    OVP --- UVP
    UVP --- PrimaryReg
    
    PrimaryReg --- SecondaryReg
    PrimaryReg --- PowerMon
    
    PrimaryReg --- HAT1_5V
    PrimaryReg --- HAT2_5V
    PrimaryReg --- HAT3_5V
    PrimaryReg --- HAT4_5V
    PrimaryReg --- HAT5_5V
    PrimaryReg --- HAT6_5V
    
    SecondaryReg --- Logic_3V3
    
    PowerMon --- OCP
    PrimaryReg --- ThermalProt
    SecondaryReg --- ThermalProt
```

## Development and Testing Architecture

### Development Workflow

```mermaid
graph LR
    subgraph "Development Environment"
        IDE[Kiro IDE<br/>Code Development]
        Simulator[HAT Simulator<br/>Virtual Testing]
        CANAnalyzer[CAN Analyzer<br/>Protocol Testing]
    end
    
    subgraph "Hardware Testing"
        Breadboard[Breadboard Prototype<br/>Initial Testing]
        TestHAT[Test HAT PCB<br/>Hardware Validation]
        FullSystem[Complete System<br/>Integration Testing]
    end
    
    subgraph "Software Testing"
        UnitTests[Unit Tests<br/>Component Level]
        IntegrationTests[Integration Tests<br/>HAT Level]
        SystemTests[System Tests<br/>Full Network]
    end
    
    IDE --> Simulator
    Simulator --> UnitTests
    UnitTests --> Breadboard
    Breadboard --> TestHAT
    TestHAT --> IntegrationTests
    IntegrationTests --> FullSystem
    FullSystem --> SystemTests
    
    CANAnalyzer --> TestHAT
    CANAnalyzer --> FullSystem
    CANAnalyzer --> SystemTests
```

## Scalability and Expansion

### Future Expansion Capabilities

```mermaid
graph TB
    subgraph "Current System"
        CurrentHATs[6 HAT Slots<br/>DriveHat, ArmHat, BPS<br/>SenseHat, SciHat, Template]
    end
    
    subgraph "Expansion Options"
        AdditionalBackplane[Additional Backplane<br/>CAN Bridge Connection]
        SpecializedHATs[Specialized HATs<br/>Custom Applications]
        RedundantHATs[Redundant HATs<br/>Fault Tolerance]
    end
    
    subgraph "Future HAT Concepts"
        CommHAT[Communication HAT<br/>Long-range Radio<br/>Satellite Uplink]
        VisionHAT[Vision Processing HAT<br/>AI Accelerator<br/>Computer Vision]
        NavigationHAT[Navigation HAT<br/>Advanced GPS<br/>INS Integration]
        PayloadHAT[Mission Payload HAT<br/>Scientific Instruments<br/>Sample Analysis]
    end
    
    CurrentHATs --> AdditionalBackplane
    AdditionalBackplane --> SpecializedHATs
    AdditionalBackplane --> RedundantHATs
    
    SpecializedHATs --> CommHAT
    SpecializedHATs --> VisionHAT
    SpecializedHATs --> NavigationHAT
    SpecializedHATs --> PayloadHAT
```

This architecture provides a solid foundation for the SIRI rover's electrical systems while maintaining flexibility for future enhancements and mission-specific modifications.