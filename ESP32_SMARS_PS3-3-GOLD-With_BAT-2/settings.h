#ifndef SETTINGS_H
#define SETTINGS_H

//*****************************************************************************
//*********************  SMARS ESP32 PS3 CONFIGURATION  ***********************
//*****************************************************************************
//
// This file contains all user-configurable parameters for the SMARS robot.
// Adjust these values to tune the robot's behavior to your preferences.
//
//*****************************************************************************

//=============================================================================
// DEBUG & DIAGNOSTICS
//=============================================================================
// Set to false to disable serial debug output for better performance
#define DEBUG_SERIAL true

// Telemetry reporting interval in milliseconds (0 to disable)
#define TELEMETRY_INTERVAL_MS 1000

//=============================================================================
// CONTROLLER INPUT SETTINGS
//=============================================================================
// Joystick deadzone - increase if controller drifts (range: 0-20, default: 5)
#define JOYSTICK_DEADZONE 5

//=============================================================================
// SAFETY FEATURES
//=============================================================================
// Watchdog timeout - stops motors if no events received (milliseconds)
// Recommended: 300-1000ms depending on controller update rate
#define EVENT_TIMEOUT_MS 500

//=============================================================================
// MOTOR CONTROL TUNING
//=============================================================================
// Motor acceleration/deceleration rate
// Lower = smoother but slower response (range: 0.05-1.0, default: 0.15)
// 0.05 = very smooth/slow, 0.15 = balanced, 0.30 = aggressive, 1.0 = instant
#define RAMP_RATE 0.15

// Steering exponential curve for fine control
// Higher = more sensitive near center (range: 1.0-3.0, default: 2.0)
// 1.0 = linear, 2.0 = squared (balanced), 3.0 = cubic (very sensitive center)
#define STEERING_EXPO 2.0

//=============================================================================
// PWM SETTINGS
//=============================================================================
// PWM frequency in Hz - adjust if motors whine
// 15000-25000 Hz is typical, 20000 Hz recommended
#define PWM_FREQUENCY 20000

// PWM resolution in bits (8 = 0-255, don't change unless you know what you're doing)
#define PWM_RESOLUTION 8

//=============================================================================
// MOTOR PIN ASSIGNMENTS (MH-ET ESP32 + L9110S Motor Driver)
//=============================================================================
#define MOTOR_LEFT_FWD_PIN   27  // GPIO27 - A1B - Left Motor Forward
#define MOTOR_LEFT_REV_PIN   25  // GPIO25 - A1A - Left Motor Reverse
#define MOTOR_RIGHT_FWD_PIN  18  // GPIO18 - B1B - Right Motor Forward
#define MOTOR_RIGHT_REV_PIN  26  // GPIO26 - B1A - Right Motor Reverse

//=============================================================================
// PWM CHANNEL ASSIGNMENTS
//=============================================================================
#define MOTOR_LEFT_FWD_CHANNEL   1  // PWM Channel for left forward
#define MOTOR_LEFT_REV_CHANNEL   0  // PWM Channel for left reverse
#define MOTOR_RIGHT_FWD_CHANNEL  3  // PWM Channel for right forward
#define MOTOR_RIGHT_REV_CHANNEL  2  // PWM Channel for right reverse

//=============================================================================
// SPEED LIMITING
//=============================================================================
// Default speed limit mode on startup (0=50%, 1=75%, 2=100%)
#define DEFAULT_SPEED_LIMIT_MODE 2

// Speed limit percentages for each mode (modify if you want different limits)
#define SPEED_LIMIT_MODE_0 0.50  // 50% - Beginner/tight spaces
#define SPEED_LIMIT_MODE_1 0.75  // 75% - Normal operation
#define SPEED_LIMIT_MODE_2 1.00  // 100% - Full speed

//=============================================================================
// BATTERY MONITORING
//=============================================================================
// Enable/disable battery voltage monitoring
// Set to false if you don't have the voltage divider circuit installed
#define ENABLE_BATTERY_MONITORING false  // Set to true to enable battery monitoring

// Battery voltage divider resistor values (in ohms)
// Original settings from your commented-out defines:
//   #define setR1 101300;
//   #define setR2 42300;
//   #define set_voltageCorrection 0.07;
#define BATTERY_R1 101300  // Resistor R1 (high side)
#define BATTERY_R2 42300   // Resistor R2 (low side)

// Voltage correction offset to match voltmeter readings
// This value is ADDED to the calculated voltage: batteryVoltage = calculated + BATTERY_VOLTAGE_CORRECTION
// - Use POSITIVE values if readings are too LOW (most common)
// - Use NEGATIVE values if readings are too HIGH
// - Use 0.0 if readings match multimeter
// - Adjust in small increments (0.1V) until serial output matches your multimeter
#define BATTERY_VOLTAGE_CORRECTION 0.0  // Start at 0.0, then calibrate (was 0.07)

// Battery voltage thresholds (2S LiPo, 2x 18650)
#define BATTERY_FULL_VOLTAGE 8.4   // Fully charged voltage
#define BATTERY_LOW_VOLTAGE  6.7   // Time to recharge (80% of full = 3.35V/cell)

// ADC pin for battery monitoring
#define BATTERY_ADC_PIN 32  // GPIO32 (ADC1_CH4)

// Battery voltage reading parameters
#define BATTERY_READ_INTERVAL_MS 2      // How often to read ADC (milliseconds)
#define BATTERY_AVERAGE_SAMPLES 500     // Number of samples to average

//=============================================================================
// PS3 CONTROLLER MAC ADDRESS
//=============================================================================
// Enter your PS3 controller MAC address here
// Find it using the PS3 library examples or SixaxisPairTool
#define PS3_MAC_ADDRESS "b8:27:eb:37:85:b9"

//=============================================================================
// SERIAL BAUD RATE
//=============================================================================
#define SERIAL_BAUD_RATE 250000

//=============================================================================
// ADVANCED TUNING (experts only)
//=============================================================================
// Minimum motor speed threshold (0-255) - motors won't run below this
// Use to compensate for motor stall at very low PWM values
#define MOTOR_MIN_SPEED 0

// Maximum motor speed limit (0-255) - cap maximum speed if needed
#define MOTOR_MAX_SPEED 255

// Tank turn speed multiplier (0.5-1.0) - reduce if tank turns too aggressive
#define TANK_TURN_SPEED_FACTOR 1.0

//*****************************************************************************
//********************  END CONFIGURATION  ************************************
//*****************************************************************************

#endif // SETTINGS_H
