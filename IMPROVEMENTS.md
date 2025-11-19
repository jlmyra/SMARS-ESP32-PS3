# SMARS ESP32 PS3 Controller - Code Improvements

## Overview
This document details the comprehensive improvements made to the SMARS ESP32 PS3 robot control code. All improvements focus on safety, control quality, and user experience.

---

## 🛡️ Safety Features (Critical)

### 1. Fail-Safe Motor Stop on Disconnect
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:255-270`
- **What it does:** Immediately stops all motors when PS3 controller disconnects
- **Why it matters:** Prevents runaway robot if controller loses connection
- **Implementation:** Calls `emergencyStopMotors()` in main loop when `!Ps3.isConnected()`

### 2. Event Timeout Watchdog
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:273-281`
- **What it does:** Stops motors if no controller events received for 500ms
- **Why it matters:** Protects against frozen controller that stays "connected" but stops sending data
- **Configuration:** Adjust `EVENT_TIMEOUT_MS` constant (default: 500ms)
- **Implementation:** Tracks `lastEventTime` and stops motors if timeout exceeded

### 3. Emergency Stop Button
**Location:** `onEvent.ino:34-50`
- **What it does:** PS button toggles emergency stop mode
- **Why it matters:** Provides immediate panic button to stop all movement
- **Usage:** Press PS button to stop, press again to resume
- **Implementation:** Sets `emergencyStop` flag which blocks all motor commands

---

## ⚡ Motor Control Improvements

### 4. Motor Speed Ramping/Acceleration
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:127-157`
- **What it does:** Smoothly accelerates/decelerates motors instead of instant changes
- **Benefits:**
  - Reduces mechanical stress on drivetrain
  - Prevents wheel slip and improves traction
  - Smoother, more controllable movement
- **Configuration:** Adjust `RAMP_RATE` constant (0.1 = slower, 0.3 = faster, default: 0.15)
- **Implementation:** Uses `targetSpeed` and `currentSpeed` with exponential smoothing

### 5. Exponential Steering Curves
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:120-125`, `onEvent.ino:161-167`
- **What it does:** Applies exponential curve to steering input for better fine control
- **Benefits:**
  - More precise control at low speeds
  - More aggressive turning at high stick deflection
  - Natural, intuitive feel
- **Configuration:** Adjust `STEERING_EXPO` constant (1.0 = linear, 2.0 = squared, 3.0 = cubic)
- **Math:** `output = sign(input) * |input|^EXPO`

### 6. Configurable Dead Zones
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:15`, `onEvent.ino:135-141`
- **What it does:** Ignores small joystick movements within deadzone threshold
- **Benefits:**
  - Compensates for controller drift
  - Prevents unwanted movement from worn joysticks
  - Easy to adjust for different controllers
- **Configuration:** Adjust `JOYSTICK_DEADZONE` constant (default: 5, was hardcoded at 3)

---

## 🎮 New Features

### 7. Speed Limiting Modes
**Location:** `onEvent.ino:59-69`
- **What it does:** Three speed limit modes: 50%, 75%, 100%
- **Benefits:**
  - Better control for beginners
  - Safer in tight spaces
  - Preserves battery when full speed not needed
- **Usage:** Press L1 button to cycle through modes
- **Current mode:** Displayed via telemetry and status messages

### 8. Tank Turn Mode (Pivot in Place)
**Location:** `onEvent.ino:71-80`, `onEvent.ino:93-125`
- **What it does:** Enables in-place rotation by driving tracks in opposite directions
- **Benefits:**
  - Turn in confined spaces
  - Precise positioning
  - More maneuverable
- **Usage:** Press R1 button to toggle tank turn mode
- **Controls:** When active and left stick centered, right stick controls rotation

### 9. Improved Control Scheme
**New differential steering algorithm:**
- **Location:** `onEvent.ino:127-182`
- **Benefits:**
  - Cleaner code structure
  - More predictable behavior
  - Easier to maintain
- **Algorithm:**
  - Calculates base speed from throttle
  - Applies exponential curve to steering
  - Differentially adjusts left/right tracks

---

## 📊 Code Quality & Performance

### 10. Refactored Event Handler
**Location:** `onEvent.ino` (complete rewrite)
- **Before:** 136 lines of nested if-else statements
- **After:** Clean, structured code with clear sections
- **Benefits:**
  - Much easier to understand and modify
  - Reduced code duplication
  - Better separation of concerns

### 11. Conditional Debug Printing
**Location:** All files - wrapped Serial prints with `if (DEBUG_SERIAL)`
- **What it does:** Debug output can be disabled to improve performance
- **Configuration:** Set `DEBUG_SERIAL false` to disable (default: true)
- **Benefits:**
  - Faster response time when debug not needed
  - Cleaner serial output in production
  - Easy to enable for troubleshooting

### 12. Helper Functions
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:96-194`
- **Functions added:**
  - `emergencyStopMotors()` - Centralized motor stop logic
  - `applySteeringCurve()` - Exponential steering calculation
  - `rampMotorSpeeds()` - Smooth motor acceleration
  - `printTelemetry()` - Diagnostic output
- **Benefits:** Code reuse, better organization, easier testing

---

## 📈 Diagnostics & Telemetry

### 13. Real-time Telemetry
**Location:** `ESP32_SMARS_PS3-3-GOLD-With_BAT-2.ino:159-194`
- **What it displays:**
  - Current speed limit mode (50%/75%/100%)
  - Tank turn mode status (ON/OFF)
  - Current motor speeds (left/right)
  - Target motor speeds (left/right)
  - Time since last controller event
  - Main loop frequency (Hz)
- **Configuration:** Displays every 1 second when `DEBUG_SERIAL` enabled
- **Benefits:** Real-time insight into robot behavior for tuning and debugging

### 14. Enhanced Connection Feedback
**Location:** `Connection.ino:4-30`
- **What it does:** Displays control scheme when controller connects
- **Benefits:**
  - User knows controls without reading documentation
  - Confirms successful connection
  - Shows current capabilities

---

## 🔧 Configuration File (settings.h)

All configurable parameters are now in a separate **`settings.h`** file for easy customization!

### Key Advantages:
- ✅ All settings in one place
- ✅ Well-organized and documented
- ✅ Easy to share configurations
- ✅ Can use `settings.example.h` as a template
- ✅ Professional code organization

### Main Configuration Categories:

**Debug & Diagnostics:**
```cpp
#define DEBUG_SERIAL true        // Enable/disable debug output
#define TELEMETRY_INTERVAL_MS 1000  // Telemetry reporting rate
```

**Controller Input:**
```cpp
#define JOYSTICK_DEADZONE 5      // Joystick deadzone threshold
```

**Safety:**
```cpp
#define EVENT_TIMEOUT_MS 500     // Watchdog timeout in milliseconds
```

**Motor Control Tuning:**
```cpp
#define RAMP_RATE 0.15           // Motor acceleration rate (0.05-1.0)
#define STEERING_EXPO 2.0        // Steering curve exponent (1.0-3.0)
#define PWM_FREQUENCY 20000      // PWM frequency in Hz
```

**Speed Limiting:**
```cpp
#define DEFAULT_SPEED_LIMIT_MODE 2  // 0=50%, 1=75%, 2=100%
#define SPEED_LIMIT_MODE_0 0.50     // Customize each mode
#define SPEED_LIMIT_MODE_1 0.75
#define SPEED_LIMIT_MODE_2 1.00
```

**Battery Monitoring:**
```cpp
#define BATTERY_R1 101300        // Voltage divider resistor values
#define BATTERY_R2 42300
#define BATTERY_FULL_VOLTAGE 8.4
#define BATTERY_LOW_VOLTAGE 6.7
```

**Hardware Configuration:**
```cpp
#define PS3_MAC_ADDRESS "b8:27:eb:37:85:b9"  // Your controller MAC
#define MOTOR_LEFT_FWD_PIN 27    // GPIO pin assignments
// ... etc
```

### Tuning Recommendations:
- **RAMP_RATE**: Increase for more responsive acceleration, decrease for smoother
- **STEERING_EXPO**: Increase for more aggressive steering, decrease for more linear
- **JOYSTICK_DEADZONE**: Increase if you have controller drift
- **EVENT_TIMEOUT_MS**: Adjust based on your controller's update rate

### Using settings.h:
1. Copy `settings.example.h` to `settings.h` if needed
2. Edit `settings.h` with your preferences
3. Upload to ESP32
4. Settings take effect immediately

---

## 🎯 Control Summary

### Button Mapping
| Button/Stick | Function |
|--------------|----------|
| Left Stick Y | Forward/Reverse |
| Right Stick X | Left/Right Turn (Normal Mode) |
| Right Stick X | Rotation (Tank Turn Mode) |
| L1 Button | Cycle Speed Limit (50%→75%→100%) |
| R1 Button | Toggle Tank Turn Mode |
| PS Button | Emergency Stop (Toggle) |

---

## ⚠️ Breaking Changes

### Changes from Original Code:
1. **PWM Frequency:** Changed from 30kHz to 20kHz (may reduce motor whine)
2. **Deadzone:** Increased from 3 to 5 (adjustable)
3. **Motor Control:** Complete rewrite - behavior may feel different initially
4. **Acceleration:** Motors now ramp instead of instant response

### Migration Notes:
- Test at 50% speed limit first to get familiar with new behavior
- Adjust `RAMP_RATE` if acceleration feels too slow/fast
- Adjust `STEERING_EXPO` if steering feels too sensitive/insensitive
- Original behavior can be approximated with: `RAMP_RATE=1.0`, `STEERING_EXPO=1.0`

---

## 🧪 Testing Recommendations

Before full operation:
1. ✅ Test emergency stop (PS button) - ensure motors stop immediately
2. ✅ Test disconnect safety - disconnect controller, verify motors stop
3. ✅ Test at 50% speed limit first
4. ✅ Verify all directions (forward, reverse, left, right)
5. ✅ Test tank turn mode
6. ✅ Test speed ramping feels smooth
7. ✅ Verify battery monitoring still works
8. ✅ Check serial output for any errors

---

## 📝 Future Enhancement Ideas

Potential additions (not yet implemented):
- [ ] Configurable motor speed curves per track (for motors with different characteristics)
- [ ] Trim adjustment (if robot drifts to one side)
- [ ] Multiple ramping profiles (sport/comfort/eco)
- [ ] Button to reverse controls (for when robot is facing you)
- [ ] Low battery warning via controller LED
- [ ] Data logging to SD card
- [ ] WiFi telemetry and tuning interface
- [ ] Obstacle detection integration
- [ ] Auto-stop on excessive motor current (stall detection)

---

## 🙏 Credits

Original SMARS project by Kevin McAleer (https://www.smarsfan.com/)
ESP32 PS3 Library by Jeffrey van Pernis (https://github.com/jvpernis/esp32-ps3)

Improvements by: Claude Code (Anthropic)
Date: 2025-11-19

---

## 📄 License

This code maintains the same license as the original SMARS ESP32 PS3 project.
