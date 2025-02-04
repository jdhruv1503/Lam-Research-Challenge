# LAM Research Challenge 2024 - Winning Solutions

This repository contains the winning solutions for Rounds 2 and 3 of the 2024 Lam Research Challenge. 

### Round 2: Temperature Control System

### Overview
The Round 2 challenge focused on developing a Peltier effect temperature control system using Arduino-based firmware. The solution implements precise temperature monitoring and control mechanisms.

### Key Features
- Temperature sensing and feedback control
- PID control implementation
- Safety mechanisms and error handling
- Real-time temperature monitoring

### Technical Implementation
The temperature control firmware (`Round 2/Firmware - Temperature Control/firmware.ino`) includes:
- Temperature sensor integration
- PID control loop
- Safety thresholds and limits
- Error detection and handling

## Round 3: Robotic Arm Control & Web Interface

### Overview
Round 3 had us implementing a robotic arm control system with a web-based interface for remote operation.

### Components

#### 1. Robotic Arm Firmware
Located in `Round 3/Firmware - Robotic Arm/firmware.ino`:
- Motor control algorithms
- Position feedback
- Movement coordination
- Safety features

#### 2. Web Interface
Located in `Round 3/Webserver/`:
- Real-time control interface
- Status monitoring
- User authentication
- Data visualization

### Setup Instructions

1. **Temperature Control System (Round 2)**
   - Upload firmware.ino to Arduino board
   - Connect temperature sensors
   - Configure PID parameters as needed

2. **Robotic Arm System (Round 3)**
   - Set up the web server
   - Upload firmware to the robotic arm controller
   - Configure network settings
   - Access web interface for control

## Dependencies

### Hardware
- Arduino-compatible microcontroller
- Temperature sensors
- Robotic arm assembly
- Servo motors
- Network connectivity

### Software
- Arduino IDE
- Web server requirements (see Round 3/Webserver/README.md)
- Required Arduino libraries (list TBD)

## Usage

Detailed usage instructions for each component can be found in their respective directories. For specific setup and configuration details, please refer to:
- Round 2/Firmware - Temperature Control/firmware.ino
- Round 3/Webserver/README.md
- Round 3/Firmware - Robotic Arm/firmware.ino

## Acknowledgments

Special thanks to Lam Research for organizing this challenge and providing the opportunity to develop these solutions! <3
