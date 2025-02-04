#include <Wire.h>
#include <AccelStepper.h>
#include <ESP32Servo.h> // include servo library to use its related functions

#define Servo_PWM 26 // A descriptive name for D6 pin of Arduino to provide PWM signal
Servo myservo;  
int servo_angle = 0;

// Multiplexing multiplier for stepper degrees
#define MULTIPLIER (1600/360)

// Steppers (X, Y, and Z axes)
const int PIN_X_STEP = 33;
const int PIN_X_DIR = 2;

const int PIN_Y_STEP = 27;
const int PIN_Y_DIR = 4;

const int PIN_Z_STEP = 32;
const int PIN_Z_DIR = 0;

AccelStepper stepperX(AccelStepper::DRIVER, PIN_X_STEP, PIN_X_DIR);
AccelStepper stepperY(AccelStepper::DRIVER, PIN_Y_STEP, PIN_Y_DIR);
AccelStepper stepperZ(AccelStepper::DRIVER, PIN_Z_STEP, PIN_Z_DIR);

#define GRABBER_CHANNEL 0 // PCA9685 channel for grabber servo

// Serial buffer
String inputString = "";
bool inputComplete = false;

// Command states
float speedX = 2000.0, speedY = 2000.0, speedZ = 2000.0; // Default speeds
float accelX = 500, accelY = 500, accelZ = 500;         // Default accelerations

void setup() {
  // Debug Serial
  Serial.begin(9600);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);         // Standard 50 Hz servo
  myservo.attach(Servo_PWM, 500, 2400); // Attach servo to pin 26

  // Initialize steppers
  stepperX.setMaxSpeed(speedX);
  stepperX.setAcceleration(accelX);
  stepperY.setMaxSpeed(speedY);
  stepperY.setAcceleration(accelY);
  stepperZ.setMaxSpeed(speedZ);
  stepperZ.setAcceleration(accelZ);

  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);

  myservo.write(0);

  Serial.println("System Initialized");
}

void loop() {
  // Handle serial input
  if (inputComplete) {
    processInput(inputString);
    inputString = "";
    inputComplete = false;
  }

  // Run steppers
  stepperX.run();
  stepperY.run();
  stepperZ.run();

  // Check if all steppers are stopped and notify
  if (stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0 && stepperZ.distanceToGo() == 0) {
    Serial.println("ready\n");
  }
}

// Process serial commands
void processInput(String command) {
  command.trim();
  if (command.startsWith("$")) {
    parseProtocolCommand(command);
  } else {
    handleStandardCommands(command);
  }
}

void handleStandardCommands(String command) {
  if (command.startsWith("set")) {
    handleSetCommand(command);
  } else if (command.startsWith("get")) {
    handleGetCommand();
  } else if (command.startsWith("pickup")) {
    handlePickup();
  } else if (command.startsWith("dropoff")) {
    handleDropoff();
  } else if (command.startsWith("x") || command.startsWith("y") || command.startsWith("z") || command.startsWith("a")) {
    handleMovementCommand(command);
  } else {
    Serial.println("Unknown command");
  }
}

// Protocol command handler
void parseProtocolCommand(String command) {
  int funcnStart = command.indexOf('$') + 1;
  int funcnEnd = command.indexOf('*');
  int actionStart = funcnEnd + 1;
  int actionEnd = command.indexOf('*', actionStart);
  int valueStart = actionEnd + 1;
  int valueEnd = command.indexOf('#');

  String funcn = command.substring(funcnStart, funcnEnd);
  String action = command.substring(actionStart, actionEnd);
  String value = command.substring(valueStart, valueEnd);

  executeProtocolCommand(funcn, action, value);
}

void executeProtocolCommand(String funcn, String action, String value) {
  int intValue = value.toInt(); // Convert value to integer
  int stepValue = intValue * MULTIPLIER;

  if (funcn == "01") { // Base Rotation
    if (action == "01") { // Clockwise
      stepperZ.move(-stepValue);
    } else if (action == "02") { // Anti Clockwise
      stepperZ.move(stepValue);
    }
  } else if (funcn == "02") { // ARM1 Rotation
    if (action == "01") { // Clockwise
      stepperX.move(-stepValue);
    } else if (action == "02") { // Anti Clockwise
      stepperX.move(stepValue);
    }
  } else if (funcn == "03") { // ARM2 Rotation
    if (action == "01") { // Clockwise
      stepperY.move(stepValue);
    } else if (action == "02") { // Anti Clockwise
      stepperY.move(-stepValue);
    }
  } else if (funcn == "04") { // End Eector Rotation
  if (action == "01") { // Clockwise
      int servoAngle = (intValue+servo_angle) % 180;
    servo_angle = servoAngle;
    myservo.write(servoAngle);
    } else if (action == "02") { // Anti Clockwise
      int servoAngle = (-intValue+servo_angle) % 180;
    servo_angle = servoAngle;
    myservo.write(servoAngle);
    }
    
  } else if (funcn == "05") { // End Eector Action
    if (action == "04") { // Pick
      handlePickup();
    } else if (action == "05") { // Drop
      handleDropoff();
    }
  }
}

// Pickup and dropoff handlers
void handlePickup() {
  Serial.println("Pickup command received");
  handleMovementCommand("y +10");
  delay(500);
}

void handleDropoff() {
  Serial.println("Dropoff command received");
  handleMovementCommand("y -10");
  delay(500);
}

// Set command handler
void handleSetCommand(String command) {
  char type[16];
  char axis;
  float value;
  sscanf(command.c_str(), "set %15s %c %f", type, &axis, &value);

  if (String(type) == "speed") {
    if (axis == 'x') stepperX.setMaxSpeed(value);
    if (axis == 'y') stepperY.setMaxSpeed(value);
    if (axis == 'z') stepperZ.setMaxSpeed(value);
  } else if (String(type) == "acceleration") {
    if (axis == 'x') stepperX.setAcceleration(value);
    if (axis == 'y') stepperY.setAcceleration(value);
    if (axis == 'z') stepperZ.setAcceleration(value);
  }

  Serial.printf("Set %s for axis %c to %.2f\n", type, axis, value);
}

// Get command handler
void handleGetCommand() {
  Serial.printf("speed_x %.2f speed_y %.2f speed_z %.2f\n", stepperX.maxSpeed(), stepperY.maxSpeed(), stepperZ.maxSpeed());
}

// Movement command handler
void handleMovementCommand(String command) {
  char axis;
  char sign;
  long value;
  int matched = sscanf(command.c_str(), "%c %c%ld", &axis, &sign, &value);

  value *= MULTIPLIER;

  if (matched >= 2) {
    switch (axis) {
      case 'x':
        stepperX.move((sign == '+' ? value : -value));
        break;
      case 'y':
        stepperY.move((sign == '+' ? value : -value));
        break;
      case 'z':
        stepperZ.move((sign == '+' ? value : -value));
        break;
      case 'a':
        servo_angle += (sign == '+' ? value : -value);
        myservo.write(servo_angle);
        break;
    }
  }
}

// Serial input event
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      inputComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
