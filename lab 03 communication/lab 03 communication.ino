// #include "DHT.h"                 // Temperature and Humidity Sensor Library
// #include <Arduino.h>             // Standard Arduino Library
#include <wiring_private.h>      // For additional serial ports
#include "./src/BreezyArduCAM_single_jpeg.h"       // Arducam 'driver'
#include <SPI.h>                 // SPI communications
//#include "./src/WiFi101.h"             // Library for the built-in WiFi card on the MKR 1000
 #include "WiFiUdp.h"             // Load UTP protocol for WiFi
#include <RTCZero.h>            // Load the Real Time Clock driver
//#include "wifi_info.h"           // WiFi SSID and password

//Change the following to the name of the satellite and the PAN ID (4 digits)
#define SAT_NAME "OSCAR-12"
#define PAN_ID   "3331"

//For photocells - determines whether to save to a csv file on Python machine or to print
int cell[2] = {3, 2};

//Create enumerated types for sensor readings
enum SENSOR {AXIS, CURRENT, LIGHT};
SENSOR which_sensor[3] = {AXIS, CURRENT, LIGHT};

//Create arrays for current sensor output 
String sensor_name[3] = {"Battery: ","Solar: ","Other: "};
long int sensor[3] = {A0, A1, A2};

//Assign pin 6 to switch on and off the magnet
#define MAGNET 5
#define STARBOARD 4
#define PORT_T 3

//UDP setup for NTP
unsigned int localPort = 2390;          // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);   // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;         // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];    // buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;


//Real Time Clock
RTCZero rtc;                     // Create a Real Time Clock object
const int GMT = 0;               // USAFA time zone

//WiFi setup
//char ssid[] = SECRET_SSID;       // The network SSID (name)
//char pass[] = SECRET_PASS;       // your network password (use for WPA, or use as key for WEP)
//int keyIndex = 0;                // your network key Index number (needed only for WEP)
//int status = WL_IDLE_STATUS;     // WiFi currently set to idle

//Arducam setup
static const int CS = 7;         // Defined for Arducam
Serial_ArduCAM_FrameGrabber fg;  // Object to get frames from Arducam
ArduCAM_Mini_2MP myCam(CS, &fg); // Notify Frame Grabber object that the camera is on Chip Select Pin 7

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
// The new Serial3 will be used to communicate with the MKRZero on its Serial1 at 9600 baud
static const char MKR1000_LED       = 6;

// Serial2 pin and pad definitions (in Arduino files Variant.h & Variant.cpp)
#define PIN_SERIAL2_RX       (1ul)                // Pin description number for PIO_SERCOM on D1
#define PIN_SERIAL2_TX       (0ul)                // Pin description number for PIO_SERCOM on D0
#define PAD_SERIAL2_TX       (UART_TX_PAD_0)      // SERCOM pad 0 TX
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_1)    // SERCOM pad 1 RX

// Serial3 pin and pad definitions (in Arduino files Variant.h & Variant.cpp)
#define PIN_SERIAL3_RX       (5ul)                // Pin description number for PIO_SERCOM on D5
#define PIN_SERIAL3_TX       (4ul)                // Pin description number for PIO_SERCOM on D4
#define PAD_SERIAL3_TX       (UART_TX_PAD_2)      // SERCOM pad 2 TX
#define PAD_SERIAL3_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3 RX

// Instantiate the extra Serial classes
// NOTE: SERCOM3 and SERCOM 4 are the 'internal' serial communication ports on the SAMD chip.
// The two lines below 'tie' the internal serial communications to the pins on the MKR1000 
Uart Serial2(&sercom3, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
Uart Serial3(&sercom4, PIN_SERIAL3_RX, PIN_SERIAL3_TX, PAD_SERIAL3_RX, PAD_SERIAL3_TX);

void SERCOM3_Handler()    // Interrupt handler for SERCOM3
{
  Serial2.IrqHandler();
}

void SERCOM4_Handler()    // Interrupt handler for SERCOM4
{
  Serial3.IrqHandler();
}

void configureRadio() {
  // The XBee 3 on the MKR 1000 has to be configured as a router, 9600 baud, and in AT mode 
  // put the radio in command mode: 
  bool not_done = true;
  String ok_response = "OK\r";                                  //The﻿response we expect.
  String response = String("");                                 //Create an empty string
  Serial.println("Starting configureRadio()");
                                                                // Read the text of the response into the response variable 
  while (not_done){                                             // As long as we did not get a response from the XBee
    response = String("");
    Serial1.print("+++");                                       // Put the XBee 3 into 'Command Mode' 
    Serial.print("+++");                                        // Put the XBee 3 into 'Command Mode' 

    delay(1000);                                                // Wait for the XBee to finish
    while (response.length() < ok_response.length()){
      Serial.println(response);
      if (Serial1.available() > 0){          

          response += (char) Serial1.read();                    // Read a single character at a time
      }
    }
    not_done = !response.equals(ok_response);                   // Set the not_done flag to the opposite of the result of equality check
  }
                                                                // If we got the right response, configure the radio and return true.   
    Serial1.print("ATDH0\r");                                   // destination high and destination low addresses set to 0 means all messages will only go
    delay(100);                                                 // Wait for the XBee
    Serial1.print("ATDL0\r");                                   // to the Network Coordinator for this PAN.
    delay(100);                                                 // Wait for the XBee
    Serial1.print("ATID");
    Serial1.print(PAN_ID);
    Serial1.print("\r");                                        // Write the "network channel", aka the channel that the radio will work upon
    delay(100);                                                 // Wait for the XBee
    Serial1.print("ATNI");
    Serial1.print(SAT_NAME);
    Serial1.print("\r");                                        // Write the "network identifier", aka the name of this radio
    delay(100);                                                 // Wait for the XBee      
    Serial1.print("ATNI\r");                                    // Display the network identifier for debugging purposes
    delay(100);                                                 // Wait for the XBee

    Serial1.print("ATWR\r");                                    // Switch back to data mode
    delay(100);                                                 // Wait for the XBee
    response = String("");
    while (Serial1.available() > 0){
            response += (char) Serial1.read();                  //Read a single character at a time
    }       
    Serial1.print("ATCN\r");                                    // Switch back to data mode
    delay(100);
    Serial.println("Radio Configured");
}

void setup_additional_serial_ports(){
  pinPeripheral(0, PIO_SERCOM);                                   // Assign pins 0 & 1 SERCOM functionality
  pinPeripheral(1, PIO_SERCOM);                                   // Pin 0 is TX and Pin 1 is RX
  Serial2.begin(9600);                                            // Begin Serial2 at 9600 baud 

  pinPeripheral(4, PIO_SERCOM_ALT);                             // Assign pins 4 & 5 SERCOM functionality
  pinPeripheral(5, PIO_SERCOM_ALT);                             // Pin 4 is TX and Pin 6 is RX
  Serial3.begin(9600);                                          // Begin Serial3 at 57600 baud 
  Serial.println("setup_additional_serial_ports() completed");
}

void setup_serial_comms(){
  Serial.begin(9600);                                             // Set the Baud rate for communication to the PC via USB
  Serial1.begin(9600);                                            // Begin serial to XBee
  while(!Serial);                                                 // Wait for the serial communications to come on-line
  Serial.println("Serial and Serial1 are ready");
  Serial.flush();
  setup_additional_serial_ports();                                // Serial2 and Serial3
  delay(1000);                                                    // Wait a second for the MKR 1000 to 'warm up'
  configureRadio();                                               // Configure the XBEE radio   
  Serial.println("Setup_serial_comms completed");                 // Debug message to signify USB has connected
}

void setup_Arducam(){
                                                                  // Arducam initialization
                                                                  // ArduCAM Mini uses both I2C and SPI buses
  SPI.begin();
  Wire.begin();
  myCam.beginJpeg320x240();
  Serial.println("setup_Arducam() completed");                    // Debug message to signify USB has connected
}



// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;                                       // LI, Version, Mode
  packetBuffer[1] = 0;                                                // Stratum, or type of clock
  packetBuffer[2] = 6;                                                // Polling Interval
  packetBuffer[3] = 0xEC;                                             // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123);                                      //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}

/*
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
*/

/*
 * void setup_wifi(){
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
    if(status == WL_CONNECT_FAILED){
      Serial.println("Connection Failed");
    }
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}
*/

unsigned long query_time_server(){
  Serial.println("Starting query_time_server()");
  Udp.begin(localPort);                                           // Start UDP protocol on Wifi 
  unsigned long epoch;                                            // Will hold the current UTC in seconds since 1/1/1970 
  bool notDone = true;                                            // We have not received the current UTC time  
  while(notDone){                                                 // As long as we have not received the current UTC time, ask for it from a NTP server
    sendNTPpacket(timeServer);                                    // Send an NTP packet to a time server
    delay(1000);                                                  // Wait to see if a reply is available
    if (Udp.parsePacket()) {                                      // We've received a packet, read the data from it                                        
      Serial.println("packet received");                          
      Udp.read(packetBuffer, NTP_PACKET_SIZE);                    // Read the packet into the buffer  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      Serial.print("Seconds since Jan 1 1900 = ");
      Serial.println(secsSince1900);
  
      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      epoch = secsSince1900 - seventyYears;
      notDone = false;
    }
  }
  return epoch;
  Serial.println("Ending query_time_server()");
}

void setup_rtc(){
  rtc.begin();                          // Start the real time clock   
  unsigned long epoch = 0;              // Time variable
  epoch = query_time_server();          //Get UTC current time
  Serial.print("Epoch received: ");     // Display current time on debug
  Serial.println(epoch);
  rtc.setEpoch(epoch);                  // Set the RTC to the current UTC time
  Serial.println();
}

void printTime(){
  print2digits(rtc.getHours());
  Serial1.print(":");
  print2digits(rtc.getMinutes());
  Serial1.print(":");
  print2digits(rtc.getSeconds());
  Serial1.print("Z");
}


void printDate(){
  Serial1.print("20");
  Serial1.print(rtc.getYear());
  Serial1.print("-");
  print2digits(rtc.getMonth());
  Serial1.print("-");
  print2digits(rtc.getDay());
  Serial1.print("T");
}


void print2digits(int number) {
  if (number < 10) {
    Serial1.print("0");
  }
  Serial1.print(number);
}

void setup_output_pins(){
  pinMode(MAGNET, OUTPUT);
  pinMode(PORT_T, OUTPUT);
  pinMode(STARBOARD, OUTPUT);
}

void setup() {                               // The items in this function are run at boot time for the Arduino 
 // setup_wifi();                            // Setup and turn on WiFi
 // setup_rtc();                             // Setup the Real Time Clock
  rtc.begin();    
  setup_serial_comms();                      // Setup Serial2 and Serial3
  setup_Arducam();                           // Setup Arducam parameters
  setup_output_pins();
  Serial.println("Cube Sat Connected!");     // Debug message to signify USB has connected

  Serial1.setTimeout(250);                   // Check reading of commands from the PC every 250 milliseconds
}

void get_light_level(bool csv){
  Serial2.print(cell[csv]);
  while(!Serial2.available());
  String response = Serial2.readString();
  Serial1.println(response);  
}

void get_3_axis_data(bool csv){
  String response;
  Serial.println("Starting get_3_axis_data");
  if(csv){
    Serial2.println("4");
  }
  else{
    Serial2.println("5");
  }
  while(!Serial2.available());
  response = Serial2.readString();
  Serial1.print(response);  
  Serial.print(response);  
  Serial.println("Exiting get_3_axis_data");
}

void move_motor(int direction){
  Serial2.print("6 ");
  Serial2.print(direction);
  while(!Serial2.available());
  String response = Serial2.readString();
  Serial1.print(response); 
}

void get_temp_data(){
  Serial2.println("1 2");
  while(!Serial2.available());
  String response = Serial2.readString();
  Serial1.print(response); 
  Serial1.println(""); 
}

void get_current(int which, bool csv){
  float value = analogRead(sensor[which]);
  value *= 5.0 / 1023; 
  if(!csv)
    Serial1.print(sensor_name[which]);
  Serial1.print(value);
  Serial.print(value);
  if(csv){
    switch(which){
      case 0:
      case 1: Serial1.print(",");
              Serial.print(",");
              break;
      case 2: Serial1.println(" ");
              Serial.println(" ");
              Serial1.flush();
              delay(250);                    //Needed to prevent buffer overruns on XBee
    }
  } 
  else
     Serial1.println(" amps DC");
}

void turn_on_pin(char pin_name[], int item){
  Serial1.print(pin_name);
  Serial1.println(" turned on");
  digitalWrite(item, HIGH);
}
                    
void turn_off_pin(char pin_name[], int item){
  Serial1.print(pin_name);
  Serial1.println(" turned off");
  digitalWrite(item, LOW);
}

void record_data(SENSOR sensor, unsigned long value){
   time_t start = rtc.getEpoch();
   Serial.println(sensor);
   while(rtc.getEpoch() < start + value){
      printDate();
      printTime();
      Serial1.print(",");
      switch(sensor){
        case AXIS    :  get_3_axis_data(true);
                        break;

        case CURRENT :  get_current(0, true);
                        get_current(1, true);
                        get_current(2, true);
                        break;

        case LIGHT   :  get_light_level(true);
                        break;
      }
      
   }
   Serial1.println("EOT");
}

void get_current_time(){
  Serial1.print("Current satellite time is: ");
  printDate();
  printTime();
  Serial1.print("!");
}

void sync_rtc(long int value){
  rtc.setEpoch(value);
  printDate();
  printTime();
  Serial1.println();
}


void get_sat_rssi(){
    // put the radio in command mode: 
  bool not_done = true;
  String ok_response = "OK\r";                                  //The﻿response we expect.
  String response = String("");                                 //Create an empty string
  Serial.println("Starting get_sat_rssi()");
                                                                // Read the text of the response into the response variable 
  while (not_done){                                             // As long as we did not get a response from the XBee
    response = String("");
    Serial1.print("+++");                                       // Put the XBee 3 into 'Command Mode' 
    Serial.print("+++");                                        // Put the XBee 3 into 'Command Mode' 

    delay(1000);                                                // Wait for the XBee to finish
    while (response.length() < ok_response.length()){
      Serial.println(response);
      if (Serial1.available() > 0){          

          response += (char) Serial1.read();                    // Read a single character at a time
      }
    }
    not_done = !response.equals(ok_response);                   // Set the not_done flag to the opposite of the result of equality check
  }
                                                                // If we got the right response, configure the radio and return true.   
    Serial1.print("ATDB\r");                                   // destination high and destination low addresses set to 0 means all messages will only go
    delay(100);                                                 // Wait for the XBee
    response = String("");
    while (Serial1.available() > 0){
            response += (char) Serial1.read();                  //Read a single character at a time
            Serial.print(response);
    }       
    Serial1.print("ATCN\r");                                    // Switch back to data mode
    delay(100);
    Serial1.print(response);
    Serial.println("sat rssi sent");
}

void get_command_from_pc(){
  int space, do_it = 0;                                       // For reading in a space from the PC
  unsigned long int value = 0;                                // Secondary value for items that have a 'set' functionality
  
  String what_to_do = Serial1.readString();                   // Find out what the user wants the MKR1000 to do.
   if (what_to_do.length() != 0){                             // As long as the string contains "something"
     Serial.print("Received: ");
     Serial.println(what_to_do);

      what_to_do.trim();                                      // Remove all leading and trailing spaces
      space = what_to_do.indexOf(' ');                        // See if there is a space somewhere after removing leading and trailing spaces
      if(space > -1){                                         // If there isa space, then we have a value to 'set'
         value = (what_to_do.substring(space)).toInt();       // Get the 'set' value as an integer
         do_it = (what_to_do.substring(0, space)).toInt();    // Get the 'command' value as an integer
      }
      else{
        do_it = (what_to_do.substring(0, space)).toInt();
      }
      Serial.print(do_it);
      Serial.print(" ");
      Serial.println(value);
      switch(do_it){                                          // Determine what we are supposed to do
        
            case 8: Serial1.print("FlatSat Command Port Ready at ");
                    sync_rtc(value);
                    Serial.println("FlatSat Command Port Ready");
                    break;
                    
            case 9: Serial1.println("FlatSat Exiting Command Mode");
                    Serial.println("FlatSat Exiting Command Mode");
                    break;
            
            case 1: get_current_time();
                    break;
            
            case 2: get_3_axis_data(false);
                    break;
            
            case 3: get_temp_data();
                    break;
                    
            case 4: 
            case 5: 
            case 6: get_current(do_it - 4, false);
                    break;

            case 7: Serial1.print("Getting picture from camera.");
                    Serial.println("Grabbing picture");
                    delay(500);
                    myCam.capture( );
                    delay(500);
                    Serial.println("Grabbing picture complete");
                    break;

           case 10: get_light_level(false);
                    break;
                    
           case 12:                   // 9 dof sensor
           case 13:                   // Current sensors
           case 14: if(!value)        // Photocells
                      value = 30;
                    record_data(which_sensor[do_it - 12], value);
                    break;

           case 15: move_motor(value);
                    break;

           case 16: turn_on_pin("Magnet", MAGNET);
                    break;
                    
          case 17: turn_off_pin("Magnet", MAGNET);
                   break;
                   
          case 18: turn_on_pin("Port thruster", PORT_T);
                    break;
                    
          case 19: turn_off_pin("Port thruster", PORT_T);
                   break;

          case 20: turn_on_pin("Starboard thruster", STARBOARD);
                   break;
                    
          case 21: turn_off_pin("Starboard thruster", STARBOARD);
                   break;

          case 22: get_sat_rssi();
                   break;
      }
  }
  delay(25);
}

void loop() {
  get_command_from_pc();
}
