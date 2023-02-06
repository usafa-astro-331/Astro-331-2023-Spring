/*  SparkFun ACS712 and ACS723 Demo
    Created by George Beckstein for SparkFun
    4/30/2017
    Updated by SFE
    6/14/2018

    Uses an Arduino to set up the ACS712 and ACS723 Current Sensors
    See the tutorial at: https://learn.sparkfun.com/tutorials/current-sensor-breakout-acs723-hookup-guide

    Parts you may need:
    - 100 Ohm, 1/2W or greater resistor OR two 220 Ohm 1/4 resistors in parallel
    - ACS712 Breakout with on-board amplifier or ACS723 Current Sensor (Low Current)

    Optional equipment:
    - Oscilloscope
    - Multimeter (or two)
    - A power supply with current limiting/constant current would be handy to calibrate the device without using resistors
*/

// const int analogInPin = A1;
const int volt_in = A1;
const int curr_in = A3; 
// set ADC resolution to 12-bit (default is 10-bit)

// 

// Number of samples to average the reading over
// Change this to make the reading smoother... but beware of buffer overflows!
const int avgSamples = 10;

const int sensitivity = 1; 

// int sensorValue = 0;
int volt_counts = 0;
int curr_counts = 0; 

float sensitivity = 1; //100.0 / 500.0; //100mA per 500mV = 0.2
float Vref = 0; // Output voltage with no current: ~ 2500mV or 2.5V

void setup() {

analogReadResolution(12); 

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  for (int i = 0; i < avgSamples; i++) {
    volt_counts += analogRead(volt_in);
    curr_counts += analogRead(curr_in);
    // sensorValue += analogRead(analogInPin);

    // wait 2 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    delay(2);
  }

  volt_counts = volt_counts / avgSamples;
  curr_counts = curr_counts / avgSamples;

  // The on-board ADC is 12-bits -> 2^12 = 4096 -> 3300 mV / 4096 counts 0.8 mV/count
  // XXX The voltage is in millivolts XXX
  // The voltage will not be in millivolts until you adjust the sensitivity
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
}
