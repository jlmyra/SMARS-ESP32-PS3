
//*****************************************************************************
//***************************ONEVENT HANDLER***********************************
//*****************************************************************************
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
//
// CONTROLS:
// - Left Stick Y-axis: Forward/Reverse
// - Right Stick X-axis: Left/Right Turn (or Tank Turn if in tank mode)
// - L1 Button: Cycle speed limit (50% -> 75% -> 100%)
// - R1 Button: Toggle tank turn mode
// - PS Button: Emergency stop
//
//*****************************************************************************

void onEvent() {

  // Update last event time for watchdog
  lastEventTime = millis();

  //***********************************************************************
  // EMERGENCY STOP - PS Button
  //***********************************************************************
  if (Ps3.event.button_down.ps) {
    emergencyStop = !emergencyStop;
    if (emergencyStop) {
      emergencyStopMotors();
      if (DEBUG_SERIAL) {
        Serial.println("*** EMERGENCY STOP - Press PS button again to resume ***");
      }
    } else {
      if (DEBUG_SERIAL) {
        Serial.println("*** EMERGENCY STOP RELEASED - Ready to move ***");
      }
    }
    return;
  }

  // If emergency stop is active, don't process any other commands
  if (emergencyStop) {
    targetSpeedLeft = 0;
    targetSpeedRight = 0;
    return;
  }

  //***********************************************************************
  // SPEED LIMIT MODE - L1 Button cycles through 50%, 75%, 100%
  //***********************************************************************
  if (Ps3.event.button_down.l1) {
    speedLimitMode = (speedLimitMode + 1) % 3;
    if (DEBUG_SERIAL) {
      Serial.print("Speed Limit Mode: ");
      Serial.print(speedLimitFactors[speedLimitMode] * 100);
      Serial.println("%");
    }
  }

  //***********************************************************************
  // TANK TURN MODE - R1 Button toggles tank turn (pivot in place)
  //***********************************************************************
  if (Ps3.event.button_down.r1) {
    tankTurnMode = !tankTurnMode;
    if (DEBUG_SERIAL) {
      Serial.print("Tank Turn Mode: ");
      Serial.println(tankTurnMode ? "ON" : "OFF");
    }
  }

  //***********************************************************************
  // JOYSTICK INPUT PROCESSING
  //***********************************************************************

  // Check if there's significant joystick movement
  if ((abs(Ps3.event.analog_changed.stick.rx) > 2) || (abs(Ps3.event.analog_changed.stick.ly) > 2)) {

    // Read joystick positions
    leftJoystickPos = Ps3.data.analog.stick.ly;    // Forward/Reverse (-128 to +128)
    rightJoystickPos = Ps3.data.analog.stick.rx;   // Left/Right steering (-128 to +128)

    //***********************************************************************
    // TANK TURN MODE - Right stick controls in-place rotation
    //***********************************************************************
    if (tankTurnMode && abs(leftJoystickPos) < JOYSTICK_DEADZONE) {
      // Tank turn: right stick controls rotation in place
      if (abs(rightJoystickPos) > JOYSTICK_DEADZONE) {
        float turnPower = map(abs(rightJoystickPos), JOYSTICK_DEADZONE, 128, 0, 256) / 256.0;
        turnPower = applySteeringCurve(turnPower);
        int turnSpeed = (int)(turnPower * 256 * speedLimitFactors[speedLimitMode]);

        if (rightJoystickPos < 0) {
          // Turn right: left forward, right reverse (inverted stick direction)
          targetSpeedLeft = turnSpeed;
          targetSpeedRight = -turnSpeed;
        } else {
          // Turn left: left reverse, right forward (inverted stick direction)
          targetSpeedLeft = -turnSpeed;
          targetSpeedRight = turnSpeed;
        }

        if (DEBUG_SERIAL) {
          Serial.print("TANK TURN - ");
          Serial.print(rightJoystickPos < 0 ? "RIGHT" : "LEFT");  // Inverted
          Serial.print(" - Speed: ");
          Serial.println(turnSpeed);
        }
      } else {
        // No input - stop
        targetSpeedLeft = 0;
        targetSpeedRight = 0;
      }
      return;
    }

    //***********************************************************************
    // NORMAL DRIVING MODE
    //***********************************************************************

    // Apply deadzone to joystick inputs
    int throttle = 0;
    int steering = 0;

    if (abs(leftJoystickPos) > JOYSTICK_DEADZONE) {
      throttle = leftJoystickPos;
    }

    if (abs(rightJoystickPos) > JOYSTICK_DEADZONE) {
      steering = -rightJoystickPos;  // Negate to fix inverted steering
    }

    // If both sticks are in deadzone, stop
    if (throttle == 0 && steering == 0) {
      targetSpeedLeft = 0;
      targetSpeedRight = 0;
      if (DEBUG_SERIAL) {
        Serial.println("STOP");
      }
      return;
    }

    // Calculate base motor speed from throttle (-256 to +256)
    // Negative throttle = forward (joystick convention)
    float baseSpeed = -map(abs(throttle), JOYSTICK_DEADZONE, 128, 0, 256);
    if (throttle > 0) baseSpeed = -baseSpeed;  // Reverse if throttle positive

    // Apply speed limit
    baseSpeed *= speedLimitFactors[speedLimitMode];

    // Calculate steering adjustment with exponential curve
    float steeringNormalized = steering / 128.0;  // -1.0 to 1.0
    float steeringCurved = applySteeringCurve(steeringNormalized);

    // Calculate differential steering
    // steeringFactor ranges from 0.0 (full turn) to 1.0 (no turn)
    float steeringFactor = 1.0 - abs(steeringCurved);

    // Apply steering to tracks
    if (steering > 0) {
      // Turning right - slow down right track
      targetSpeedLeft = baseSpeed;
      targetSpeedRight = baseSpeed * steeringFactor;
    } else if (steering < 0) {
      // Turning left - slow down left track
      targetSpeedLeft = baseSpeed * steeringFactor;
      targetSpeedRight = baseSpeed;
    } else {
      // No steering - both tracks same speed
      targetSpeedLeft = baseSpeed;
      targetSpeedRight = baseSpeed;
    }

    // Debug output
    if (DEBUG_SERIAL) {
      String direction = "";
      if (baseSpeed > 0) {
        direction = "FORWARD";
      } else if (baseSpeed < 0) {
        direction = "REVERSE";
      }

      if (steering > JOYSTICK_DEADZONE) {
        direction += " RIGHT";
      } else if (steering < -JOYSTICK_DEADZONE) {
        direction += " LEFT";
      }

      Serial.print(direction);
      Serial.print(" - Throttle: ");
      Serial.print(throttle);
      Serial.print(" Steering: ");
      Serial.print(steering);
      Serial.print(" - Target L/R: ");
      Serial.print(targetSpeedLeft);
      Serial.print(" / ");
      Serial.println(targetSpeedRight);
    }

  } // End joystick processing
}

//*****************************************************************************
//**************************END ONEVENT HANDLER********************************
//*****************************************************************************
