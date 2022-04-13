// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(9600);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin(MPU6050_I2CADDR_DEFAULT, &Wire1)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

  double sumAX = 0;
  double sumAY = 0;
  double sumAZ = 0;
  double sumGX = 0;
  double sumGY = 0;
  double sumGZ = 0;

int datapoints = 0;

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  sumAX += a.acceleration.x;
  sumAY += a.acceleration.y;
  sumAZ += a.acceleration.z;

  sumGX += g.gyro.x;
  sumGY += g.gyro.y;
  sumAZ += g.gyro.z;

  datapoints += 1;

  if (datapoints % 100 == 0)
  {
    Serial.print("Collected point ");
    Serial.println(datapoints);
  }

  if (datapoints == 500)
  {
    // Average and print
    Serial.println("Calibration results:");
    
    Serial.print("#define MPU_OFFSET_AX ");
    Serial.println(-sumAX / datapoints);

    Serial.print("#define MPU_OFFSET_AY ");
    Serial.println(-sumAY / datapoints);

    Serial.print("#define MPU_OFFSET_AZ ");
    Serial.println(-sumAZ / datapoints);

    Serial.print("#define MPU_OFFSET_GX ");
    Serial.println(-sumGX / datapoints);

    Serial.print("#define MPU_OFFSET_GY ");
    Serial.println(-sumGY / datapoints);

    Serial.print("#define MPU_OFFSET_GZ ");
    Serial.println(-sumGZ / datapoints);

    while (true) {delay(1000);}
  }

 delay(250);
}
