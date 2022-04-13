#ifndef data_h
#define data_h

struct mpu_data
{
  short accX;
  short accY;
  short accZ;

  short gyroX;
  short gyroY;
  short gyroZ;

  double tempC;
};

struct dataDate
{
  short day;
  short month;
  short year;
};

struct gps_data
{
  double latitude;
  double lat;
  double longitude;
  double lon;
  
  double speed;
  double altitude;
  int satellites;

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
  mpu_data mpu_buffer[MAX_DATA_POINTS];
  gps_data gps_buffer[MAX_DATA_POINTS];

public:
  dataBuffer()
  {
    // Nothing to do really
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
    return dataPointsCollected == MAX_DATA_POINTS;
  }

  
};




#endif
