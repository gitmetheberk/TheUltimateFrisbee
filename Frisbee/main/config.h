#ifndef config_h
#define config_h

// Debug config
#define DEBUG_SERIAL false
#define DEBUG_BLUETOOTH false
#define GPS_FIX_REQUIRED true

// General config
#define MAX_BUFFER_SIZE 5000
#define INITIAL_SAMPLING_DELAY 100
#define SAMPLING_DELAY 250
#define INITIAL_DATA_POINTS 50
#define FLIGHT_TERMINATION_TOLERANCE 8

// GPS Config (TODO Not implemented)
#define MIN_SATELLITES 3

// MPU Config
#define GYRO_EQUIVALENCE_DEADZONE 0.1

// MPU calibration
#define MPU_OFFSET_AX -0.25
#define MPU_OFFSET_AY -0.06
#define MPU_OFFSET_AZ -11.27
#define MPU_OFFSET_GX 0.09
#define MPU_OFFSET_GY -0.02
#define MPU_OFFSET_GZ 0.00


#endif
