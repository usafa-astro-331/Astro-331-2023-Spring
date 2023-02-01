# Lab 0: metrology

In this introductory lab you will get familiar with lab and measurement equipment. 



## Hardware 

- Arduino microcontroller
- multimeters
- current sensing ICs
- current and voltage ranges
- voltage dividers

## Documentation

- Arduino MKR datasheet
- Sparkfun Hall sensor datasheet

## Solar arrays

Previously you determined the max current and max voltage of FlatSat's 4-cell solar array with the cells wired in series and in parallel. Now you will investigate measurement techniques for the current and voltage. In a future lab you will fully characterize the solar array's electrical performance. 

|              | series | parallel |
| ------------ | ------ | -------- |
| current (mA) | 40     | 150      |
| voltage (V)  | 25     | 6        |

## Current

measure 0–150 mAk

still need good resolution at 0–40 mA

One method: Hall effect current sensor: non-invasive

sparkfun current sensor

connect on breadboard

analog sensor: measure output with a multimeter

adjust power supply from 0–40 mA

Is it good? Hard to tell



Connect to Arduino and plot via serial monitor

how about 0–150 mA?

calibrate/adjust gain and sensitivity

…





## Voltage

measure 0–25 V

still need good resolution for 0–6 V



Find voltage input range for Arduino MKR (datasheet).

Find Arduino MKR ADC (analog-to-digital converter) resolution



Need to match signal range (0–6V/0–25V) to sensor range. 25V is the worst case, so build a sensor that can handle input up to 25 V. 

Voltage divider

connect to arduino analog input pin

modify code, run code, open serial monitor, adjust power supply voltage and watch input change

