// #include "DHT.h"                 // Temperature and Humidity Sensor Library
#include <Arduino.h>                    // Standard Arduino Library
#include <wiring_private.h>             // For additional serial ports
#include <BreezyArduCAM_single_jpeg.h>  // Arducam 'driver'
#include <SPI.h>                        // SPI communications
// #include <WiFi101.h>                    // Library for the built-in WiFi card on the MKR 1000
// #include <WiFiUdp.h>                    // Load UTP protocol for WiFi
// #include <RTCZero.h>                    // Load the Real Time Clock driver
#include "wifi_info.h"  // WiFi SSID and password

//Change the following to the name of the satellite and the PAN ID (4 digits)
#define SAT_NAME "OSCAR-12"
#define PAN_ID "3331"

//For photocells - determines whether to save to a csv file on Python machine or to print
int cell[2] = { 3, 2 };

//Create enumerated types for sensor readings
enum SENSOR { AXIS,
              CURRENT,
              LIGHT };
SENSOR which_sensor[3] = { AXIS, CURRENT, LIGHT };

//Create arrays for current sensor output
String sensor_name[3] = { "Battery: ", "Solar: ", "Other: " };
long int sensor[3] = { A0, A1, A2 };

//Assign pin 6 to switch on and off the magnet
#define MAGNET 5
#define STARBOARD 4
#define PORT_T 3

//UDP setup for NTP
unsigned int localPort = 2390;         // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);  // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;        // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];    // buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
// WiFiUDP Udp;


//Arducam setup
static const int CS = 7;          // Defined for Arducam
Serial_ArduCAM_FrameGrabber fg;   // Object to get frames from Arducam
ArduCAM_Mini_2MP myCam(CS, &fg);  // Notify Frame Grabber object that the camera is on Chip Select Pin 7

//Gravity Acceleration
#ifdef SOFTWAREWIRE
#include <SoftwareWire.h>
SoftwareWire myWire(3, 2);
#define WIRE myWire
#else
#include <Wire.h>
#define WIRE Wire
#endif

// Serial is used for USB communications (Debugging output)
// Serial1 is used by the MKR 1000 and Zero as the USART
// Serial1 on the MKR1000 is used to communicate with the XBEE Radio
static const char MKR1000_LED = 6;

void configureRadio() {
  // The XBee 3 on the MKR 1000 has to be configured as a router, 9600 baud, and in AT mode
  // put the radio in command mode:
  bool not_done = true;
  String ok_response = "OK\r";   //The﻿response we expect.
  String response = String("");  //Create an empty string
  Serial.println("Starting configureRadio()");
  // Read the text of the response into the response variable
  while (not_done) {  // As long as we did not get a response from the XBee
    response = String("");
    Serial1.print("+++");  // Put the XBee 3 into 'Command Mode'
    Serial.print("+++");   // Put the XBee 3 into 'Command Mode'

    delay(1000);  // Wait for the XBee to finish
    while (response.length() < ok_response.length()) {
      Serial.println(response);
      if (Serial1.available() > 0) {

        response += (char)Serial1.read();  // Read a single character at a time
      }
    }
    not_done = !response.equals(ok_response);  // Set the not_done flag to the opposite of the result of equality check
  }
  // If we got the right response, configure the radio and return true.   
  Serial1.print("ATDH0\r");  // destination high and destination low addresses set to 0 means all messages will only go
  delay(100);                // Wait for the XBee
  Serial1.print("ATDL0\r");  // to the Network Coordinator for this PAN.
  delay(100);                // Wait for the XBee
  Serial1.print("ATID");
  Serial1.print(PAN_ID);
  Serial1.print("\r");  // Write the "network channel", aka the channel that the radio will work upon
  delay(100);           // Wait for the XBee
  Serial1.print("ATNI");
  Serial1.print(SAT_NAME);
  Serial1.print("\r");      // Write the "network identifier", aka the name of this radio
  delay(100);               // Wait for the XBee
  Serial1.print("ATNI\r");  // Display the network identifier for debugging purposes
  delay(100);               // Wait for the XBee

  Serial1.print("ATWR\r");  // Switch back to data mode
  delay(100);               // Wait for the XBee
  response = String("");
  while (Serial1.available() > 0) {
    response += (char)Serial1.read();  //Read a single character at a time
  }
  Serial1.print("ATCN\r");  // Switch back to data mode
  delay(100);
  Serial.println("Radio Configured");
}



void setup_serial_comms() {
  Serial.begin(9600);   // Set the Baud rate for communication to the PC via USB
  Serial1.begin(9600);  // Begin serial to XBee
  while (!Serial)
    ;  // Wait for the serial communications to come on-line
  Serial.println("Serial and Serial1 are ready");
  Serial.flush();
  // setup_additional_serial_ports();                                // Serial2 and Serial3
  delay(1000);                                     // Wait a second for the MKR 1000 to 'warm up'
  configureRadio();                                // Configure the XBEE radio
  Serial.println("Setup_serial_comms completed");  // Debug message to signify USB has connected
}

void setup_Arducam() {
  // Arducam initialization
  // ArduCAM Mini uses both I2C and SPI buses
  SPI.begin();
  Wire.begin();
  myCam.beginJpeg320x240();
  Serial.println("setup_Arducam() completed");  // Debug message to signify USB has connected
}




void print2digits(int number) {
  if (number < 10) {
    Serial1.print("0");
  }
  Serial1.print(number);
}

void setup_output_pins() {
  pinMode(MAGNET, OUTPUT);
  pinMode(PORT_T, OUTPUT);
  pinMode(STARBOARD, OUTPUT);
}

void setup() {  // The items in this function are run at boot time for the Arduino
                // setup_wifi();                            // Setup and turn on WiFi
                // setup_rtc();                             // Setup the Real Time Clock
  // rtc.begin();
  setup_serial_comms();  // Setup Serial2 and Serial3
  setup_Arducam();                           // Setup Arducam parameters
  setup_output_pins();
  Serial.println("Cube Sat Connected!");  // Debug message to signify USB has connected

  Serial1.setTimeout(250);  // Check reading of commands from the PC every 250 milliseconds
}


void turn_on_pin(char pin_name[], int item) {
  Serial1.print(pin_name);
  Serial1.println(" turned on");
  digitalWrite(item, HIGH);
}

void turn_off_pin(char pin_name[], int item) {
  Serial1.print(pin_name);
  Serial1.println(" turned off");
  digitalWrite(item, LOW);
}


void get_sat_rssi() {
  // put the radio in command mode:
  bool not_done = true;
  String ok_response = "OK\r";   //The﻿response we expect.
  String response = String("");  //Create an empty string
  Serial.println("Starting get_sat_rssi()");
  // Read the text of the response into the response variable
  while (not_done) {  // As long as we did not get a response from the XBee
    response = String("");
    Serial1.print("+++");  // Put the XBee 3 into 'Command Mode'
    Serial.print("+++");   // Put the XBee 3 into 'Command Mode'

    delay(1000);  // Wait for the XBee to finish
    while (response.length() < ok_response.length()) {
      Serial.println(response);
      if (Serial1.available() > 0) {

        response += (char)Serial1.read();  // Read a single character at a time
      }
    }
    not_done = !response.equals(ok_response);  // Set the not_done flag to the opposite of the result of equality check
  }
  // If we got the right response, configure the radio and return true.   
  Serial1.print("ATDB\r");  // destination high and destination low addresses set to 0 means all messages will only go
  delay(100);               // Wait for the XBee
  response = String("");
  while (Serial1.available() > 0) {
    response += (char)Serial1.read();  //Read a single character at a time
    Serial.print(response);
  }
  Serial1.print("ATCN\r");  // Switch back to data mode
  delay(100);
  Serial1.print(response);
  Serial.println("sat rssi sent");
}

void get_command_from_pc() {
  int space, do_it = 0;         // For reading in a space from the PC
  unsigned long int value = 0;  // Secondary value for items that have a 'set' functionality

  String what_to_do = Serial1.readString();  // Find out what the user wants the MKR1000 to do.
  if (what_to_do.length() != 0) {            // As long as the string contains "something"
    Serial.print("Received: ");
    Serial.println(what_to_do);

    what_to_do.trim();                                   // Remove all leading and trailing spaces
    space = what_to_do.indexOf(' ');                     // See if there is a space somewhere after removing leading and trailing spaces
    if (space > -1) {                                    // If there isa space, then we have a value to 'set'
      value = (what_to_do.substring(space)).toInt();     // Get the 'set' value as an integer
      do_it = (what_to_do.substring(0, space)).toInt();  // Get the 'command' value as an integer
    } else {
      do_it = (what_to_do.substring(0, space)).toInt();
    }
    Serial.print(do_it);
    Serial.print(" ");
    Serial.println(value);
    switch (do_it) {  // Determine what we are supposed to do

      case 8:
        Serial1.print("OSCAR-12 Command Port Ready at ");
        // sync_rtc(value);
        Serial.println("OSCAR-12 Command Port Ready");
        break;

      case 9:
        Serial1.println("OSCAR-12 Exiting Command Mode");
        Serial.println("OSCAR-12 Exiting Command Mode");
        break;

      case 7:
        Serial1.print("Getting picture from camera.");
        Serial.println("Grabbing picture");
        delay(500);
        myCam.capture();
        delay(500);
        Serial.println("Grabbing picture complete");
        break;


      case 12:  // 9 dof sensor
      case 13:  // Current sensors
      case 16:
        turn_on_pin("Magnet", MAGNET);
        break;

      case 17:
        turn_off_pin("Magnet", MAGNET);
        break;

      case 18:
        turn_on_pin("Port thruster", PORT_T);
        break;

      case 19:
        turn_off_pin("Port thruster", PORT_T);
        break;

      case 20:
        turn_on_pin("Starboard thruster", STARBOARD);
        break;

      case 21:
        turn_off_pin("Starboard thruster", STARBOARD);
        break;

      case 22:
        get_sat_rssi();
        break;
    }
  }
  delay(25);
}

void loop() {
  get_command_from_pc();
}
