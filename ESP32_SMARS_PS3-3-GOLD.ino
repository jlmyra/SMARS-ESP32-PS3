

//SMARS Tracked Vehicle Controller PS3 on MH ET Live ESP32 Bluetooth

//Uses L9110 2-CHANNEL MOTOR DRIVER. Spec sheet-https://tinyurl.com/y88pgaka

//**********Libraries**********

#include <Ps3Controller.h>

//********************MOTORS***************************
uint32_t freq = 30000;
uint8_t resolution = 8;

uint8_t motorA1AChannel = 0; //PWM Channel-A1A Front Motor + 
uint8_t motorA1BChannel = 1; //PWM Channel-AIB Front Motor - 
uint8_t motorB1AChannel = 2; //PWM Channel-B1A Rear Motor +
uint8_t motorB1BChannel = 3; //PWM Channel-B1B Rear Motor -

int motorA1ApwmPin = 27; //GPIO27 A1A Front Motor + 
int motorA1BpwmPin = 25; //GPIO25 A1B Front Motor -  
int motorB1ApwmPin = 26; //GPIO26 B1A Rear Motor +
int motorB1BpwmPin = 18; //GPIO18 B1B Rear Motor -

float motorSpeedIncrement = 20;
int motorSpeed = 0;
int oldMotorSpeed = 0;
int motorSpeedSlow = 0;
int motorSpeedADJ = 0;
int oldMotorSpeedADJ = 0;

//********************MOTORS***************************

//*************Joystick Variables*********
int leftJoystickPos = 0;
int rightJoystickPos = 0;
int leftRight = 0;

//*************Joystick Variables*********

//****************Battery Voltage Alarm******************************/
int batStatusLED = 0; //Variable to hold rover battery status on controller

float bitVoltage; //ADC converted voltage on batPin A0

float batteryVoltage; //computed battery voltage

int batRemPercent;

const int batPin =  32; //GPIO32 - ADC1_CH4 Variable that holds the GPIO Address 
unsigned long previousMillis = 0; //for timer reading A0
unsigned long interval = 1000; // millis between read A0

int rumbleCounter = 0;

//****************END Battery Voltage Alarm******************************/
 
//***Ramp Timers***
unsigned long millisBefore = 0; //for ramp timer 
unsigned long rampInterval = 1; // millis between ramping change
 
//*****************************************************************************
//********************************VOID SETUP***********************************
//*****************************************************************************

void setup() {
  
Serial.begin(250000);

  if (!Ps3.begin("b8:27:eb:37:85:b9")) {
    Serial.println("Initialization failed.");
    return;
  }
  Serial.println("Initialization finished.");
 
  Ps3.attach(onEvent);
  Ps3.attachOnConnect(onConnection);

//Battery Meter Setup
  pinMode(batPin, INPUT);
  analogSetPinAttenuation(batPin, ADC_0db);
 
  pinMode(motorA1ApwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorA1BpwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorB1ApwmPin,OUTPUT); //Set  PIN to OUTPUT
  pinMode(motorB1BpwmPin,OUTPUT); //Set  PIN to OUTPUT

  ledcSetup(motorA1AChannel, freq, resolution);
  ledcAttachPin(motorA1ApwmPin, motorA1AChannel); //GPIO25, DOIT D25, WEMOS I25

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

  void loop(){
    
  if(!Ps3.isConnected())
        return; 
           
  //computeBatteryVoltage();
  
}

//**************************************************************************
//*************************END VOID LOOP************************************
//**************************************************************************
