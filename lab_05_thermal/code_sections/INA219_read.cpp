
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  solar_panel_voltage = busvoltage + (shuntvoltage / 1000);
  

voltage = voltage*(1-1/samples) + solar_panel_voltage/samples; 
current = current * (1-1/samples) + current_mA/samples;
