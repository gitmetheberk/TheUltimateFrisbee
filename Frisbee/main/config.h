#ifndef config_h
#define config_h

// Debug config
#define DEBUG_SERIAL true
#define GPS_FIX_REQUIRED false

// General config
#define MAX_BUFFER_SIZE 2500
#define INITIAL_SAMPLING_DELAY 100
#define SAMPLING_DELAY 250
#define INITIAL_DATA_POINTS 50
#define FLIGHT_TERMINATION_TOLERANCE 8

// GPS Config
#define MIN_SATELLITES 3

// MPU Config
#define GYRO_EQUIVALENCE_DEADZONE 1

// MPU calibration
#define MPU_OFFSET_AX 0.06
#define MPU_OFFSET_AY -0.01
#define MPU_OFFSET_AZ 11.22
#define MPU_OFFSET_GX -0.09
#define MPU_OFFSET_GY 0.03
#define MPU_OFFSET_GZ 0.00


#endif
