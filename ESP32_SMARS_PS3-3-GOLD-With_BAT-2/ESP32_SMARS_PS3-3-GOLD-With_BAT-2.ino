

//SMARS Tracked Vehicle Controller PS3 on MH ET Live ESP32 Bluetooth
//Enhanced with Safety Features, Motor Ramping, and Improved Control

//Uses L9110 2-CHANNEL MOTOR DRIVER. Spec sheet-https://tinyurl.com/y88pgaka

//**********Libraries**********

#include <Ps3Controller.h>
#include "esp_adc_cal.h"

//********************CONFIGURATION PARAMETERS***************************
#define DEBUG_SERIAL true        // Set to false to disable debug printing for better performance
#define JOYSTICK_DEADZONE 5      // Configurable deadzone (was hardcoded at 3)
#define EVENT_TIMEOUT_MS 500     // Watchdog timeout - stops motors if no events for 500ms
#define RAMP_RATE 0.15           // Motor acceleration rate (0.1 = slower, 0.3 = faster)
#define STEERING_EXPO 2.0        // Steering exponential curve (1.0 = linear, 2.0 = squared, 3.0 = cubic)
#define PWM_FREQUENCY 20000      // PWM Frequency - 20kHz to reduce motor whine (was 30kHz)

//********************MOTORS***************************
uint32_t freq = PWM_FREQUENCY; //PWM Frequency
uint8_t resolution = 8; //PWM - 8 bit resolution, 2^^8 or 0 - 255

uint8_t motorA1AChannel = 0; //PWM Channel-A1A Front Motor +
uint8_t motorA1BChannel = 1; //PWM Channel-A1B Front Motor -
uint8_t motorB1AChannel = 2; //PWM Channel-B1A Rear Motor +
uint8_t motorB1BChannel = 3; //PWM Channel-B1B Rear Motor -

int motorA1ApwmPin = 27; //GPIO27 A1A Front Motor +
int motorA1BpwmPin = 25; //GPIO25 A1B Front Motor -
int motorB1ApwmPin = 26; //GPIO26 B1A Rear Motor +
int motorB1BpwmPin = 18; //GPIO18 B1B Rear Motor -

// Motor speed variables
int motorSpeed = 0;
int motorSpeedADJ = 0;

// Motor ramping variables (for smooth acceleration)
float currentSpeedLeft = 0.0;
float currentSpeedRight = 0.0;
float targetSpeedLeft = 0.0;
float targetSpeedRight = 0.0;

//********************MOTORS***************************

//*************Joystick Variables*********
int leftJoystickPos = 0;
int rightJoystickPos = 0;
int leftRight = 0;

//*************Joystick Variables*********

//***************Safety & Control Features*************
unsigned long lastEventTime = 0;  // Watchdog timer for event timeout
bool emergencyStop = false;       // Emergency stop flag
bool tankTurnMode = false;        // Tank turn mode (pivot in place)

// Speed limit modes: 0=50%, 1=75%, 2=100%
int speedLimitMode = 2;           // Default to 100%
float speedLimitFactors[3] = {0.5, 0.75, 1.0};

//***************Safety & Control Features*************

//***************Telemetry & Diagnostics****************
unsigned long lastTelemetryTime = 0;
unsigned long telemetryInterval = 1000; // Report telemetry every 1 second
unsigned long loopCounter = 0;
unsigned long lastLoopTime = 0;

//***************Telemetry & Diagnostics****************

//****************Battery Voltage Alarm******************************/
int batStatusLED = 0; //Variable to hold rover battery status on controller
int R1 = 101300; //Voltage divider R1
int R2 = 42300; //Voltage Divider R2
float vOutMax = 8.4 * R2 / (R1 + R2); // Calculate output of voltage divider with fully charged batteries
float mSlope = 1 / (vOutMax / 8.4); //Calculate slope of voltage correction curve

float adcRead = 0;
float batteryVoltage = 0; //computed battery voltage
float batteryVoltageSum = 0;
float batteryVoltageAvg = 0;
float batteryVoltageCorr = 0;

unsigned long analogReadCounter = 0;

const int batPin =  32; //GPIO32 - ADC1_CH4 Variable that holds the GPIO Address
unsigned long previousMillis = 0; //for timer reading A0
unsigned long interval = 2; // millis between read A0

int rumbleCounter = 0;

//****************END Battery Voltage Alarm******************************/

//*****************************************************************************
//****************************HELPER FUNCTIONS*********************************
//*****************************************************************************

// Emergency stop - immediately stop all motors and reset speeds
void emergencyStopMotors() {
  ledcWrite(motorA1AChannel, 0);
  ledcWrite(motorA1BChannel, 0);
  ledcWrite(motorB1AChannel, 0);
  ledcWrite(motorB1BChannel, 0);

  // Reset all speed variables
  currentSpeedLeft = 0.0;
  currentSpeedRight = 0.0;
  targetSpeedLeft = 0.0;
  targetSpeedRight = 0.0;
  motorSpeed = 0;
  motorSpeedADJ = 0;

  if (DEBUG_SERIAL) {
    Serial.println("*** EMERGENCY STOP ACTIVATED ***");
  }
}

// Apply exponential curve to steering for better fine control
float applySteeringCurve(float input) {
  // input is -1.0 to 1.0
  float sign = (input >= 0) ? 1.0 : -1.0;
  return pow(fabs(input), STEERING_EXPO) * sign;
}

// Ramp motor speeds smoothly to target
void rampMotorSpeeds() {
  // Smoothly ramp current speeds toward target speeds
  currentSpeedLeft += (targetSpeedLeft - currentSpeedLeft) * RAMP_RATE;
  currentSpeedRight += (targetSpeedRight - currentSpeedRight) * RAMP_RATE;

  // Apply ramped speeds (constrain to 0-255)
  int leftSpeed = constrain((int)fabs(currentSpeedLeft), 0, 255);
  int rightSpeed = constrain((int)fabs(currentSpeedRight), 0, 255);

  // Determine direction and apply to motors
  if (currentSpeedLeft >= 0) {
    // Left track forward
    ledcWrite(motorA1AChannel, 0);
    ledcWrite(motorA1BChannel, leftSpeed);
  } else {
    // Left track reverse
    ledcWrite(motorA1AChannel, leftSpeed);
    ledcWrite(motorA1BChannel, 0);
  }

  if (currentSpeedRight >= 0) {
    // Right track forward
    ledcWrite(motorB1AChannel, 0);
    ledcWrite(motorB1BChannel, rightSpeed);
  } else {
    // Right track reverse
    ledcWrite(motorB1AChannel, rightSpeed);
    ledcWrite(motorB1BChannel, 0);
  }
}

// Print telemetry data
void printTelemetry() {
  if (!DEBUG_SERIAL) return;

  unsigned long currentTime = millis();
  if (currentTime - lastTelemetryTime >= telemetryInterval) {
    Serial.println("===== TELEMETRY =====");
    Serial.print("Speed Limit Mode: ");
    Serial.print(speedLimitMode);
    Serial.print(" (");
    Serial.print(speedLimitFactors[speedLimitMode] * 100);
    Serial.println("%)");
    Serial.print("Tank Turn Mode: ");
    Serial.println(tankTurnMode ? "ON" : "OFF");
    Serial.print("Current Speed L/R: ");
    Serial.print(currentSpeedLeft);
    Serial.print(" / ");
    Serial.println(currentSpeedRight);
    Serial.print("Target Speed L/R: ");
    Serial.print(targetSpeedLeft);
    Serial.print(" / ");
    Serial.println(targetSpeedRight);
    Serial.print("Last Event: ");
    Serial.print(currentTime - lastEventTime);
    Serial.println("ms ago");
    Serial.print("Loop Frequency: ");
    if (lastLoopTime > 0) {
      Serial.print(1000.0 / (currentTime - lastLoopTime));
      Serial.println(" Hz");
    }
    Serial.println("====================");

    lastTelemetryTime = currentTime;
    lastLoopTime = currentTime;
  }
}

//*****************************************************************************
//*************************END HELPER FUNCTIONS********************************
//*****************************************************************************

//*****************************************************************************
//********************************VOID SETUP***********************************
//*****************************************************************************

void setup() {
  
Serial.begin(250000); // Serial Port Speed

  if (!Ps3.begin("b8:27:eb:37:85:b9")) { //MAC address of the PS3 Controller
    Serial.println("Initialization failed.");
    return;
  }
  Serial.println();
  Serial.println();
  Serial.println("*****************************************************");
  Serial.println("Initialization finished. Ready to pair PS3 controller");
  Serial.println("*****************************************************");
  Serial.println();
  Serial.println();
 
  Ps3.attach(onEvent);
  Ps3.attachOnConnect(onConnection);

//Battery Meter Setup
  pinMode(batPin, INPUT);
 
  pinMode(motorA1ApwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorA1BpwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorB1ApwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorB1BpwmPin,OUTPUT); //Set  PIN to OUTPUT

  ledcSetup(motorA1AChannel, freq, resolution);
  ledcAttachPin(motorA1ApwmPin, motorA1AChannel); 

  ledcSetup(motorA1BChannel, freq, resolution);
  ledcAttachPin(motorA1BpwmPin, motorA1BChannel);

  ledcSetup(motorB1AChannel, freq, resolution);
  ledcAttachPin(motorB1ApwmPin, motorB1AChannel);

  ledcSetup(motorB1BChannel, freq, resolution);
  ledcAttachPin(motorB1BpwmPin, motorB1BChannel);
}
//**********************************************************************
//*************************END VOID SETUP*******************************
//**********************************************************************

//**********************************************************************
//***************************VOID LOOP**********************************
//**********************************************************************

void loop() {

  loopCounter++;

  // SAFETY: Check if PS3 controller is disconnected
  if(!Ps3.isConnected()) {
    if (DEBUG_SERIAL) {
      Serial.println();
      Serial.println();
      Serial.println("***************************");
      Serial.println("PS3 Controller Disconnected");
      Serial.println("***************************");
      Serial.println();
      Serial.println();
    }

    // CRITICAL: Stop all motors immediately on disconnect
    emergencyStopMotors();

    return;
  }

  // SAFETY: Event timeout watchdog - stop motors if no events received for EVENT_TIMEOUT_MS
  unsigned long currentTime = millis();
  if (currentTime - lastEventTime > EVENT_TIMEOUT_MS && !emergencyStop) {
    if (DEBUG_SERIAL && (targetSpeedLeft != 0 || targetSpeedRight != 0)) {
      Serial.println("*** WATCHDOG TIMEOUT - No events received, stopping motors ***");
    }
    targetSpeedLeft = 0;
    targetSpeedRight = 0;
  }

  // Apply motor speed ramping for smooth acceleration/deceleration
  rampMotorSpeeds();

  // Read and compute battery voltage
  computeBatteryVoltage();

  // Print telemetry (rate-limited)
  printTelemetry();
}

//**************************************************************************
//*************************END VOID LOOP************************************
//**************************************************************************
