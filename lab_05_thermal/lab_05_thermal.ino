/* 
lab 05 thermal
USAF Astro 331
2023 Spring
Lt Col Jordan Firth

required:
INA219 current sensor (on Adafruit breakout board)
Sparkfun 9DOF IMU
Arduino MKR ZERO
57 ohm carbon film resistor
  (2 resistors in series: 57 = 47 + 10) 

This script is used for an Astro 331 lab to measure current flowing through a resistor as 
the resistor's temperature changes. 
The  current is measured with an INA219 shunt resistor. 
Temperature is measured with the 9DOF sensor (it's the 10th DOF). 

Temperature and current will be written to the SD card every 15 seconds.  

*/

#include <Wire.h>;

#include "code_sections/INA219_initialize.h"


#include <SD.h>;

const int chipSelect = SDCARD_SS_PIN;

float samples = 50;

long current_time = 0;
long write_due = 0;
long write_interval = 15000;



#include "code_sections/IMU_initialize.h";


void setup(void) {
  Serial.begin(115200);


#include "code_sections/INA219_setup.cpp";


#include "code_sections/IMU_setup.cpp";

#include "code_sections/SD_setup.cpp";



}  // end function setup


// #include "code_sections/formatting.cpp";


void loop(void) {
  

#include "code_sections/INA219_read.cpp";

  myICM.getAGMT();                        // The values are only updated when you call 'getAGMT'
  
current_time = millis();


  if (write_due < current_time) {  // write data to file periodically
   String write_line = "";

  
  write_line += current_time;
  write_line += ", ";

  write_line += printScaledAGMT(&myICM);  // This function takes into account the scale settings from when the measurement was made to calculate the values with units


    write_line += current;
    write_line += ", ";
    write_line += voltage;
    write_line += ", ";



    File dataFile = SD.open("thermal.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(write_line);
      dataFile.close();
      // print to the serial port too:
      // Serial.println(write_line);
      // // Serial.print("Power:         ");
      // Serial.print(power_mW);
      // Serial.println(" mW");

    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }

Serial.println(write_line);
      
    write_due += write_interval;
  }  // end write data to file

  //delay(200);
}
