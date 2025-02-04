# Robot Control Web Interface

## Overview
This is a web-based control interface for an ESP32-based robot, allowing manual positioning, speed/acceleration control, and macro commands.

## Prerequisites
- Python 3.8+
- Flask
- pyserial
- React
- Tailwind CSS

## Backend Setup
1. Install dependencies:
```bash
pip install flask flask-cors pyserial
```

2. Configure serial port in `app.py`:
   - Update `port` and `baudrate` in `SerialController` initialization if needed

## Frontend Setup
1. Install dependencies:
```bash
npx create-react-app robot-control
cd robot-control
npm install tailwindcss lucide-react
```

2. Configure Tailwind CSS (follow Tailwind's setup guide)

## Running the Application
1. Start Flask backend:
```bash
python app.py
```

2. Start React frontend:
```bash
npm start
```

## Features
- Manual XYZ positioning
- Speed and acceleration control
- Macro commands for pickup/dropoff
- Real-time command response display

## Notes
- Ensure ESP32 is connected and configured to match serial settings
- Modify commands as needed to match your specific robot firmware