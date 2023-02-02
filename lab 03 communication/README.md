# Lab 3 is currently broken!!



# Lab 3: communication

In this lab you will build FlatSAT's communication system including the onboard radio, antenna, and ground station, to ensure FlatSAT has sufficient link margin to communicate from orbit. You will test communication and measure signal strength at multiple link distances to characterize the link budget. In your final lab report, you will need to compare your prelab predictions to your experimental results. 

## documentation

- Arduino MKR Zero pinout
- `lab 03 communication.ino`

## software

- Arduino IDE
  - `lab 03 communication.ino`

- Arduino libraries (install by running `install_libraries.bat`)
  - RTCZero
- Thonny IDE

## hardware

* ESD-safe grounding straps
* FlatSAT
  * INA219 current sensor
  * ArduCAM-M-2MP

* Digi XBee (small black board atop a small red board)
* SparkFun XBee explorer dongle (USB)
* 3 antennas (2 small, 1 large)
* 3 LEDs
* cables and wires



![image-20230114103713337](C:\Users\jordan\AppData\Roaming\Typora\typora-user-images\image-20230114103713337.png)



## setup

Whenever you handle the Arduino or any microcontroller electronics, be sure that you have a grounding strap on, to prevent unintentional electro-static discharge (ESD). The strap should have contact with your skin and the banana plug end should plug into one of the grounding holes (indicated in red) on the front of your lab bench. There are two grounding plugs at each lab station.

In this lab FlatSAT will be powered by a 3-cell Lithium Ion battery via a 5V BEC, as it was in the electrical power lab. However, during the initial steps including code upload, FlatSAT will be powered via USB. 

- connect power and ground lines
  - Top rail: 3.3 V (diagram: orange wires)
    - 3.3 V supply comes from VCC pin of Arduino
  
- connect XBee radio

<img src="sources\comm_bb.svg" alt="solar_bb"  />

### XBee breakout

The XBee radio communicates with Arduino using serial communication. Serial communication uses two wires: transmit and receive. 

![XBee_explorer](sources/XBee_explorer.png)

Connect serial lines

- XBee 3 (DOUT) -> Arduino D13 (RX)
- XBee 4 (DIN) -> Arduino D14 (TX) 


ArduCAM-M-2MP communicates with Arduino using both I2C and SPI. The Arduino sketch handles these protocols with the `Wire` and `SPI` libraries. I2C: sensor configuration. SPI: camera commands and data stream (images). 

Note the orientation of the connections in the diagram: the camera board should be vertical with the sensor looking over the LCD. 

Connect power

- VCC (5 V)
- ground (any ground is fine)

Connect I2C comm lines--these are already in place from lab 1

- SDA
- SCL

Connect SPI comm lines

- SCK (clock)
- CIPO (controller in/peripheral out)*
- COPI (controller out/peripheral in)*
- CS (chip select)

**\*Note**: some components and documentation use master/slave terminology (MISO/MOSI), and some use controller/peripheral (CIPO/COPI). MISO = CIPO, MOSI = COPI. 



### Prepare for communication testing

For communication testing, FlatSAT will be powered by a 3-cell 18650 Lithium ion battery with the cells connected in series, providing a nominal 11.1 V. The input pin of an Arduino MKR Zero (or MKR 1000) can tolerate 5-5.5 V. You will use a switching regulator called a BEC to step the supply voltage down from 12 V to 5 V. 

Arduino will further step the 5 V down to 3.3 V for its internal logic. 3.3 V out is also available on the Vcc pin to power peripherals. 

Arduino pins: 

- Vin: 5 V power input
- 5V: 5 V power output (for some peripherals)
- Vcc: 3.3 V power output
- Gnd

**Note**: applying 12 V directly to Vin will break your Arduino. 

- Velcro the battery holder to back of breadboard. 
- Connect 3-cell 18650 Li-ion holder to posts on breadboard holder. 

- Using banana plugs, connect the battery holder wires to the posts on your breadboard
  - Red: positive
  - Black: negative
- Connect the BEC 
  - 12 V side connects to breadboard posts
  - 5 V side connects to FlatSAT
    - Red: Vin
    - Black: ground rail
- **Have your instructor check your connections**







## Part 2 – Opening up Arduino IDE

- Open `lab 03 communication.ino` in Arduino IDE
- From the tools menu, select the correct board (MKR 1000) and port
- Note the number written on tape on the bottom of your XBee module—a 2-digit number between 31 and 35. Record this number in your lab notebook. 
- Check the back of your XBee USB dongle—it should be the same. 
- In `lab 03 communication.ino` you will see a line that says “#define PAN_ID “3331””. Ensure that the last two digits in the PAN_ID match the two digits that were on the tape on the bottom of your XBees. In this screenshot, the matching number on the FlatSat would be “32.” If the PAN_ID does not match the number on your FlatSat module, modify the last two digits to match the number on your FlatSat module. This step syncs your radios so that they won’t interfere with the radios of other lab groups.

![img](sources/clip_image008.jpg)

- Compile (verify) your file. 
- Upload the file to the Arduino. You should see the LED labelled “L” on the Arduino board blink three times in rapid succession when the upload completes. You will also get a message in the black dialog box at the bottom of the IDE window. If you have any errors uploading, double check that you have the correct board and port selected.
- Gently disconnect the USB cable from the Arduino and disconnect the USB end of the cable from the laptop.

 

## Part 3 – Setting up the Communication

- Set up the ground station antenna. Pull out the XBee USB dongle (looks like a red USB stick). It should have a black Digi XBee Radio module mounted on the top that looks just like the shield mounted on top of the XBee Explorer board. That is the radio module. Carefully screw in one small antenna to your ground radio (XBee USB Dongle) and the other small antenna to your FlatSat radio. 

![img](sources/clip_image002.jpg)

- Connect your ground radio (XBee USB Dongle) to the lab station PC. Open Device Manager on the lab station PC, and click on “Ports.” Note which port number is assigned to your ground antenna (there should only be one active port, as you should have already disconnected the USB to the Arduino).

![img](sources/clip_image004-1675315164882-5.jpg)

- Open File Explorer. Navigate to Libraries > Documents > cube_sat !!. Find a file called “config.json” and double-click on it.

![img](sources/clip_image006-1675315164882-6.jpg)

- Within the Config file, at the top, you will see a section called “radio_config.” Modify the “port” name to match what you saw in Device Manager for your ground antenna. Then modify the “pan” number to match the two-digit number written on the tape on the bottom of your ground antenna and your FlatSat Module 1. Save your changes and close the Config file.

![img](sources/clip_image008-1675315164882-7.jpg)

##  Part 4 – Connecting the FlatSat Module to the second laptop

- Carefully connect the USB cable to the Arduino MKR 1000 and a USB port on a second laptop (either your own cadet one or an extra lab station laptop). You should see a green LED light up on the board, indicating it has power.

- Open Arduino IDE 
- From the tools menu, select the correct board (MKR 1000) and port

- Open the serial monitor (tools menu)

![img](sources/clip_image004.gif)

- A dialog box should open with the number of your COM port at the top. This helps with debugging error messages from the Arduino. Ensure that the “Autoscroll” and “Show timestamp” boxes are checked.

![img](sources/clip_image006-1675315331939-13.jpg)

- You should start to see messages from Arduino. If they end with “ Received: OK and 0 0” then you are good to go. If the messages continue to autoscroll without stopping, you will need to disconnect your USB cable from your laptop and close the serial monitor. Then you will need to re-connect the USB cable to your laptop and re-open the serial monitor. If you do not get the “OK” message in the serial monitor the second time, double-check that you connected all of the wires correctly.

## Part 5 – Sending and Receiving Data

- We will now be using a simulated ground station software, written in Python, to communicate from the ground station radio (connected to your laptop) to your FlatSat (the breadboard radio). Open Thonny on the lab station PC.

![img](sources/clip_image002-1675315509767-20.jpg)

- Click on the “Open” menu, then navigate to the following folder Libraries > Documents > cube_sat. Select “main.py”!!  and click “Open.”

![img](sources/clip_image004-1675315509766-17.jpg)

- Click the Green triangle button just below the “View” tab along the top left. This will run the main Python script that is managing the communication between the ground station radio and the FlatSat radio.

![img](sources/clip_image006-1675315509766-18.jpg)

- A pop-up window called “Cube Sat Response” should appear. Click on the teal Arduino icon on the top-left to open communications between the ground radio and the FlatSat radio.

![img](sources/clip_image008-1675315509766-19.jpg)

- You should see a message pop up saying “Attempting to connect to OSCAR-12.” Then, a second or so later you should see another message saying “OSCAR-12 Command Port Ready.” Your ground radio and FlatSat radio are now communicating! If you receive an error message, click on the red “X” icon to close the communication attempt. Wait a second or two, then click on the teal Arduino icon again. Sometimes this connection can be finicky, and you’ll need to attempt it more than once.

![img](sources/clip_image013.jpg)

- Within the “Cube Sat Response” dialog box, click on the “Help” menu item. A dialog box will open showing all of the possible commands you can send. Today, we are interested in determining the received signal strength (RSSI). There are two different ways to measure it: the RSSI of the FlatSat antenna and the RSSI of the ground antenna. For your lab report, you need to determine the RSSI of the ground antenna (that is the received signal strength on the ground from the FlatSat). That command is “get_rssi.”

![img](sources/clip_image015.jpg)

- Close the “Help” menu. Type `get_rssi` into the command line prompt and click “Transmit.”

- You should get a value back in dBm (decibel-milliWatts). Click on the red “X” icon to disconnect communications. 

![img](sources/clip_image017.jpg)

- Now, you are ready to set up your FlatSat Module at the pre-determined distances and take measurements of the signal strength. To appropriately space out the two antennas to perform your data collection, you will be moving your second !! laptop connected to the FlatSat module and leaving the ground antenna (XBee USB Dongle) connected to your lab station laptop. Using the measuring tape, align the laptop such that the FlatSat antenna is 0.5 meters away from the ground antenna. 

- Click on the teal Arduino icon on the top-left to open communications between the ground radio and the FlatSat radio. As before, you should see a message pop up saying “Attempting to connect to OSCAR-12.” Then, a second or so later you should see another message saying “OSCAR-12 Command Port Ready.”

- Within the “Cube Sat Response” dialog box, type in `get_rssi` and click “Transmit.” Record the RSSI value in your notes for use in your final lab report. Don’t forget that the value you record is in dBm, and you’ll need to convert it to dB to compare with your predictions! Click on the Red “X” icon to disconnect communications after taking each measurement.

- Repeat steps 9-12 above for distances of 1 meter, 2 meters, and 5 meters. Note: If you get the same value for “get_rssi” for any of the distances, ensure that you have closed communications and re-opened them between each data collection. You should have different values of RSSI for each distance.

**Note: If you do not record these RSSI values at each distance, you will have to repeat the lab. Don’t forget to write them down!**

- Try changing out your small antenna on your ground station antenna (the small red USB stick) with the larger antenna – representing that ground stations typically have much higher gain than the spacecraft. Gently disconnect the XBee USB Dongle from the laptop before you unscrew the small antenna and screw in the large antenna. The large antenna has a gain of 5 dB – see if you notice the difference in the RSSI values. Try taking a measurement at one of the distances you already did (0.5, 1, 2 or 5 m) and compare your results.







## send data

- You may be as far or as close as you would like for these next few steps.

- Within the case at your lab station, you should have more wires and 3 LEDs. On the Arduino on your FlatSat module, connect an LED from Digital Pin 3 (D3) to the black ground rail on the breadboard. Ensure that the short leg, indicated with the indent on the LED is the pin going to ground. Repeat this with another LED from Digital Pin 4 (D4) to the black ground rail and another LED from Digital Pin 5 (D5) to the black ground rail.

- Click on the teal Arduino icon on the top-left to open communications between the ground radio and the FlatSat radio. As before, you should see a message pop up saying “Attempting to connect to OSCAR-12.” Then, a second or so later you should see another message saying “OSCAR-12 Command Port Ready.”

- Within the “Cube Sat Response” dialog box, type in the command “turn_on_pin_3”. You should see the LED connected to Digital Pin 3 light up. Then type in the command “turn_off_pin_3”. You should see the LED connected to Digital Pin 3 turn off. 

- Repeat Step 15 for Digital Pin 4 and Digital Pin 5. Try this at various distances around the lab to see how far your radio link will allow you to send commands from. Try walking out into the hallway and see how far you can get and still receive commands. (The measuring tape is only 30’, so you’ll have to approximate distances.) You will need to comment on how far you are able to transmit and receive in your final lab report.

- Try to take an image with the ArduCAM and send it back over the radio link to the ground station (lab station laptop). Using your cell phone or watch, time how long it takes from the time you send the command to take an image until you receive it (it should pop open) on the ground station (lab station laptop). The command for that is “get_picture”. Recall from our Payloads lesson that it could take a while if the data rate is not very high. If you are able to receive an image, save the image to include the image in your final lab report. If you don’t receive anything after five minutes, you may close communications by click on the red “x” icon. Whether you receive an image or not, comment on how long it took and what that means for the FlatSat communications design.









# leftovers from camera lab



You will use your camera to record images at the same distances you predicted in the prelab (except 500 km). 

- Position the camera and resolution chart so that they are perpendicular at the same height and 0.5 m apart. Ensure the chart is illuminated evenly and both objects are sufficiently supported to remain still during image capture. 
- Set the resolution chart vertically
- Click “Capture” to take the image. You may have to wait a few seconds for the data to transfer back to the computer. Once it has completed, you should get a message in the GUI and should be able to see a jpeg file in the location you specified. Double-click on the image, to ensure a satisfactory image. Re-take if necessary. 
- Once you are satisfied, rename the image with the distance in the filename. 
- Repeat this process for the distances calculated in your prelab (1, 2, and 5 m).

## Lab station cleanup

- Transfer all captured images to your group's storage location
- Carefully disconnect the micro-USB to USB cable from your laptop and the Arduino microcontroller
- Remove the ESD wrist straps and replace them in the bag at your lab station.
- Replace all items at your lab station the way you found them. 
- Close the GUI, close the Arduino IDE, and log out of the laptop.
- Have your instructor check off your lab station before you depart.

## Data Reduction (after the lab)

To determine the camera's resolution for each distance, find the point where you can distinguish each individual line. Refer to the scale you recorded during your lab. If the smallest lines you can resolve are 2/3 mm wide, your camera's resolution is 0.667 mm. 

Include the measured resolution for each distance (0.5 m, 1 m, 2 m, and 5 m) in the results section of your team’s final lab report. Is your resolution different in the corners vs the center of the image? If so, discuss possible causes. 
