# Lab 3 is currently broken!!



# Lab 3: communication

In this lab you will build FlatSAT's communication system including the onboard radio, antenna, and ground station, to ensure FlatSAT has sufficient link margin to communicate from orbit. You will test communication and measure signal strength at multiple link distances to characterize the link budget. In your final lab report, you will need to compare your prelab predictions to your experimental results. 

## documentation

- Arduino MKR Zero pinout
- `lab 03 communication.ino`

## software

- Arduino IDE
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

![XBee_explorer](sources/README/XBee_explorer.png)

Connect serial lines

- XBee 3 (DOUT) -> Arduino D13 (RX)
- XBee 4 (DIN) -> Arduino D14 (TX) 

The correct pin definitions are included in `lab 03 communication.ino`. Refer to the Arduino MKR Zero pinout diagram. 

**I don't really understand what's going on here!!**

```
\#define PIN_SERIAL2_RX    (1ul)         // Pin description number for PIO_SERCOM on D1
\#define PIN_SERIAL2_TX    (0ul)         // Pin description number for PIO_SERCOM on D0
#define PAD_SERIAL2_TX       (UART_TX_PAD_0)      // SERCOM pad 0 TX
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_1)    // SERCOM pad 1 RX
```




ArduCAM-M-2MP communicates with Arduino using both )I2C and SPI. The Arduino sketch handles these protocols with the `Wire` and `SPI` libraries. I2C: sensor configuration. SPI: camera commands and data stream (images). 

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
