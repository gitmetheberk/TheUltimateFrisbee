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

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
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
      do {
        GPS.read();
        while (!GPS.newNMEAreceived())
        {
          GPS.read();
        }
      } while (!GPS.parse(GPS.lastNMEA()));

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

#if DEBUG_BLUETOOTH
    bluetooth.print("An error occured: ");
    bluetooth.println(errorMessage);
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
//    while(!GPS.newNMEAreceived())
//    {
//      GPS.read();
//    }
//
//    GPS.parse(GPS.lastNMEA());
//    
//    setTime(GPS.hour, GPS.minute, GPS.seconds
    
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
      do {
        GPS.read();
        while (!GPS.newNMEAreceived())
        {
          GPS.read();
        }
      } while (!GPS.parse(GPS.lastNMEA()));

      db.writeGPS(gps_data{
        true,
        (bool)GPS.fix,
        GPS.satellites,
        
        GPS.latitudeDegrees,
        GPS.longitudeDegrees,
  
        GPS.speed,
        GPS.altitude,

        // Note: GPS time unreliable, use millis time for data sequencing
        GPS.milliseconds,
        GPS.seconds,
        GPS.minute,
        GPS.hour,
        millis()
      });

      #if DEBUG_SERIAL
      Serial.println("GPS seconds: " + String(GPS.seconds));
      #endif

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

    while (true)
    {
      bluetooth.print("ComCheck\n");
      String received = bluetooth.readString();
      
      if (received.startsWith("ComEstablished"))
      {
        bluetooth.print("TRANSMISSION_BEGIN\n");

        // Transmit header information
        // Format: DAY,MONTH,YEAR,DATAPOINTS\n
        int currentSize = db.currentSize();
        bluetooth.print(String(db.getDate().day) + "," +
        String(db.getDate().month) + "," +
        String(db.getDate().year) + "," +
        String(currentSize) + "\n");

        // Transmit MPU
        // Format: ACCX, ACCY, ACCZ, GYROX, GYROY, GYROZ, TEMPC\n
        bluetooth.print("TRANSMITTING_MPU\n");
        mpu_data mpuData;
        for (int i = 0; i < currentSize; i++)
        {
          db.readMPU(i, mpuData);

          bluetooth.print(
            String(mpuData.accX, 3) + "," +
            String(mpuData.accY, 3) + "," +
            String(mpuData.accZ, 3) + "," +
            
            String(mpuData.gyroX, 3) + "," +
            String(mpuData.gyroY, 3) + "," +
            String(mpuData.gyroZ, 3) + "," +

            String(mpuData.tempC, 2) + "\n"
          );
        }
      
        // Transmit GPS
        // Format: NEWDATA,HASFIX,SATELLITES,LATITUDE,LONGITUDE,SPEEDKNOTS,ALTITUDEFEET,MILLISECONDS,SECONDS,MINUTES,HOURS,MILLIS        bluetooth.print("TRANSMITTING_GPS\n");
        gps_data gpsData;
        
        for (int i = 0; i < currentSize; i++)
        {
          db.readGPS(i, gpsData);

          bluetooth.print(
            String(gpsData.newData) + "," +
            String(gpsData.hasFix) + "," +
            String(gpsData.satellites) + "," +
            
            String(gpsData.latitude, 6) + "," +
            String(gpsData.longitude, 6) + "," +
            String(gpsData.speedKnots, 4) + "," +
            String(gpsData.altitudeFeet, 1) + "," +
            
            String(gpsData.milliseconds) + "," +
            String(gpsData.seconds) + "," +
            String(gpsData.minutes) + "," +
            String(gpsData.hours) + "," +
            String(gpsData.millisTime) + "\n"
          );
          
        }

        bluetooth.print("TRANSMISSION_END\n");
        break;
      }
    }
    
    delay(5000);
    state = ready_to_collect;
  }
}
