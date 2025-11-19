
//*******************Check controller Connection********************************

void onConnection() {

  if (Ps3.isConnected()) {
    if (DEBUG_SERIAL) {
      Serial.println("=====================================================");
      Serial.println("            PS3 Controller Connected!");
      Serial.println("=====================================================");
      Serial.println("CONTROLS:");
      Serial.println("  Left Stick Y-axis  : Forward/Reverse");
      Serial.println("  Right Stick X-axis : Left/Right Turn");
      Serial.println("  L1 Button         : Cycle Speed Limit (50%/75%/100%)");
      Serial.println("  R1 Button         : Toggle Tank Turn Mode");
      Serial.println("  PS Button         : Emergency Stop");
      Serial.println("=====================================================");
      Serial.println();
    }

    // Reset emergency stop on new connection
    emergencyStop = false;

    // Initialize last event time
    lastEventTime = millis();

    // Stop motors on connection (safety)
    emergencyStopMotors();
  }
}

//*******************END Check controller Connection*******************************
