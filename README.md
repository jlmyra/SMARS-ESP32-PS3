![pic two](https://github.com/jlmyra/SMARS-ESP32-PS3/blob/main/IMG_9174.jpeg =500X500)
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
- 425K resistor
- 680K resistor
- 0.1uf capacitor
- 5V, 3A BEC/UBEC (battery elimination circuit) - https://tinyurl.com/y3t4wqus (~$4.00) or https://tinyurl.com/y2z84n8p
- 2 Cell 18650 battery holder
- Wire

### Software Implementation:
- Set-up ESP32 in Arduino IDE (for help see: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/)
- Install PS3 Library in Arduino IDE https://github.com/jvpernis/esp32-ps3 (for help see: https://techtutorialsx.com/2020/02/16/esp32-connecting-a-ps3-controller/). Follow the instructions at either link for setting or determining the MAC address of your PS3 controller. 
- Description: The sketch is presented in a 'tabbed' format for ease of editing. The files are:
   - ESP32_SMARS_PS3-3-GOLD.ino contains the variables, setup, and loop. Enter your PS3 MAC Address on Line 70 in void Setup().
   - Battery_Check.ino is a function that samples the battery voltage and reports it on pin 32. It is commented out in the main Loop and does not need to be used.
   - Connection.ino is a function that looks for the bluetooth connection between the ESP32 and the PS3 controller.
   - onEvent.ino is a function that listens for changes occuring on the PS3 controller and converts them to motor actions for FORWARD, REVERSE, LEFT-RIGHT turns.
   - Citations.ino includes the license, useful links and recognition to those that contributed to this by sharing their wisdom and code.
These five files need to be downloaded into a folder for the Arduino IDE to access.
   
There is also a circuit and code for monitoring the charge state of the battery in the vehicle. There is a tap on the + side of the battery that goes to a voltage divider. The output of the voltage divider is read on GPIO32 an ADC pin. The charge state of the battery is displayed on the controller LEDS. 4 LEDS - full charge, 3 LEDS 75%, 2 LEDS 50%, and 1 LED- time to recharge. When the voltage goes below 7 volts for 15 seconds the controller will begin vibrating to remind the operator to recharge the battery. You can explore voltage dividers here - https://www.mischianti.org/2019/06/15/voltage-divider-calculator-and-application/ . The voltage divider can be omitted from your design. The function call is commented out in the main program loop. If you wish to use it you will need to uncomment it.

### Circuit Diagram

![pic one](https://github.com/jlmyra/SMARS-ESP32-PS3/blob/main/SMARS_ESP32_Wiring_Diagram.png)


   
### Final Thoughts
I am grateful to Kevin McAleer for sharing the SMARS project with us. Thanks!
