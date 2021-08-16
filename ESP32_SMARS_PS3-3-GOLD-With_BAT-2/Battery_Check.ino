//*************************Compute Battery Voltage function***********************
  
  // The max voltage of the ESP32 ADC at the analogSetPinAttenuation() of 11db setting is 3.3V (see void setup()). 
  // The ESP32 ADC is non-linear from about 2.6V to 3.3V. A voltage divider is needed to drop the battery voltage
  // from 8.4V (fully charged) to ~ 2.5V where the ADC becomes linear (but noisy). To smooth out the noise the 
  // sketch reads the battery voltage every 2ms, 500 times and computes an average. In other words every second 
  // a new voltage measurement is computed~.The voltage divider uses an R1 of 68k ohms and R2 of ~29,500 ohms. 
  // A 0.1uf capacitor should be connected across Vout and ground to reduce noise.
  // The ESP32 can communicate the status of the battery charge to the PS3 controller by using 
  // the 4 connection leds. LED 4 is full charge, LED 3 is 3/4. LED 2 is 50%. LED 1 means it time to recharge
  // the battery.
  // Full charge is ~8.4V Should not use the battery below 6.7V (80% of 8.4V or 3.2V/cell*2 cells)
  // see - DeepBlueEmbedded describes a method for using the ESP32 internal reference voltage calibration and 
  // correction.
  // This sketch follows the description here:
  // https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/ for more detail

  void computeBatteryVoltage(){
    
  unsigned long currentMillis = millis(); //set the current time
  if (currentMillis - previousMillis > interval) { //check to see if the interval has been met, interval = 2 millis
  previousMillis = currentMillis; //reset the time

  if (analogReadCounter < 500){ //Read the ADC 500 times every 2 millis or 500 times a second
    analogReadCounter = analogReadCounter + 1;
    adcRead = analogRead(batPin); //Read the battery voltage on the ADC and correct it on the next line
    batteryVoltage = readADC_Cal(adcRead); //ADC Calibrated read function - see below returns mV
    batteryVoltage = ((batteryVoltage) * .001) - 0.07; //convert mV to V & add 0.07 offset to agree with voltmeter
    batteryVoltageSum = batteryVoltageSum + batteryVoltage; //Sum the voltage readings
    //Serial.print("batteryVoltageSum= "); Serial.println(batteryVoltage);
  }

  if (analogReadCounter == 500){
    //Serial.print("analogReadCounter= "); Serial.println(analogReadCounter);
    batteryVoltageAvg = batteryVoltageSum / 500; //Compute the Average
    batteryVoltageCorr = (batteryVoltageAvg + 0.07) * mSlope; //Correct the ADC reading to Actual Battery Voltage
                                                     
    Serial.print ("batteryVoltageAvg= "); Serial.print (batteryVoltageAvg); Serial.print("\t"); Serial.print("batteryVoltageCorr= "); 
    Serial.print (batteryVoltageCorr); Serial.print("  RumbleCounter= "); Serial.println(rumbleCounter);

    batteryVoltageSum = 0;
    analogReadCounter = 0;
 // Test the battery voltage and set the led's accordingly
  if (batteryVoltageCorr > 8.0) {
    Ps3.setPlayer(batStatusLED);
    batStatusLED = 10; //4 LED's
    rumbleCounter = 0;
    batteryVoltageCorr = 0;
  }
  else if(batteryVoltageCorr <= 7.8 && batteryVoltageCorr > 7.3 ) {
    Ps3.setPlayer(batStatusLED);
    batStatusLED = 9; // 3 LEDS
    rumbleCounter = 0; 
    batteryVoltageCorr = 0;
  }
  else if(batteryVoltageCorr <= 7.3 && batteryVoltageCorr > 6.7 ) {
    Ps3.setPlayer(batStatusLED);
    batStatusLED = 7; // 2 LEDS
    rumbleCounter = 0; 
    batteryVoltageCorr = 0;
  }
    else if(batteryVoltageCorr < 6.7 ) {
    Ps3.setPlayer(batStatusLED);
    batStatusLED = 4;
    batteryVoltageCorr = 0;

//************************SET RUMBLE*************************
// if the battery voltage drops below 6.7 volts for more than 15 seconds turn on the PS3 Rumble
// to alert the user

   if (rumbleCounter == 14){  //15 seconds or rumbleCounter * interval(millis)
   
    ps3_cmd_t cmd;

    cmd.rumble_left_intensity = 0x7d;
    cmd.rumble_right_intensity = 0x7d; //0xff = 255
  
    cmd.rumble_right_duration = 100;
    cmd.rumble_left_duration = 100;
  
    ps3Cmd(cmd);
    //Serial.print("rumbleCounter ="); Serial.println(rumbleCounter);
    rumbleCounter = 13; //keep the rumble on
   }
   
    rumbleCounter++; // rumbleCounter initialized at 0
  
//***********************END SET RUMBLE*********************
      } // else if <6.7
     } // if analogReadCounter
    } // if currentMillis
  } // void computeBatteryVoltage

//***********************ADC Read Calibration Function*********************

   uint32_t readADC_Cal(int ADC_Raw){
    
   esp_adc_cal_characteristics_t adc_chars;
  
   esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
   return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
  }

//************************END Compute Battery Voltage function***********************
