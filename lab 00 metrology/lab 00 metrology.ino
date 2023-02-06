/* 
lab 00 metrology (A)
USAF Astro 331
2023 Spring
Lt Col Jordan Firth

required:
ACS 723 current sensor- (on sparkfun breakout board- low current version)
Arduino MKR 1000

This script is used for an Astro 331 lab to measure current and voltage. 
*/

/*  modified from SparkFun ACS712 and ACS723 Demo
    Created by George Beckstein for SparkFun
    4/30/2017
    Updated by SFE
    6/14/2018

    Uses an Arduino to set up the ACS712 and ACS723 Current Sensors
    See the tutorial at: https://learn.sparkfun.com/tutorials/current-sensor-breakout-acs723-hookup-guide
*/

// const int analogInPin = A1;
const int volt_in = A1;
const int curr_in = A3; 

// Number of samples to average the reading over
// Change this to make the reading smoother... but beware of buffer overflows!
const int avgSamples = 10;

// default sensor sensitivity of 1 and Vref of 0
float sensitivity = 1; 
float Vref = 0; // Output voltage with no current


// initialize empty values for current and voltage
int volt_counts = 0;
int curr_counts = 0; 


void setup() {

// set ADC resolution to 12-bit (default is 10-bit)
analogReadResolution(12); 

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
} // end function setup()

void loop() {

  // read the analog in value:
  for (int i = 0; i < avgSamples; i++) {
    volt_counts += analogRead(volt_in);
    curr_counts += analogRead(curr_in);

    // wait 2 milliseconds before the next loop for the analog-to-digital converter to settle
    // after the last reading:
    delay(2);
  }

  volt_counts = volt_counts / avgSamples;
  curr_counts = curr_counts / avgSamples;

  // The on-board ADC is 12-bits -> 2^12 = 4096 -> 3300 mV / 4096 counts 0.8 mV/count
  // XXX Voltage is in millivolts XXX
  // Voltage will not be in millivolts until you adjust the sensitivity
  float voltage = volt_counts * sensitivity; 

  float current = (curr_counts*0.8 - Vref) * sensitivity;
  
  Serial.print(voltage); 
  Serial.print(", ");
  Serial.print(current);
 
  Serial.print("\n");

  // Reset the sensor value for the next reading
  volt_counts = 0;
  curr_counts = 0;
  delay(100);
} // end function loop()
