
/*
The INA219 communicates with Arduino via I2C. With Adafruit's breakout board it has a default 
address of 0x40. Communication is handled by the Adafruit_INA219 library. 

*/

#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float solar_panel_voltage = 0;
  float power_mW = 0;

  float voltage=0 ;
  float current=0 ; 

