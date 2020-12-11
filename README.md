<img src="https://user-images.githubusercontent.com/19330858/101283882-461a5c00-37ab-11eb-8002-d988d48a8043.jpeg" width="35%"></img> <img src="https://user-images.githubusercontent.com/19330858/101283896-5c281c80-37ab-11eb-9f04-f05337873b00.jpeg" width="35%"></img> 
# SMARS-ESP32-PS3
SMARS bot with ESP32 using PS3 controller

This is an adaptation of Kevin McAleer's (https://www.smarsfan.com/) SMARS project. In this version I have lengthened the chassis by one track segment length so that 2 - 18650 batteries fit in the bottom of the chassis just above the motors. Instead of an Arduino microcontroller an MH-ET ESP32 microcontroller is used. I chose this one for its compact size as well as the built in bluetooth and wifi eliminating the need for additional hardware. The ESP32 - PS3 Controller communication is via bluetooth. Jeffrey van Pernis has created an excellent library for PS3 code (See-https://github.com/jvpernis/esp32-ps3). It is available on Github or through the Arduino IDE.
If you are familiar with Arduino evolving to ESP32 should be fairly easy. The Arduino IDE works well with the ESP32 with a few modifications. I have posted links below for further information. The most challenging step in this was setting the PS3 controller MAC address. The biggest difference you will find in coding the ESP32 is that the digitalWrite() and analogWrite() functions are replaced by ledcWrite(). There are a few more items that need to be setup to get PWM working. There is a good resource for this here - https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
I have included the code in a 'tabbed' format and files (.stl's) for the modified chassis, mounting plate, dome and a wiring diagram. 

### ESP32 - PS3 Controller Parts:
- MH-ET ESP32 Microcontroller - https://acrobotic.com/collections/esp8266/products/acr-00024 ($6.95)
- HiLetGo L9110S DC Motor Controller - https://tinyurl.com/y57pky5p ($1.50)
- PS3 Controller - Amazon : https://tinyurl.com/y3ootahd ($19.90)
- 2.54mm pitch Screw Terminal Blocks - https://tinyurl.com/yxtv7g5w (~$15 for 50)
- 30K resistor
- 68K resistor
- 0.1uf capacitor
- 5V, 3A BEC/UBEC (battery elimination circuit) - https://tinyurl.com/y3t4wqus (~$4.00) or https://tinyurl.com/y2z84n8p
- 2 Cell 18650 battery holder
- Wire

### Software Implementation:
- Set-up ESP32 in Arduino IDE (for help see: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/)
- Install PS3 Library in Arduino IDE https://github.com/jvpernis/esp32-ps3 (for help see: https://techtutorialsx.com/2020/02/16/esp32-connecting-a-ps3-controller/). Follow the instructions at either link for setting or determining the MAC address of your PS3 controller. 
- Description: The sketch is presented in a 'tabbed' format for ease of editing. The files are:
   - **ESP32_SMARS_PS3-3-GOLD.ino** contains the variables, setup, and loop. Enter your PS3 MAC Address on Line 70 in void Setup().
   - **Battery_Check.ino** is a function that samples the battery voltage and reports it on pin 32. It is commented out in the main Loop and does not need to be used.
   - **Connection.ino** is a function that looks for the bluetooth connection between the ESP32 and the PS3 controller.
   - **onEvent.ino** is a function that listens for changes occuring on the PS3 controller and converts them to motor actions for FORWARD, REVERSE, LEFT-RIGHT turns.
   - **Citations.ino** includes the license, useful links and recognition to those that contributed to this by sharing their wisdom and code.
These five files need to be downloaded into a folder for the Arduino IDE to access.
   
- There is also a circuit and code for monitoring the charge state of the battery in the vehicle. There is a tap on the + side of the battery that goes to a voltage divider. The output of the voltage divider is read on GPIO32 an ADC pin. The charge state of the battery is displayed on the controller LEDS. 4 LEDS - full charge, 3 LEDS 75%, 2 LEDS 50%, and 1 LED- time to recharge. When the voltage goes below 7 volts for 15 seconds the controller will begin vibrating to remind the operator to recharge the battery. You can explore voltage dividers here - https://www.mischianti.org/2019/06/15/voltage-divider-calculator-and-application/ . The voltage divider can be omitted from your design. The function call is commented out in the main program loop. If you wish to use it you will need to uncomment it.
- I use cheap knockoff PS3 controllers. I have not tried an authentic PS3 controller but hopefully they provide more accuracy than the knockoffs. One of my knockoffs does not zero out the joysticks in the 'neutral' position and the range is not balanced up - down and left - right. JVPernis provides sketches in his library that makes is easy to see the controller performance on the serial monitor.

### Circuit Diagram

![pic one](https://github.com/jlmyra/SMARS-ESP32-PS3/blob/main/SMARS_ESP32_Wiring_Diagram.png)

### Wiring
<img src="https://user-images.githubusercontent.com/19330858/101287278-4f142900-37bd-11eb-98c9-aa06b67d0906.jpeg" width="15%"></img> <img src="https://user-images.githubusercontent.com/19330858/101287284-54717380-37bd-11eb-911c-3e1199b6ab27.jpeg" width="15%"></img> <img src="https://user-images.githubusercontent.com/19330858/101287286-5a675480-37bd-11eb-9305-bae63bd2e4bf.jpeg" width="15%"></img> 

 - I used 2.54mm pitch screw terminal blocks for making circuit board connections instead of soldering wires directly to the circuit boards. This makes it easy to fix wiring issues (mistakes!). It also makes it easy to reuse the boards in other applications.
 - I offer suggestions for 2 different BEC's necessary to reduce the battery voltage from ~8V to 5V for the ESP32. The second one in the parts list is much smaller an preferable over the other. Any device that will reduce the battery voltage to 5V can be used.
 - The L9110s motor controller was selected as it was cheap and easy to get. Any dual channel controller can be used. The L9110S comes with a 6 pin terminal block on one end. I have had mixed success removing the pins and soldering in 2.54 pitch screw terminals. I now solder the screw terminals to the pins instead. It's functional but not very neat.

### Chassis
- I have included .stl's for the modified (elongated) chassis, ESP32/L9110S mounting place and dome. 
- The elongated chassis is based on a four motor design. With this I use the adjustable wheel modification found here - https://www.thingiverse.com/thing:3164589
- The printed dome fits better if printed 'right-side-up' with supports. When printed upside down - the flat side on the print bed it will likely have a loose fit. 

### ESP32 Battery Charge Monitor
(see - https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/#:~:text=The%20ESP32%20DEVKIT%20V1%20DOIT,use%20the%20analogRead()%20function)

Battery monitor circuits for the ESP32 are a challenge since the ADC is nonlinear particularly at the ends of its range.. Between 2.6V and 3.3V the ADC will output similar values. The same happens at the low end of the range as well. In monitoring a pair of 18650's A small voltage range is needed to determine the stae of the battery charge. LIPO's should be recharged when they reach 80% of their full charge voltage or 80% of 8.4V = 6.7V. This narrow range makes it easy
to use a linear portion of the ADC curve. The ESP32 ADC resolution is adjustable. It can be a value between 9 (0 – 511) and 12 bits (0 – 4095). Default is 
12-bit resolution. I used 10 bit resolution 0-1024 as it was suggested that the response is less 'noisy' (see - https://esp32.com/viewtopic.php?f=12&t=1045)

A voltage divider is necessary to reduce 8.4V battery voltage to something the ADC can use. Given the nonlinearity of the ADC I chose resistors R1-68K and R2-30K to give ~2.5V output at 8.4V. A 0.1uf capacitor is connected across Vout and ground to clean up the signal.  
(see - https://www.mischianti.org/2019/06/15/voltage-divider-calculator-and-application/) 

To smooth out the ADC reading the ADC is read every 2ms, 500 times with computing an average every second. This may be overkill but it's what I chose to do.

The ESP32 communicates the charge status of the battery by lighting the 'Player' "LEDs on the side panel of the PS3 controller. 4 LEDs = 75-100%, 3 LEDs = 50-74%, 2 LEDs = 25-49%, 1 LED < 24%. At 0% or ~6.7V the vibration motors are turned on in the PS3 controller alerting the operator to recharge the batteries.

### B
### Final Thoughts
I am grateful to Kevin McAleer for sharing the SMARS project with us. Thanks!
