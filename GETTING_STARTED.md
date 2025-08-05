# SIRI-Elec Getting Started Guide

Welcome to SIRI-Elec! This guide will help you set up your development environment and start building HAT modules for the SIRI rover. We'll cover multiple development approaches and modern AI tools to accelerate your development.

## 🚀 Quick Start Overview

SIRI-Elec uses **Teensy 4.1** microcontrollers with **Arduino IDE** for HAT development. You have several options for your development environment:

1. **Arduino IDE** (Traditional, beginner-friendly)
2. **VS Code + Arduino Extension** (Modern, feature-rich)
3. **VS Code + Arduino CLI** (Advanced, command-line focused)

## 📋 Prerequisites

### Hardware Requirements
- **Teensy 4.1** development board
- **USB-C cable** for programming
- **CAN transceiver breakout** (TJA1050) for testing
- **Breadboard and jumper wires** for prototyping

### Software Requirements
- **Windows 10/11**, **macOS**, or **Linux**
- **Git** for version control
- **Arduino IDE 2.x** or **VS Code**
- **Teensyduino** add-on

## 🛠️ Development Environment Setup

### Option 1: Arduino IDE (Recommended for Beginners)

#### Step 1: Install Arduino IDE
1. Download **Arduino IDE 2.x** from https://www.arduino.cc/en/software
2. Install following the platform-specific instructions
3. Launch Arduino IDE to verify installation

#### Step 2: Install Teensyduino
1. Download **Teensyduino** from https://www.pjrc.com/teensy/teensyduino.html
2. **Important**: Choose the version compatible with your Arduino IDE
3. Run the installer and follow the setup wizard
4. Select **Teensy 4.1** when prompted for board support

#### Step 3: Configure Arduino IDE
1. Open Arduino IDE
2. Go to **Tools > Board > Teensyduino > Teensy 4.1**
3. Set **Tools > USB Type > Serial**
4. Set **Tools > CPU Speed > 600 MHz**
5. Set **Tools > Optimize > Faster**

#### Step 4: Install Required Libraries
1. Go to **Tools > Manage Libraries**
2. Search and install:
   - **FlexCAN_T4** by tonton81
   - **ArduinoJson** by Benoit Blanchon
3. Wait for installation to complete

### Option 2: VS Code + Arduino Extension (Recommended for Most Users)

#### Step 1: Install VS Code
1. Download **Visual Studio Code** from https://code.visualstudio.com/
2. Install following platform instructions
3. Launch VS Code

#### Step 2: Install Arduino Extension
1. Open **Extensions** (Ctrl+Shift+X)
2. Search for **"Arduino"** by Microsoft
3. Click **Install**
4. Restart VS Code

#### Step 3: Install Arduino IDE (Backend)
1. Install **Arduino IDE 2.x** (even if you won't use the GUI)
2. Install **Teensyduino** as described in Option 1
3. VS Code will use Arduino IDE as the backend

#### Step 4: Configure VS Code Arduino
1. Open **Command Palette** (Ctrl+Shift+P)
2. Type **"Arduino: Initialize"**
3. Select your Arduino IDE installation path
4. Configure board: **Teensy 4.1**

#### Step 5: Install Helpful Extensions
```
Recommended VS Code Extensions:
- Arduino (Microsoft) - Arduino development
- C/C++ (Microsoft) - IntelliSense and debugging
- GitLens (GitKraken) - Enhanced Git integration
- Error Lens (Alexander) - Inline error display
- Bracket Pair Colorizer - Visual bracket matching
```

### Option 3: VS Code + Arduino CLI (Advanced Users)

#### Step 1: Install Arduino CLI
```bash
# Windows (using winget)
winget install ArduinoSA.CLI

# macOS (using Homebrew)
brew install arduino-cli

# Linux (using curl)
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```

#### Step 2: Configure Arduino CLI
```bash
# Initialize configuration
arduino-cli config init

# Update package index
arduino-cli core update-index

# Install Teensy core (requires manual Teensyduino installation first)
arduino-cli core install teensy:avr
```

#### Step 3: VS Code Configuration
1. Install **Arduino** extension
2. Set Arduino CLI path in settings
3. Configure tasks.json for building and uploading



## 📁 Project Setup

### Clone the SIRI-Elec Repository
```bash
# Clone with submodules
git clone --recursive https://github.com/your-org/SIRI-Elec.git

# Navigate to project
cd SIRI-Elec

# If already cloned, initialize submodules
git submodule update --init --recursive
```

### Open TemplateHAT Project

#### In Arduino IDE:
1. Navigate to `TemplateHAT/firmware/TemplateHAT_Firmware/`
2. Open `TemplateHAT_Firmware.ino`
3. All tabs should load automatically

#### In VS Code:
1. **File > Open Folder**
2. Select `TemplateHAT/firmware/TemplateHAT_Firmware/`
3. VS Code will detect the Arduino project
4. Use **Ctrl+Shift+P > Arduino: Verify** to compile

## 🤖 AI Development Tools

Modern AI tools can significantly accelerate your HAT development. Here are our recommendations based on real-world usage:

### AI-Powered IDEs

#### Cursor (Highly Recommended for Prototyping)
- **Status**: Previously used and loved, but current usage has strict limitations
- **Best for**: Rapid prototyping and quick iterations
- **Strengths**: Excellent for getting ideas into code quickly
- **Note**: Check current availability and usage policies

#### Kiro (Excellent but Limited Access)
- **Status**: Very nice IDE but currently on waitlist
- **Best for**: Comprehensive development workflow
- **Strengths**: Integrated AI assistance throughout development process
- **Note**: Join waitlist if interested in advanced AI-integrated development

#### Windsurf (Solid Alternative)
- **Status**: Available and functional
- **Best for**: General development with AI assistance
- **Strengths**: Reliable AI integration without strict limitations
- **Note**: Good middle-ground option for consistent development

#### VS Code with Copilot (Decent and Accessible)
- **Status**: Widely available and stable
- **Best for**: Traditional development with AI assistance
- **Strengths**: Good at code completion and suggestions
- **Limitations**: Not as quick for prototyping as specialized AI IDEs
- **Setup**: Install GitHub Copilot extension, requires GitHub subscription

### AI Models for Development

#### Claude 4 (Best for Large Codebases)
- **Best for**: Complex, large-scale problems and architecture decisions
- **Strengths**: Handles generic, broad questions excellently
- **Use cases**: System design, large refactoring, complex debugging
- **Limitations**: Very expensive, usage runs out quickly
- **When to use**: Major architectural decisions, complex multi-file problems

#### Claude 3.5 Sonnet (Smart but Specific)
- **Best for**: Specific, well-defined problems
- **Strengths**: Very intelligent for targeted questions
- **Limitations**: Not ideal for large, generic problems
- **Use cases**: Specific function implementation, targeted bug fixes
- **When to use**: When you have a clear, specific question or task

#### Claude 3.7 (Balanced Option)
- **Best for**: Medium-complexity problems
- **Strengths**: Good balance between capability and cost
- **Use cases**: General development questions, moderate complexity tasks
- **When to use**: Most day-to-day development questions

#### ChatGPT (Great for Learning)
- **Best for**: Explanations and learning concepts
- **Strengths**: Excellent at breaking down complex topics
- **Limitations**: Not as good at agentic/autonomous tasks
- **Use cases**: Understanding CAN protocol, learning Teensy programming
- **When to use**: When you need to understand how something works

#### Grok (Good for Single Files)
- **Best for**: Single-file code problems
- **Strengths**: Focused, direct assistance for individual files
- **Use cases**: Debugging a specific .cpp file, optimizing a single function
- **When to use**: Working on isolated code problems

#### Gemini (Experimental)
- **Status**: Mixed results in our experience
- **Recommendation**: Feel free to explore and experiment
- **Note**: Your mileage may vary - some developers find it useful

### Practical AI Usage Strategy

#### For Different Development Phases:

**Initial Prototyping:**
- Use **Cursor** or **Windsurf** for rapid iteration
- Start with **Claude 3.5** for specific implementation questions

**Architecture Design:**
- Use **Claude 4** for system-level decisions (budget permitting)
- Fall back to **Claude 3.7** for medium-complexity architecture

**Daily Development:**
- **VS Code + Copilot** for code completion
- **ChatGPT** for understanding concepts
- **Grok** for single-file debugging

**Learning and Documentation:**
- **ChatGPT** for explanations and tutorials
- **Claude 3.5** for specific technical questions

#### Cost Management Tips:
- Start with free/cheaper options (ChatGPT, VS Code Copilot)
- Use expensive models (Claude 4) only for complex problems
- Batch similar questions to maximize expensive model usage
- Use specific models for their strengths (ChatGPT for learning, Grok for single files)

#### Effective Prompting:
```
Good prompts for HAT development:

"Implement a CAN message handler for motor control on Teensy 4.1"
"Review this state machine implementation for safety issues"
"Explain how TJA1050 CAN transceiver works with Teensy"
"Optimize this code for real-time performance on embedded system"
"Debug this FlexCAN_T4 initialization error"
```

## 🔧 Your First Build

### Test the Template
1. **Connect Teensy 4.1** via USB-C
2. **Select correct COM port** in your IDE
3. **Compile and upload** the TemplateHAT firmware
4. **Open Serial Monitor** (115200 baud)
5. **Verify output**: You should see initialization messages

### Expected Serial Output:
```
=== SIRI TemplateHAT Starting ===
HAT Name: TemplateHAT
Version: 1.0.0
Node ID: 0x7
ERROR: CAN interface initialization failed
ERROR: State machine initialization failed
=== Initialization Complete ===
```

*Note: Errors are expected without CAN hardware connected*

## 🎯 Development Workflow

### 1. Create Your HAT
```bash
# Copy TemplateHAT as starting point
cp -r TemplateHAT MyCustomHAT

# Update configuration
# Edit MyCustomHAT/firmware/TemplateHAT_Firmware/include/hat_config.h
```

### 2. Customize Configuration
```cpp
// In include/hat_config.h
#define HAT_NAME "MyCustomHAT"
#define HAT_NODE_ID 0x08  // Get from hw_inf_mappings.md
#define HAT_BASE_ADDRESS 0x800
```

### 3. Implement Your Logic
```cpp
// Add your components in include/hardware_map.h
#define PIN_SERVO_1 6
#define PIN_SENSOR_1 14

// Implement in src/component_ctrl.cpp
bool ComponentController::initialize() {
    // Your initialization code
    pinMode(PIN_SERVO_1, OUTPUT);
    pinMode(PIN_SENSOR_1, INPUT);
    return true;
}
```

### 4. Leverage AI Tools
See the **AI Development Tools** section below for recommended tools and usage strategies.

## 🧪 Testing and Debugging

### Serial Debugging
```cpp
// Use debug macros in your code
#if HAT_DEBUG_ENABLED
Serial.println("Debug: Component initialized");
Serial.print("Sensor value: ");
Serial.println(sensorValue);
#endif
```

### LED Status Indicators
```cpp
// Status LED patterns (built into template)
// Slow blink: POWER_IDLE
// Fast blink: DISARMED  
// Double blink: UNLOCKED
// Solid: POWER_ARMED
// Rapid flash: EMERGENCY_STOP
```

### CAN Network Testing
1. **Use CAN analyzer** or **second Teensy** for testing
2. **Monitor CAN traffic** with provided tools
3. **Test state transitions** via CAN commands
4. **Verify heartbeat** transmission

## 📚 Learning Resources

### SIRI-Elec Documentation
- **[README.md](README.md)** - Project overview
- **[docs.md](docs.md)** - Technical API reference
- **[architecture.md](architecture.md)** - System architecture
- **[state_machine.md](state_machine.md)** - State machine details

### External Resources
- **[Teensy 4.1 Documentation](https://www.pjrc.com/teensy/teensy41.html)**
- **[FlexCAN_T4 Library](https://github.com/tonton81/FlexCAN_T4)**
- **[Arduino Language Reference](https://www.arduino.cc/reference/en/)**
- **[CAN Bus Tutorial](https://www.csselectronics.com/pages/can-bus-simple-intro-tutorial)**



## 🚨 Troubleshooting

### Common Issues

#### "Board not found" Error
```
Solutions:
1. Install Teensyduino correctly
2. Select Teensy 4.1 in board menu
3. Check USB cable connection
4. Press program button on Teensy
```

#### Library Not Found
```
Solutions:
1. Install via Library Manager
2. Check library version compatibility
3. Restart Arduino IDE/VS Code
4. Clear library cache
```

#### CAN Errors
```
Solutions:
1. Check TJA1050 wiring
2. Verify 120Ω termination
3. Confirm baud rate (1 Mbps)
4. Test with CAN analyzer
```

#### VS Code Arduino Issues
```
Solutions:
1. Reload VS Code window
2. Reinstall Arduino extension
3. Check Arduino IDE path in settings
4. Use Command Palette > Arduino: Initialize
```

## 🎉 Next Steps

### After Setup
1. **Build and test** TemplateHAT
2. **Read the documentation** thoroughly
3. **Study existing HAT examples** (ArmHAT, etc.)
4. **Join the development community**
5. **Start building your custom HAT**

### Development Best Practices
- **Use version control** (Git) for all changes
- **Test frequently** on hardware
- **Document your code** thoroughly
- **Follow SIRI-Elec coding standards**
- **Choose appropriate AI tools** based on task complexity
- **Ask for help** when needed

### Community and Support
- **GitHub Issues** for bug reports
- **Discussions** for questions and ideas
- **Pull Requests** for contributions
- **AI tools** for development assistance (see AI section above)

## 🤝 Contributing

We encourage contributions! Here's how to get involved:

1. **Fork the repository**
2. **Create a feature branch**
3. **Develop with AI assistance** (see AI tools section)
4. **Test thoroughly**
5. **Submit a pull request**



---

**Welcome to SIRI-Elec development! 🚀**

*Remember: Choose the right AI tool for the job - use expensive models for complex problems, free tools for learning, and specialized IDEs for rapid prototyping!*