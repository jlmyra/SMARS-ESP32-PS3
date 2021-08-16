
void onEvent() {
  //   _______________________________________________________________
  //   |                           |                                 |
  //   |          Forward          |                                 |
  //   |           -128            |                                 |
  //   |            /\             |                                 |         
  //   |            /\             |               /\                |
  //   |         <  ly  >          |  Left -128 << rx >> +128 Right  |
  //   |            \/             |               \/                |
  //   |            \/             |                                 |              
  //   |           +128            |                                 |
  //   |          Reverse          |                                 |
  //   |                           |                                 |
  //   |       LEFT JOYSTICK       |         RIGHT JOYSTICK          |
  //   |___________________________|_________________________________|

  
//READ the JOYSTICKS

  if((abs(Ps3.event.analog_changed.stick.rx) > 2) || (abs(Ps3.event.analog_changed.stick.ly) > 2)){   
       leftJoystickPos = (Ps3.data.analog.stick.ly); //use left joystick for forward and reverse
       motorSpeed = map(leftJoystickPos, -128, 128, -256, 256); //map joystick 2**8 resolution
  
       rightJoystickPos = (Ps3.data.analog.stick.rx); //uses right joystick x-position for steering left/right   
       leftRight = map(rightJoystickPos, -128, 128, -256, 256); //map joystick 2**8 resolution, use this for determining the steering motor speed
       
            if(leftRight >= 2){
             motorSpeedADJ = map(leftRight, 0, 256, 256, 0);//Right turn, slow the right track
              }
            else if(leftRight <= -2){
             leftRight = -leftRight;
             motorSpeedADJ = map(leftRight, 0, 256, 256, 0);//Left turn, slow the left track
              }
                  
  //***STOP***
 
   if((leftJoystickPos >= -3) && (leftJoystickPos <= 3)) {
      ledcWrite(motorA1AChannel, 0);
      ledcWrite(motorA1BChannel, 0);
      ledcWrite(motorB1AChannel, 0);
      ledcWrite(motorB1BChannel, 0);
      
      Serial.print("STOP - "); Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); Serial.print(" rightJoystickPos="); 
      Serial.print(rightJoystickPos); Serial.print(" leftRight = "); Serial.print(leftRight);
  }else 

  //***FORWARD*** 

   if((leftJoystickPos < -3)  && (rightJoystickPos > -2 && rightJoystickPos < 2)) {
    
    motorSpeed = -motorSpeed;
      
      ledcWrite(motorA1AChannel, 0);
      ledcWrite(motorA1BChannel, motorSpeed); 
      ledcWrite(motorB1AChannel, 0);
      ledcWrite(motorB1BChannel, motorSpeed);
       
      Serial.print("FORWARD - "); Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); Serial.print(" rightJoystickPos="); 
      Serial.print(rightJoystickPos); Serial.print(" motorSpeed = "); Serial.println(motorSpeed);
    }else

  //***BACKWARD***
   
   if((leftJoystickPos > 3)  && (rightJoystickPos > -2 && rightJoystickPos < 2)) { 

      ledcWrite(motorA1AChannel, motorSpeed);
      ledcWrite(motorA1BChannel, 0); 
      ledcWrite(motorB1AChannel, motorSpeed);
      ledcWrite(motorB1BChannel, 0);
           
      Serial.print("REVERSE - "); Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); Serial.print(" rightJoystickPos="); 
      Serial.print(rightJoystickPos); Serial.print("  motorSpeed= "); Serial.println (motorSpeed);  
   }else

  //***FORWARD DIAGONAL RIGHT***
  
   if((leftJoystickPos) < -3 && (rightJoystickPos) > 3) {
      
      motorSpeed = -motorSpeed;
        
      ledcWrite(motorA1AChannel, 0);
      ledcWrite(motorA1BChannel, motorSpeed); 
      ledcWrite(motorB1AChannel, 0);
      ledcWrite(motorB1BChannel, motorSpeedADJ); 
     
      Serial.print("FORWARD DIAGONAL RIGHT-> ");Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); 
      Serial.print(" rightJoystickPos="); Serial.print(rightJoystickPos); 
      Serial.print(" motorSpeedADJ= "); Serial.println(motorSpeedADJ);          
  }else

  //***BACKWARD DIAGONAL RIGHT***
  
   if((leftJoystickPos) > 3 && (rightJoystickPos) > 3) {

      ledcWrite(motorA1AChannel, motorSpeed); //Turning Side slower than opposing side
      ledcWrite(motorA1BChannel, 0);
      ledcWrite(motorB1AChannel, motorSpeedADJ);            
      ledcWrite(motorB1BChannel, 0);
           
      Serial.print("BACKWARD DIAGONAL RIGHT-> "); Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); 
      Serial.print(" rightJoystickPos="); Serial.print(rightJoystickPos); 
      Serial.print(" motorSpeedADJ= "); Serial.println(motorSpeedADJ);     
  }else

  //***BACKWARD DIAGONAL LEFT***
  
   if((leftJoystickPos) > 3 && (rightJoystickPos) < -3) {

      ledcWrite(motorA1AChannel, motorSpeedADJ);        
      ledcWrite(motorA1BChannel, 0);
      ledcWrite(motorB1AChannel, motorSpeed);
      ledcWrite(motorB1BChannel, 0); 
       
      Serial.print("BACKWARD DIAGONAL LEFT-> ");Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); 
      Serial.print(" rightJoystickPos="); Serial.print(rightJoystickPos);
      Serial.print(" motorSpeedADJ= "); Serial.println(motorSpeedADJ);
  }else
                                    
  //***FORWARD DIAGONAL LEFT***
  
    if ((leftJoystickPos) < -3 && (rightJoystickPos) < -3) {
      
      motorSpeed = -motorSpeed;

      ledcWrite(motorA1AChannel, 0);
      ledcWrite(motorA1BChannel, motorSpeedADJ); 
      ledcWrite(motorB1AChannel, 0);
      ledcWrite(motorB1BChannel, motorSpeed);
      
      Serial.print("FORWARD DIAGONAL LEFT-> "); Serial.print("leftJoystickPos="); Serial.print(leftJoystickPos); 
      Serial.print(" rightJoystickPos="); Serial.print(rightJoystickPos);
      Serial.print(" motorSpeedADJ= "); Serial.println(motorSpeedADJ);    
     }   
    }
   }
