#include <Adafruit_GPS.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "enums.h"
#include "pins.h"
#include "config.h"
#include "data.h"
#include "functions.cpp"

states state = boot;
String errorMessage;

Adafruit_GPS GPS(&GPS_Serial);
Adafruit_MPU6050 MPU;

dataBuffer db;

bool checkGyroEquivalence(const mpu_data &l, const mpu_data &r)
{
  if (fabs(l.gyroX - r.gyroX) > GYRO_EQUIVALENCE_DEADZONE)
  {
    return false;
  }
  else if (fabs(l.gyroY - r.gyroY) > GYRO_EQUIVALENCE_DEADZONE)
  {
    return false;
  }
  else if (fabs(l.gyroZ - r.gyroZ) > GYRO_EQUIVALENCE_DEADZONE)
  {
    return false;
  }

  return true;
}

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

  // TODO Tune GPS update rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
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

    // TODO Wait for min satellites

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

    unsigned short stationaryDatapoints = 0;
    mpu_data lastMPUData = mpu_data{0,0,0,0,0,0,0};

    while(!db.bufferFull())
    {
      // MPU
      sensors_event_t a, g, temp;
      MPU.getEvent(&a, &g, &temp);

      mpu_data newMPUData = mpu_data{
        a.acceleration.x + MPU_OFFSET_AX,
        a.acceleration.y + MPU_OFFSET_AY,
        a.acceleration.z + MPU_OFFSET_AZ,

        g.gyro.x + MPU_OFFSET_GX,
        g.gyro.y + MPU_OFFSET_GY,
        g.gyro.z + MPU_OFFSET_GZ,

        temp.temperature
      };
      
      db.writeMPU(newMPUData);

      // GPS
      char c = GPS.read();  // This line might not be needed
      if (GPS.newNMEAreceived()) {
        if (GPS.parse(GPS.lastNMEA())) 
        {
          // New parsed data, store
          db.writeGPS(gps_data{
            true,
            (bool)GPS.fix,
            GPS.satellites,
            
            GPS.latitudeDegrees,
            GPS.longitudeDegrees,

            GPS.speed,
            GPS.altitude,

            GPS.milliseconds,
            GPS.seconds,
            GPS.minute,
            GPS.hour
          });
        }
        else
        {
          // Parse failed
          db.writeGPS(gps_data{false});
        }
      }
      else
      {
        // No new data
        db.writeGPS(gps_data{false});
      }

      // TODO Check for flight termination
      if (checkGyroEquivalence(newMPUData, lastMPUData))
      {
        stationaryDatapoints  += 1;
      }
      else
      {
        stationaryDatapoints = 0;
      }

      if (stationaryDatapoints == FLIGHT_TERMINATION_TOLERANCE)
      {
        break;
      }

      lastMPUData = newMPUData;
      db.incrementBuffer();

      if (db.currentSize() <= INITIAL_DATA_POINTS)
      {
        delay(INITIAL_SAMPLING_DELAY);
      }
      else
      {
        delay(SAMPLING_DELAY);
      }
    }

    delay(1000);
    state = awaiting_transmission;
  }
  else if (state == awaiting_transmission)
  {
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_YELLOW, HIGH);

    
    
    delay(5000);
    state = ready_to_collect;
  }
}
