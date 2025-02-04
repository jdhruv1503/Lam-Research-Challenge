#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>


float Kp = 2;
float Kd = 0;
float Ki = 0;

float Pterm = 0;
float Dterm = 0;
float Iterm = 0;

unsigned long currTime = 0;
unsigned long prevTime = 0;

float waterTemp = 0;

float prevError = 0;

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 18

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

int encoderPinA = 2; // CLK pin
int encoderPinB = 4; // DT pin
int buttonPin;
volatile int buttonSet = 0;

volatile int count = 10;
volatile int encoderPinA_prev = 0;
int encoderPinA_value;
int prevButtonState = 0;


volatile int encoderPinB_prev = 0;
volatile int encoderPinB_value;




#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// void IRAM_ATTR rotaryEncoderA() {
//   if (encoderPinB_prev) {
//     count += 1;
//     encoderPinB_prev = 0;
//     encoderPinA_prev = 0;
//   } else {
//     encoderPinA_prev = 1;
//   }

// }

// void IRAM_ATTR rotaryEncoderB() {
//   if (encoderPinA_prev) {
//     count -= 1;
//     encoderPinA_prev = 0;
//     encoderPinB_prev = 0;
//   } else {
//     encoderPinB_prev = 1;
//   }
  
// }

void knobCallback( long value )
{
	count += value;
}

void IRAM_ATTR rotaryEncoder() {
  if (buttonSet) {
    count += 1;
  } else {
    count -= 1;
  }
}


void setup() {
  Serial.begin(115200);
  sensors.begin();


  pinMode (encoderPinB, INPUT_PULLUP);

  pinMode(encoderPinA, INPUT_PULLUP);

  encoderPinA_prev = digitalRead(encoderPinA);
  encoderPinB_prev = digitalRead(encoderPinB);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // attachInterrupt(encoderPinA, rotaryEncoderA, CHANGE);
  // attachInterrupt(encoderPinB, rotaryEncoderB, CHANGE);


  display.clearDisplay();
  startSequence();

  currTime = millis();

  attachInterrupt(encoderPinA, rotaryEncoder, CHANGE);
}

void loop () {
  int currButtonState = digitalRead(buttonPin);
  if (currButtonState) {
    
  }
  updateWaterTemp();
  changeTempDisplay(waterTemp, count);
  PID();
}


void PID() {
  currTime = millis();
  float error = waterTemp - count;
  unsigned long delta = (currTime - prevTime <= 0 ? 0 : currTime-prevTime);

  Pterm = error*Kp;
  Dterm = Kd*(error-prevError)/(1+delta); //Adding 1 to prevent division by 0 error
  Iterm = Ki*error*(currTime-prevTime);

  int value = ((int)(Pterm+Dterm+Iterm));
  prevTime = currTime;
  prevError = error;

  int scaledValue = map(constrain(value, 0 * (Kp + Ki + Kd), 15 * (Kp + Ki + Kd)), 0 * (Kp + Ki + Kd), 15 * (Kp + Ki + Kd), 0, 240);
  Serial.println(value);

  setPeltier(scaledValue);
}


void startSequence() {
  Serial.println();
  Serial.println("Starting Display");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40,0);
  display.println(F("TEMP"));
  display.setCursor(24,18);
  display.println(F("CONTROL"));
  display.display();
  Serial.println("Successful");

  delay(2000);

  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("TEMP:"));
  display.setCursor(64,0);
  display.println(F("00.0"));
  display.display();      // Show initial text
  delay(100);

  display.setCursor(0,18);
  display.println(F("SET:"));
  display.setCursor(64,18);
  display.println(F("00.0"));
  display.display();
}



void changeTempDisplay(float temp, float set) {
  if (temp>0){
    display.fillRect(64,0,50,18, SSD1306_BLACK);

    display.setCursor(64,0);
    
    char text[] = "    ";
    char* print_text = convertFloat(temp, text);
    display.println(print_text);
    display.display();
  }
  if (set > 0){
    display.fillRect(64,18,50,18, SSD1306_BLACK);

    display.setCursor(64,18);

    char text[] = "    ";
    char* print_text = convertFloat(set, text);
    display.println(print_text);
    display.display();
  }
}

char* convertFloat(float value, char* charArray) {


  // Assign digits to the character array
  int tens = (static_cast<int>(value) / 10) % 10;        // Tens place
  int ones = static_cast<int>(value) % 10;        // Ones place
  int decimal = static_cast<int>((value * 10)) % 10; // First digit after the decimal

  // Assign values to the character array
  charArray[0] = '0' + tens; // First element
  charArray[1] = '0' + ones; // Second element
  charArray[2] = '.';        // Third element
  charArray[3] = '0' + decimal; // Fourth element
  charArray[4] = '\0';       // Null terminator

  return charArray;
}



void updateWaterTemp(){
  sensors.requestTemperatures(); 
  
  //Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  waterTemp = sensors.getTempCByIndex(0);
  //Serial.println(waterTemp);
  
}

void setPeltier(int val) {
  analogWrite(5, val); //Implement changing the voltage of desired pin here
  Serial.println(val);
}