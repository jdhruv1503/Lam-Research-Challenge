import flask
from flask import Flask, request, jsonify
from flask_cors import CORS
import serial
import threading
import queue
import time

app = Flask(__name__)
CORS(app)

# Serial communication setup
class SerialController:
    def __init__(self, port='/dev/ttyUSB1', baudrate=9600, timeout=2):
        self.serial_port = None
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.response_queue = queue.Queue()
        self.lock = threading.Lock()
        self.connect()

    def connect(self):
        try:
            self.serial_port = serial.Serial(self.port, self.baudrate, timeout=self.timeout)
            print(f"Connected to {self.port}")
        except serial.SerialException as e:
            print(f"Could not open serial port: {e}")
            self.serial_port = None

    def send_command(self, command):
        if not self.serial_port:
            return {"error": "Serial port not connected"}
        
        try:
            with self.lock:
                # Ensure command ends with newline
                if not command.endswith('\n'):
                    command += '\n'
                
                # Clear any existing input buffer
                self.serial_port.reset_input_buffer()
                
                # Send command
                self.serial_port.write(command.encode('utf-8'))
                
                # Wait for and collect responses
                responses = []
                start_time = time.time()
                
                while time.time() - start_time < self.timeout:
                    # Read line with timeout
                    line = self.serial_port.readline().decode('utf-8').strip()
                    
                    if line:
                        responses.append(line)
                        
                        # Check for specific responses
                        if line == 'ok':
                            # Command acknowledged
                            continue
                        elif line == 'ready':
                            # Movement completed
                            break
                
                return {
                    "command": command.strip(), 
                    "responses": responses,
                    "status": "success" if 'ready' in responses else "incomplete"
                }
        except Exception as e:
            return {"error": str(e), "status": "error"}

# Global serial controller
serial_controller = SerialController()

@app.route('/command', methods=['POST'])
def send_command():
    data = request.json
    command = data.get('command', '')
    result = serial_controller.send_command(command)
    return jsonify(result)

@app.route('/serial_command', methods=['POST'])
def send_serial_command():
    data = request.json
    command = data.get('command', '')

    result = serial_controller.send_command(command)
    return jsonify(result)

@app.route('/get_status', methods=['GET'])
def get_status():
    result = serial_controller.send_command('get\n')
    return jsonify(result)

if __name__ == '__main__':
    app.run(debug=True, port=5000)