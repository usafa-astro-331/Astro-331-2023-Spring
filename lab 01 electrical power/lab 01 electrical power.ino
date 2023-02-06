/* 
lab 01 electrical power
USAF Astro 331
2023 Spring
Lt Col Jordan Firth

required:
INA219 current sensor (on Adafruit breakout board)
16x2 LCD display
Arduino MKR ZERO

This script is used for an Astro 331 lab to measure current and voltage of a small
solar array. Expected output is 0~24 V DC and 0~40 mA. 
Because the current is low, we will use the higher-sensitivty lower-range mode of 
the INA219: 0-32 V DC, 0-1 A. 

This script will read voltage and current from an INA219 current sensor and 
calculate an exponential weighted moving average over `samples` samples (currently 50).

The result will be:
1. displayed on a 16x2 LCD display
2. written to the onboard microSD card every `write_interval` milliseconds

If connected to a serial monitor, the Arduino will send several diagnostic messages.
Also, there are a few serial write commands that are commented out. 

The INA219 communicates via I2C. With Adafruit's breakout board it has a default 
address of 0x40. Communication is handled by the Adafruit_INA219 library. 

*/

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#include <SPI.h>
#include <SD.h>

const int chipSelect = SDCARD_SS_PIN;


// liquidcrystal lcd display library
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(void) {
  Serial.begin(115200);
  /*
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
  */

  Serial.println("Hello!");
  
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

  Serial.println("Measuring voltage and current with INA219 ...");

    // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Voltage: ");

  lcd.setCursor(0,1);
  lcd.print("Current: ");

    Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
     
                File dataFile = SD.open("datalog.txt", FILE_WRITE);
              // if the file is available, write to it:
              if (dataFile) {
                dataFile.println("start of data");
                dataFile.println("current (mA), voltage (V)");
                dataFile.close();
              }
              // if the file isn't open, pop up an error:
              else { Serial.println("error opening datalog.txt");  }

} // end function setup

  long current_time = 0; 
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
  power_mW = ina219.getPower_mW();
  solar_panel_voltage = busvoltage + (shuntvoltage / 1000);
  
  /*
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
*/

voltage = voltage*(1-1/samples) + solar_panel_voltage/samples; 
current = current * (1-1/samples) + current_mA/samples;
 
  lcd.setCursor(9,0);
  lcd.print(voltage); 
  lcd.print(" V");

  lcd.setCursor(9,1);
  lcd.print(current); 
  lcd.print(" mA");

current_time = millis(); 

if(write_due < current_time){ // write data to file periodically
      String dataString = "";

        
          dataString += current ;
          dataString += ", ";
          dataString += voltage;

              File dataFile = SD.open("datalog.txt", FILE_WRITE);
              // if the file is available, write to it:
              if (dataFile) {
                dataFile.println(dataString);
                dataFile.close();
                // print to the serial port too:
                Serial.println(dataString);
              }
              // if the file isn't open, pop up an error:
              else {
                Serial.println("error opening datalog.txt");
              }
        
        write_due += write_interval; 
} // end write data to file

  //delay(200);
}
