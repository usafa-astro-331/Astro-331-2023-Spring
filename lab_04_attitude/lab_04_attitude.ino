// ----- ICM 20948 IMU
  #include <ICM_20948.h>
  #define USE_SPI      

  #define SERIAL_PORT Serial
  #define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
  #define CS_PIN 6     // Which pin you connect CS to. Used only when "USE_SPI" is defined
  ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object


// ----- TB9051FTG Motor Carrier
  #include <TB9051FTGMotorCarrier.h>
  #include <QuadratureEncoder.h>
  // QuadratureEncoder requires <EnableInterrupt.h>

  // TB9051FTGMotorCarrier pin definitions
  static constexpr uint8_t pwm1Pin{2};
  static constexpr uint8_t pwm2Pin{3};

  // Instantiate TB9051FTGMotorCarrier
  static TB9051FTGMotorCarrier driver{pwm1Pin, pwm2Pin};

  // set up variable for pulse width modulation of motor
  static float throttlePWM{0.0f};

  // Create an Encoder object name Encoder, using digital pins 0 & 1 (interrupt pins on MKR Zero)
  Encoders Encoder(0,1);  

  unsigned long lastMilli = 0;
  long currentEncoderCount = 0;
  long lastEncoderCount = 0;
  float speed_rpm = 0.0;
  long timeElapsed = 0;

  float speed_pwm;



  // ----- time variables -----
  int print_time = 0 ; 
  int print_delay = 500; 
  int current_time = 0; 
  int elapsed = 0; 


void setup() {
  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT)
  {
  };

// ----- ICM 20948 IMU
  SPI_PORT.begin();

   bool initialized = false;
     while (!initialized)
  {

    myICM.begin(CS_PIN, SPI_PORT);


    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }

// ----- TB9051FTG Motor Carrier
  driver.enable();

  
  
  
  // put your setup code here, to run once:
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  
}

float angle;

int speed; 

int t; 
int t0 = millis(); // set start time right before loop

void loop() {


  // if (myICM.dataReady())
  // {
  //   myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
  //                               printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
  //   // printScaledAGMT(&myICM); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
  //   // delay(30);
  // }
  // else
  // {
  //   // SERIAL_PORT.println("Waiting for data");
  //   // delay(500);
  // }



  
//   // put your main code here, to run repeatedly:
//   digitalWrite(A0,HIGH); 
// digitalWrite(A1,HIGH); 
// digitalWrite(A2,HIGH); 
// digitalWrite(A3,HIGH); 

//   delay(250); 

//   digitalWrite(A0,LOW); 
//   digitalWrite(A1,LOW); 
//   digitalWrite(A2,LOW); 
//   digitalWrite(A3,LOW); 

//   delay(250); 

speed_pwm = set_speed(); 
Serial.println(speed_pwm);
delay(350);

// current_time = millis();
// if (print_time+print_delay > current_time){
//   Serial.println(speed_pwm); 
//   print_time += print_delay; 

// } // end time to print

}


void printRawAGMT(ICM_20948_AGMT_t agmt)
{

  // SERIAL_PORT.print(agmt.mag.axes.x);
  // SERIAL_PORT.print(", ");
  // SERIAL_PORT.print(agmt.mag.axes.y);
  // SERIAL_PORT.print(", ");
  // SERIAL_PORT.print(agmt.mag.axes.z);

  angle = atan2(agmt.mag.axes.y, agmt.mag.axes.x); 
  angle = angle*180/3.1415926;
  if(angle<0){
    angle+= 360;
  }

  SERIAL_PORT.print(angle);
 
  SERIAL_PORT.println();
}


float set_speed(){
	t = millis() - t0; 
  Serial.println(t);

	if ( t-t0 < 10e3){ // hold still at half speed (10 sec)
		elapsed = t - 10e3 ;
		throttlePWM = 0.5*elapsed/5e3; 
    
    Serial.println(throttlePWM);
  }
	else if (t-t0 < 15e3){ // ramp up to full speed (5 sec)
		elapsed = t - 10e3 ;
		throttlePWM = 0.5 + 0.5*elapsed/5e3;
  }
	else if (t-t0 < 20e3){ // ramp back down to half speed (5 sec)
		elapsed = t - 15e3; 
		throttlePWM = 1 - 0.5*elapsed/5e3;
  }
	else if (t-t0 < 25e3){ // hold new position (5 sec)
		throttlePWM = 0.5;
  }
	else if (t-t0 < 30e3){ // ramp down to (near) motionless (5 sec)
		elapsed = t - 25e3;
		throttlePWM = 0.5 - .45*elapsed/5e3; 
  }
	else if (t-t0 < 35e3){ // ramp back up to half speed  (5 sec)
		elapsed = t - 30e3; 
		throttlePWM = .05 + .45*elapsed/5e3; 
  }
	else { // hold position 
		throttlePWM = 0.5;
  }

  driver.setOutput(throttlePWM);

	return throttlePWM;
}

float max_speed(){
  driver.setOutput(1); 
}

float zero_speed(){
  driver.setOutput(0); 

}
