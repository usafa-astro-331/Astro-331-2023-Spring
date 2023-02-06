/* 
lab 00 metrology (B)
USAF Astro 331
2023 Spring
Lt Col Jordan Firth

required:
INA219 current sensor (on Adafruit breakout board)
Arduino MKR 1000

This script is used for an Astro 331 lab to measure current and voltage. 

This script will read voltage and current from an INA219 current sensor and 
calculate an exponential weighted moving average over `samples` samples (currently 50).

The INA219 communicates with Arduino via I2C. With Adafruit's breakout board it has a default 
address of 0x40. Communication is handled by the Adafruit_INA219 library. 

*/

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#include <SPI.h>

void setup(void) {
  Serial.begin(9600);

  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

 
} // end function setup

  long present_time = 0; 
  long write_due = 0;
  long write_interval = 1000;

  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float solar_panel_voltage = 0;
  float power_mW = 0;

  float voltage=0 ;
  float current=0 ; 

  float samples = 50; // num of samples for an exponential weighted moving average

void loop(void) 
{

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  //power_mW = ina219.getPower_mW();
  solar_panel_voltage = busvoltage + (shuntvoltage / 1000);
  

voltage = voltage*(1-1/samples) + solar_panel_voltage/samples; 
current = current * (1-1/samples) + current_mA/samples;

present_time = millis(); 

if(write_due < present_time){ 
  String dataString = "";

    
  dataString += voltage;
  dataString += ", ";
  dataString += current ;
 
  Serial.println(dataString);
              
        
  write_due += write_interval; 
} // end if: write data to serial

} // end function loop()