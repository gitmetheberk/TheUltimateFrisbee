from bluetooth_receiver import BluetoothReceiver
import time
import pandas as pd
import numpy as np
from IPython.display import display

DEGREES_TO_FEET = 364567.2

MPU_data = []
MPU_header = ["ACC_X","ACC_Y","ACC_Z","GYRO_X","GYRO_Y","GYRO_Z","TEMP"]
GPU_data = []
GPU_header = ["NEW_DATA","HAS_FIX","SATELLITES","LATITUDE","LONGITUDE","SPEED","ALTITUDE","GPS_MILLISECONDS","GPS_SECONDS","GPS_MINUTES","GPS_HOURS","MILLIS"]
lat_change = []
lon_change = []
distances = []

interface = BluetoothReceiver()


while True:
    data = interface.collectData()

    if data is not None:
        # if data is not none, you have received some data

        # for List in data:
        #     print(List)


        # split list into MPU and GPU
        MPU_data.clear()
        GPU_data.clear()
        for List in data:
            if len(List) == 7:
                MPU_data.append(List)
            elif len(List) == 12:
                GPU_data.append(List)
        

        # put data into pandas dataframe
        MPU_df = pd.DataFrame(MPU_data, columns=MPU_header)
        MPU_df = MPU_df.apply(pd.to_numeric)
        display(MPU_df)
        GPU_df = pd.DataFrame(GPU_data, columns=GPU_header)
        GPU_df = GPU_df.apply(pd.to_numeric)
        # calculate distance thrown (feet)
        GPU_df['LAT_CHANGE'] = GPU_df.LATITUDE.diff(periods=1)
        GPU_df['LON_CHANGE'] = GPU_df.LONGITUDE.diff(periods=1)
        GPU_df['LAT_CHANGE'] = GPU_df['LAT_CHANGE'].fillna(0)
        GPU_df['LON_CHANGE'] = GPU_df['LON_CHANGE'].fillna(0)
        GPU_df['DISTANCE'] = np.sqrt(GPU_df['LAT_CHANGE']*GPU_df['LAT_CHANGE'] + GPU_df['LON_CHANGE']*GPU_df['LON_CHANGE']).apply(lambda x: x * DEGREES_TO_FEET)
        GPU_df['CUM_DIST'] = GPU_df['DISTANCE'].cumsum()
        display(GPU_df)

        break

    time.sleep(1)
