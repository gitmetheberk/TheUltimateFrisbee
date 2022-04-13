#include <Adafruit_GPS.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "enums.h"
#include "pins.h"
#include "config.h"
#include "data.h"

states state = boot;
String errorMessage;

Adafruit_GPS GPS(&GPS_Serial);
Adafruit_MPU6050 MPU;

dataBuffer db;

void setup() {
  // Pin init
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(BUTTON, INPUT);
  
  digitalWrite(LED_RED, HIGH);

  #if DEBUG_SERIAL
  Serial.begin(9600);
  while (!Serial) {delay(25);}
  Serial.println("Serial Online");
  #endif

  // Bluetooth init
  bluetooth.begin(9600);

  delay(250);
  if (!bluetooth)
  {
    errorMessage = "Could not establish serial connection with bluetooth";
    return;
  }

  // MPU init
  if (!MPU.begin(MPU6050_I2CADDR_DEFAULT, &MPU_Wire))
  {
    errorMessage = "Could not establish serial connection with MPU";
    return;
  }
  
  MPU.setAccelerometerRange(MPU6050_RANGE_8_G);
  MPU.setGyroRange(MPU6050_RANGE_500_DEG);
  MPU.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // GPS init
  GPS.begin(9600);
  
  delay(250);
  if (!GPS_Serial)
  {
    errorMessage = "Could not establish serial connection with GPS";
    return;
  }
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);

  // TODO Tune time to wait for fix
  if (GPS_FIX_REQUIRED)
  {
    #if DEBUG_SERIAL
    Serial.println("Waiting for GPS fix");
    #endif
    
    const int loopLimit = 100;
    for (int loop = 0; loop < loopLimit; loop++)
    {
      if (GPS.fix)
      {
        break;
      }
      else
      {
        delay(1000);
      }
    }

    if (!GPS.fix)
    {
      errorMessage = "Unable to establish GPS fix";
      return;
    }

    #if DEBUG_SERIAL
    Serial.println("GPS fix established");
    #endif
  }
  
  db = dataBuffer();
  
  #if DEBUG_SERIAL
  Serial.println("Boot complete");
  #endif

  #if !GPS_FIX_REQUIRED
  delay(2000);
  #endif
  
  state = ready_to_collect;
}

void loop() {
  if (state == error)
  {
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_WHITE, LOW);

#if DEBUG_SERIAL
    Serial.print("An error occured: ");
    Serial.println(errorMessage);
#endif

    digitalWrite(LED_RED, HIGH);
    delay(1000);
    digitalWrite(LED_RED, LOW);
    delay(1000);
  }
  else if (state == boot)
  {
    // Something went wrong, boot did not complete successfully
    state = error;
  }
  else if (state == ready_to_collect)
  {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    
    // Read button press
    int buttonState;
    buttonState = digitalRead(BUTTON);
    while (buttonState != HIGH)
    {
      digitalWrite(LED_WHITE, HIGH);
      delay(200);
      digitalWrite(LED_WHITE, LOW);
      delay(200);
      buttonState = digitalRead(BUTTON);
    }

    state = collecting_initial;
  }
  else if (state == collecting_initial)
  {
    digitalWrite(LED_WHITE, HIGH);

    // TODO Include date in buffer reset
    db.resetBuffer();
    
    state = collecting;
  }
  else if (state == collecting)
  {
    // TODO Write data collection functions
    // TODO Log data until
    //  a. Max data points reached
    //  b. Altitude static, gyro relatively static



    // TEMP
    delay(5000);
    state = awaiting_transmission;
  }
  else if (state == awaiting_transmission)
  {
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_YELLOW, HIGH);

    // Check for com established, delay 5 seconds, then transmit
    // TODO

    // TEMP
    delay(5000);
    state = ready_to_collect;
  }
}
