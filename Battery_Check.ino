//*************************Compute Battery Voltage function***********************
  
  // The max voltage of the ESP32 ADC at the analogSetPinAttenuation() of 11db setting is 3.3V (see void setup()). 
  // The ESP32 ADC is non-linear from about 2.6V to 3.3V. A voltage divider is needed to drop the battery voltage from 8.4
  // (fully charged) to ~ 2.5V where the ADC becomes linear (but noisy). To smooth out the noise the sketch reads the 
  // battery voltage every 2ms, 500 times and computes an average. In other words every second a new voltage measurement is
  // computed~.The voltage divider uses an R1 of 68k ohms and R2 of ~29,500 ohms. 
  // A 0.1uf capacitor should be connected across Vout and ground to reduce noise.
  // The ESP32 can communicate the status of the battery charge to the PS3 controller by using 
  // the 4 connection leds. LED 4 is full charge, LED 3 is 3/4. LED 2 is 50%. LED 1 means it time to recharge the battery.
  // Full charge is ~8.4V Should not use the battery below 6.4V (80% of 8.3V or 3.2V/cell*2 cells)
  // Your reisitors and ESP32 combination may yield different results. It may be useful to check the serial monitor readings 
  // with a volt meter.

  void computeBatteryVoltage(){
    
  unsigned long currentMillis = millis(); //set the current time
  if (currentMillis - previousMillis > interval) { //check to see if the interval has been met
  previousMillis = currentMillis; //reset the time

  if (analogReadCounter < 500){
    analogReadCounter = analogReadCounter + 1;
    bitVoltage = analogRead(batPin);
    bitVoltageSum = bitVoltageSum + bitVoltage;
    //Serial.print("analogReadCounter= "); Serial.println(analogReadCounter);
  }

  if (analogReadCounter == 500){
    //Serial.print("analogReadCounter= "); Serial.println(analogReadCounter);
    bitVoltageAvg = bitVoltageSum / 500;
    batteryVoltage = ((bitVoltageAvg / 734) * 8.4); // 734 is A0 ADC reading with fully charged batteries thru the voltage divider
                                                    // 8.4 is the Voltage of 2 18650's fully charged
                                                      
    Serial.print ("bitVoltageAvg= "); Serial.print (bitVoltageAvg); Serial.print("\t"); Serial.print("batteryVoltage= "); 
    Serial.print (batteryVoltage); Serial.print("  RumbleCounter= "); Serial.println(rumbleCounter);

    bitVoltageSum = 0;
    analogReadCounter = 0;
 
  if(bitVoltageAvg > 685) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 10; //4 LED's
   rumbleCounter = 0;
   bitVoltageAvg = 0;
  }
  else if(bitVoltageAvg <= 684 && bitVoltage > 643 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 9; // 3 LEDS
   rumbleCounter = 0; 
   bitVoltageAvg = 0;
  }
  else if(bitVoltageAvg <= 642 && bitVoltage > 600 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 7; // 2 LEDS
   rumbleCounter = 0; 
   bitVoltageAvg = 0;
  }
   else if(bitVoltageAvg < 560 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 4;
   bitVoltageAvg = 0;

//************************SET RUMBLE*************************

   if (rumbleCounter == 14){  //15 seconds or rumbleCounter * interval(millis)
   
    ps3_cmd_t cmd;

    cmd.rumble_left_intensity = 0x7d;
    cmd.rumble_right_intensity = 0x7d; //0xff = 255
  
    cmd.rumble_right_duration = 100;
    cmd.rumble_left_duration = 100;
  
    ps3Cmd(cmd);
    //Serial.print("rumbleCounter ="); Serial.println(rumbleCounter);
    rumbleCounter = 13;
   }
   
  rumbleCounter++;
  
//***********************END SET RUMBLE*********************
      }
    }
  }
}
//************************END Compute Battery Voltage function***********************
