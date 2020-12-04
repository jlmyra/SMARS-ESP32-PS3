//*************************Compute Battery Voltage function***********************
  
  // The max voltage this ESP32 ADC is 1.0V. In void setup the adc attenuation is set to 0 db.
  // A voltage divider is required to drop the battery max voltage of 8.3 to 1.0 on GPIO 32. This divider
  // has R1 = 1470 on (+) side and R2 = 200 on (-) side. A 0.1uf capacitor is connected across Vout and ground.
  // The ESP32 can communicate the status of the battery charge to the PS3 controller by using 
  // the 4 connection leds. LED 4 is full charge, LED 3 is 3/4. LED 1 means it time to recharge the battery.
  // The esp32 has 10 bit resolution (4096) on ADC pins vs 8 bit on ESP8266
  // Battery Specs - http://www.zeeepower.com/pro.asp?keyno=1475&bascode=101&name=Zeee-4600mAh-100C-7.4V-2s-Shorty-Lipo-Battery-with-Deans-T-plug(4mm-Bullet)
  // Full charge is ~8.3V Should not use the battery below 6.4V (80% of 8.3V or 3.2V/cell*2 cells)
  // Using 7V as the lower limit = 3220 bitVoltage or .850 V at A0

  
  void computeBatteryVoltage(){
    
  unsigned long currentMillis = millis(); //set the current time
  if (currentMillis - previousMillis > interval) { //check to see if the interval has been met
  previousMillis = currentMillis; //reset the time
  
  bitVoltage = analogRead(batPin); // read the ADC voltage on Pin A0
  //batteryVoltage = (8.3 * bitVoltage / 4096) + .45; //Convert the ADC voltage to actual voltage
  batteryVoltage = ((bitVoltage / 4096) * 8.3) + .3;
  //batRemPercent = map(bitVoltage, 3280, 4096, 0, 100); //Map ADC voltage to percent
  Serial.print ("bitVoltage= "); Serial.print (bitVoltage); Serial.print("\t"); Serial.print("batteryVoltage= "); 
  Serial.print (batteryVoltage); Serial.print("RumbleCounter= "); Serial.println(rumbleCounter);
 
  if(bitVoltage > 3680) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 10; //4 LED's
   rumbleCounter = 0;
  }
  else if(bitVoltage <= 3680 && bitVoltage > 3480 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 9; 
   rumbleCounter = 0; 
  }
  else if(bitVoltage <= 3480 && bitVoltage > 3280 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 7; 
   rumbleCounter = 0; 
  }
   else if(bitVoltage < 3280 ) {
   Ps3.setPlayer(batStatusLED);
   batStatusLED = 4;

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
//************************END Compute Battery Voltage function***********************
