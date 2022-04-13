#ifndef data_h
#define data_h

#include "config.h"

struct mpu_data
{
  float accX;
  float accY;
  float accZ;

  float gyroX;
  float gyroY;
  float gyroZ;

  float tempC;
};

struct dataDate
{
  short day;
  short month;
  short year;
};

struct gps_data
{
  bool newData;
  bool hasFix;
  int satellites;
  
  float latitude;
  float longitude;
  
  float speedKnots;
  float altitudeFeet;

  short milliseconds;
  short seconds;
  short minutes;
  short hours;
};

class dataBuffer
{
private:
  unsigned int dataPointsCollected;
  dataDate date;

  // Data arrays
  mpu_data mpu_buffer[MAX_BUFFER_SIZE];
  gps_data gps_buffer[MAX_BUFFER_SIZE];

public:
  dataBuffer()
  {
    // Could overwrite buffers, but there's no reason to
    
    dataPointsCollected = 0;
    date.day = 0;
    date.month = 0;
    date.year = 0;
  }

  bool writeMPU(const mpu_data &d)
  {
    if (bufferFull()) {return false;}
    mpu_buffer[dataPointsCollected] = d;
    return true;
  }

  bool writeGPS(const gps_data &d)
  {
    if (bufferFull()) {return false;}
    gps_buffer[dataPointsCollected] = d;
    return true;
  }

  int incrementBuffer()
  {
    if (bufferFull()) {return MAX_BUFFER_SIZE;}
    return dataPointsCollected++;
  }

  void resetBuffer()
  {
    dataPointsCollected = 0;
  }

  void resetBuffer(dataDate d)
  {
    dataPointsCollected = 0;
    date = d;
  }

  bool bufferFull()
  {
    return dataPointsCollected >= MAX_BUFFER_SIZE;
  }

  int currentSize()
  {
    return dataPointsCollected;
  }

  dataDate getDate()
  {
    return date;
  }

  bool readMPU(unsigned int i, mpu_data &dataOut)
  {
    if (i >= dataPointsCollected)
    {
      return false;
    }
    else
    {
      dataOut = mpu_buffer[i];
      return true;
    }
  }

  bool readGPS(unsigned int i, gps_data &dataOut)
  {
    if (i >= dataPointsCollected)
    {
      return false;
    }
    else
    {
      dataOut = gps_buffer[i];
      return true;
    }
  }  
};




#endif
