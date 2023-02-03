# Lab 0: metrology

> metrology: the science of  measurement, embracing both experimental and theoretical determinations  at any level of uncertainty in any field of science and technology
> -International Bureau of Weights and Measures (BIPM) 

In this introductory lab you will gain familiarity with lab equipment and with taking measurements. 



Previously, you determined the current and voltage limits of a 4-cell solar array. In a future lab you will measure the array's current and voltage output and create an I-V curve to characterize the solar array. In this lab you will begin learning how to measure analog signals with FlatSAT—the bridge between an analog world and digital engineering. 

To measure FlatSAT's current and voltage, you must pick appropriate sensors. 

## Overview

- voltage measurement

  - voltage divider
  - analog-to-digital converter (ADC)

- current measurement

  - hall effect sensor
  - shunt resistor

- Arduino basics

  



## Hardware 

- Arduino MKR 
- multimeter
- benchtop power supply
- resistors
- 
- current sensing ICs

## Software 

- Arduino IDE
- `lab 00 metrology.ino`

## Documentation

- Arduino MKR datasheet
- Sparkfun Hall sensor datasheet



## Solar arrays limits

For this lab, use these values as your solar array's max current and voltage. 

|              | series | parallel |
| ------------ | ------ | -------- |
| current (mA) | 40     | 160      |
| voltage (V)  | 30     | 7.5      |



## Voltage

To accurately measure FlatSAT's solar array performance, your sensor must be able to measure DC voltage from 0–30 V. 

Investigate this requirement. 

### Multimeter

- Turn on your benchtop power supply and set it to 5 V/0.5 A. 
- Turn your multimeter to the 600 V range
- Connect wires with banana plugs between the power supply and the multimeter
- Adjust the power supply up and down in 0.1 V increments (use the dial)
- What happens? Record observations in your lab notebook. 

These multimeters are *2000 count* multimeters—a common resolution for digital multimeters. They have 4-digit precision from -1999 counts to 1999 counts. A count is the smallest discrete measurement the multimeter can make. A 2000 count multimeter is also known as a 3-1/2 digit multimeter. They have 4 digits until 1.999, and then 3 digits from 2.00 to 9.99. 

Ranges on a multimeter can provide precision proportional to the range. 

- 0–1.999 V
- 0–19.99 V
- 0–199.9 V
- 0–600. V

Internally, a 2000 count multimeter uses a 12-bit analog-to-digital converter (ADC). 2^12^ = 4096 bits, a close match for -1999 to 1999 counts.  

What multimeter range should you use to measure your 0–30 V array output? What is the best resolution you can achieve? 

### Arduino limits

Now you will prepare to measure voltage with an Arduino MKR. 

First, review the Arduino MKR datasheet. 

- Allowable input voltage: 
  - see the warning at the bottom of page 2
- Max ADC resolution: 

Record these values in your lab notebook. 

Using these values, how many digits do you expect to be able to report for the voltage of FlatSAT's 30 V solar array?



Every sensor is resolution-limited and provides maximum resolution when the range of the measured signal is matched to the sensor's detection range. 

To achieve the best available voltage resolution you will need to map the 0–30 V panel voltage to the 0–3.3 V input range of the Arduino MKR. Note: 3.3 V is sometimes referred to as 3V3. 

If you connect anything higher than 3.3 V directly to an input pin on your Arduino, the best possible outcome is that the measured input will remain pegged at "1" or "3.3V." More likely is that you will permanently damage your Arduino. 

To map a 30 V signal to a 3.3 V sensor, you will use a voltage divider. 

### Voltage Divider

>A **voltage divider** is a passive linear circuit that produces an output voltage V~out~ that is a fraction of its input voltage V~in~.**Voltage division** is the result of distributing the input voltage among the components of the divider. A simple example of a voltage divider is two resistors connected in series, with the input voltage applied across the resistor pair and the output voltage emerging from the connection between them.
>-Wikipedia

![Resistive_divider2](sources/Resistive_divider2.svg)

$V_{out} = \frac{R_2}{R_1+R_2}V_{in}$

Approximately how much do you need to reduce your input voltage? What relative values of $R_1$ and $R_2$ will give you that ratio? Work this out in your lab notebook. 

Now you have to pick actual resistors and make a voltage divider. 

Keep in mind: 

- $V = IR$
- $P=IV$
- your resistors are 1/4 W resistors
- an ideal voltmeter has infinite resistance

Now, select resistors for R1 and R2. Record these values in your lab notebook. 

At this time, explain your choice to your instructor. 



**Note:** Do not connect your Arduino to power or to a computer at this time. 

Place R1 and R2 on the breadboard as shown. R1 is the larger resistor. 

Connect and energize a 30 V power supply to the bottom rail of the breadboard. 

With your multimeter, measure voltages. 

- ground–middle
- ground–top (30 V) 

Ground–middle should be less than 3.3 V. 

​		**Stop. Do not proceed until Ground–middle is ≤ 3.3 V**

Record these measurements in your lab notebook. 



### Arduino measurement

Now you will measure voltage with Arduino. 

- Energize the power supply. **With a voltmeter, ensure that Arduino pin A1 only sees 3.3 V from ground even when the power supply provides 30 V.** 

- Open `lab 00 metrology.ino` 

- Connect Arduino to your computer. 

- Open the serial plotter (tools -> serial plotter). 

- Select the correct board (tools -> board -> SAMD -> Arduino MKR 1000)

- Select the correct port (COMXX—try one until it works). 

- Click `upload` (right arrow near the top of the window). 

The serial plotter will show a moving graph. Adjust the power supply **downward only** and watch the plotted line move. As the power supply moves from 30 V–0 V, Arduino sees from 3.3 V–0V. What range of values does the serial plotter display? 

This is because of the 12-bit ADC resolution. You must add a scale factor to output the correct voltage. 

Arduino's default ADC resolution is 10-bit. `lab 00 metrology.ino` uses the command `analogReadResolution(12);` to adjust the ADC resolution. 

With an input range of 0–3.3 V and 12-bit/4096 count, Arduino's sensitivity is 0.8 mV/count (3.3V/4096 count). 

To properly display Arduino's **measured voltage**, you must **multiply the ADC reading `volt_counts` by 0.8.** 

However, you instead want to display the voltage of your power rail (which represents the array power). You must **multiply the measured voltage** by the ratio of your voltage divider. 

Calculate this factor and change the following code line to include the proper scale factor (instead of 1). 

```
 float voltage = volt_counts * 1;
```

Upload this modified code and ensure that the serial plotter matches the output of your power supply. If not, adjust as necessary. 



Congratulations! You can measure voltage!

## To do

move serial.print to the end of the lab for easy finding and modification

write current section

update fritzing file



## Current





In 12-bit mode 

measure 0–25 V

still need good resolution for 0–6 V



Find voltage input range for Arduino MKR (datasheet).

Find Arduino MKR ADC (analog-to-digital converter) resolution



Need to match signal range (0–6V/0–25V) to sensor range. 25V is the worst case, so build a sensor that can handle input up to 25 V. 

Voltage divider

connect to arduino analog input pin

modify code, run code, open serial monitor, adjust power supply voltage and watch input change



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





