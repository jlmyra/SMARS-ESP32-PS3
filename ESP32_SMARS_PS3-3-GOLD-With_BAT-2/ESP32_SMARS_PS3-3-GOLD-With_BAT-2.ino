

//SMARS Tracked Vehicle Controller PS3 on MH ET Live ESP32 Bluetooth

//Uses L9110 2-CHANNEL MOTOR DRIVER. Spec sheet-https://tinyurl.com/y88pgaka

//**********Libraries**********

#include <Ps3Controller.h>
#include "esp_adc_cal.h"

//********************MOTORS***************************
uint32_t freq = 30000; //PWM Frequency
uint8_t resolution = 8; //PWM - 8 bit resolution, 2^^12 or 0 - 255 

uint8_t motorA1AChannel = 0; //PWM Channel-A1A Front Motor + 
uint8_t motorA1BChannel = 1; //PWM Channel-A1B Front Motor - 
uint8_t motorB1AChannel = 2; //PWM Channel-B1A Rear Motor +
uint8_t motorB1BChannel = 3; //PWM Channel-B1B Rear Motor -

int motorA1ApwmPin = 27; //GPIO27 A1A Front Motor + 
int motorA1BpwmPin = 25; //GPIO25 A1B Front Motor -  
int motorB1ApwmPin = 26; //GPIO26 B1A Rear Motor +
int motorB1BpwmPin = 18; //GPIO18 B1B Rear Motor -

int motorSpeed = 0;
int motorSpeedADJ = 0;

//********************MOTORS***************************

//*************Joystick Variables*********
int leftJoystickPos = 0;
int rightJoystickPos = 0;
int leftRight = 0;

//*************Joystick Variables*********

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

  void loop(){
    
  if(!Ps3.isConnected()){//If PS3 controller is not connected - time out
    
  Serial.println();
  Serial.println();
  Serial.println("***************************");
  Serial.println("PS3 Controller Disconnected");
  Serial.println("***************************");
  Serial.println();
  Serial.println();
  
  return; 
  }
   
                 
  computeBatteryVoltage(); // Function to read ADC and compute the battery voltage
  
}

//**************************************************************************
//*************************END VOID LOOP************************************
//**************************************************************************
