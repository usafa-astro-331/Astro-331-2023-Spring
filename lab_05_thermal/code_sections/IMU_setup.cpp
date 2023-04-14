// ----- ICM 20948 IMU
  SPI_PORT.begin();

   bool initialized = false;
     while (!initialized)
  {

    myICM.begin(CS_PIN, SPI_PORT);


    Serial.print(F("Initialization of the sensor returned: "));
    Serial.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }


  // end ICM 20948 IMU 