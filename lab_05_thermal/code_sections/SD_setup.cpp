

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");

  File dataFile = SD.open("thermal.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("time (ms), temp (K), current (mA), voltage (V)");
    dataFile.close();
    Serial.println("INA219 I (mA), voltage (V)");
    
  }
  // if the file isn't open, pop up an error:
  else { Serial.println("error opening datalog.txt"); }
