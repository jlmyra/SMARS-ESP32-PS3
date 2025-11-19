//*************************Battery Voltage Diagnostics*************************
//
// This file contains diagnostic functions to help calibrate battery voltage
// readings. Enable these by calling from the main loop.
//
//*******************************************************************************

// Print detailed battery voltage diagnostics
void printBatteryDiagnostics() {
  static unsigned long lastDiagTime = 0;
  unsigned long currentTime = millis();

  // Print diagnostics every 2 seconds
  if (currentTime - lastDiagTime >= 2000) {
    lastDiagTime = currentTime;

    Serial.println("\n========== BATTERY VOLTAGE DIAGNOSTICS ==========");

    // Raw ADC reading
    int rawADC = analogRead(batPin);
    Serial.print("Raw ADC Value: ");
    Serial.println(rawADC);

    // Calibrated mV reading
    uint32_t milliVolts = readADC_Cal(rawADC);
    Serial.print("Calibrated ADC (mV): ");
    Serial.println(milliVolts);

    // Convert to volts
    float voltsAtPin = milliVolts * 0.001;
    Serial.print("Voltage at ADC Pin (V): ");
    Serial.println(voltsAtPin, 4);

    // Calculate expected battery voltage from voltage divider
    // Battery voltage = ADC voltage * (R1 + R2) / R2
    float calculatedBatteryVoltage = voltsAtPin * (R1 + R2) / R2;
    Serial.print("Calculated Battery Voltage (no correction): ");
    Serial.println(calculatedBatteryVoltage, 3);

    // With voltage correction
    float correctedVoltage = (voltsAtPin - BATTERY_VOLTAGE_CORRECTION) * (R1 + R2) / R2;
    Serial.print("With -0.07V correction: ");
    Serial.println(correctedVoltage, 3);

    float correctedVoltage2 = (voltsAtPin + BATTERY_VOLTAGE_CORRECTION) * (R1 + R2) / R2;
    Serial.print("With +0.07V correction: ");
    Serial.println(correctedVoltage2, 3);

    // Voltage divider calculations
    Serial.print("\nVoltage Divider Ratio (R2/(R1+R2)): ");
    Serial.println((float)R2 / (R1 + R2), 6);

    Serial.print("Voltage Multiplier ((R1+R2)/R2): ");
    Serial.println((float)(R1 + R2) / R2, 6);

    Serial.print("Expected ADC voltage at 8.4V battery: ");
    float expectedVout = BATTERY_FULL_VOLTAGE * R2 / (R1 + R2);
    Serial.print(expectedVout, 4);
    Serial.println(" V");

    // Current averaged reading
    if (analogReadCounter > 0) {
      Serial.print("\nCurrent average (");
      Serial.print(analogReadCounter);
      Serial.print(" samples): ");
      Serial.print(batteryVoltageSum / analogReadCounter, 4);
      Serial.println(" V (at ADC pin)");
    }

    Serial.print("Last batteryVoltageAvg: ");
    Serial.print(batteryVoltageAvg, 4);
    Serial.println(" V");

    Serial.print("Last batteryVoltageCorr: ");
    Serial.print(batteryVoltageCorr, 3);
    Serial.println(" V");

    Serial.println("\n** MEASURE YOUR BATTERY WITH A MULTIMETER **");
    Serial.println("** THEN ADJUST SETTINGS IN settings.h **");
    Serial.println("=================================================\n");
  }
}

// Simple battery voltage reading without averaging (for quick diagnostics)
float getInstantBatteryVoltage() {
  int rawADC = analogRead(batPin);
  uint32_t milliVolts = readADC_Cal(rawADC);
  float voltsAtPin = milliVolts * 0.001;

  // Simple calculation: ADC voltage * divider ratio
  return voltsAtPin * (R1 + R2) / R2;
}

//******************END Battery Voltage Diagnostics*************************
