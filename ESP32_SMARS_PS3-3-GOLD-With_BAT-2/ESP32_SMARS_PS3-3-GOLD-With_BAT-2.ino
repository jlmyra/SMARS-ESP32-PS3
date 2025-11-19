

//SMARS Tracked Vehicle Controller PS3 on MH ET Live ESP32 Bluetooth
//Enhanced with Safety Features, Motor Ramping, and Improved Control

//Uses L9110 2-CHANNEL MOTOR DRIVER. Spec sheet-https://tinyurl.com/y88pgaka

//**********Libraries**********

#include <Ps3Controller.h>
#include "esp_adc_cal.h"
#include "settings.h"  // User configuration file

//********************MOTORS***************************
uint32_t freq = PWM_FREQUENCY; //PWM Frequency from settings.h
uint8_t resolution = PWM_RESOLUTION; //PWM resolution from settings.h

// PWM Channel assignments from settings.h
uint8_t motorA1AChannel = MOTOR_LEFT_REV_CHANNEL;  // Left Motor Reverse
uint8_t motorA1BChannel = MOTOR_LEFT_FWD_CHANNEL;  // Left Motor Forward
uint8_t motorB1AChannel = MOTOR_RIGHT_REV_CHANNEL; // Right Motor Reverse
uint8_t motorB1BChannel = MOTOR_RIGHT_FWD_CHANNEL; // Right Motor Forward

// GPIO Pin assignments from settings.h
int motorA1ApwmPin = MOTOR_LEFT_REV_PIN;   // Left Motor Reverse Pin
int motorA1BpwmPin = MOTOR_LEFT_FWD_PIN;   // Left Motor Forward Pin
int motorB1ApwmPin = MOTOR_RIGHT_REV_PIN;  // Right Motor Reverse Pin
int motorB1BpwmPin = MOTOR_RIGHT_FWD_PIN;  // Right Motor Forward Pin

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

// Speed limit modes from settings.h
int speedLimitMode = DEFAULT_SPEED_LIMIT_MODE;
float speedLimitFactors[3] = {SPEED_LIMIT_MODE_0, SPEED_LIMIT_MODE_1, SPEED_LIMIT_MODE_2};

//***************Safety & Control Features*************

//***************Telemetry & Diagnostics****************
unsigned long lastTelemetryTime = 0;
unsigned long telemetryInterval = TELEMETRY_INTERVAL_MS; // From settings.h
unsigned long loopCounter = 0;
unsigned long lastLoopTime = 0;

//***************Telemetry & Diagnostics****************

//****************Battery Voltage Alarm******************************/
int batStatusLED = 0; //Variable to hold rover battery status on controller
int R1 = BATTERY_R1; //Voltage divider R1 from settings.h
int R2 = BATTERY_R2; //Voltage Divider R2 from settings.h

float adcRead = 0;
float batteryVoltage = 0; //voltage at ADC pin (after voltage divider)
float batteryVoltageSum = 0;
float batteryVoltageAvg = 0; //averaged voltage at ADC pin
float batteryVoltageCorr = 0; //actual battery voltage (after divider calculation & correction)

unsigned long analogReadCounter = 0;

const int batPin = BATTERY_ADC_PIN; //Battery ADC pin from settings.h
unsigned long previousMillis = 0; //for timer reading battery ADC
unsigned long interval = BATTERY_READ_INTERVAL_MS; // Battery read interval from settings.h

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

Serial.begin(SERIAL_BAUD_RATE); // Serial Port Speed from settings.h

  if (!Ps3.begin(PS3_MAC_ADDRESS)) { //MAC address from settings.h
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
