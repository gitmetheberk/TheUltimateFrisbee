from bluetooth_receiver import BluetoothReceiver
import time
import pandas as pd
import numpy as np
from IPython.display import display
import tkinter as tk
import matplotlib

matplotlib.use('TkAgg')

from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg,
    NavigationToolbar2Tk
)



DEGREES_TO_FEET = 364567.2
DEGREES_TO_METERS = 364567.2 * 0.3048

MPU_data = []
MPU_header = ["ACC_X","ACC_Y","ACC_Z","GYRO_X","GYRO_Y","GYRO_Z","TEMP"]
GPU_data = []
GPU_header = ["NEW_DATA","HAS_FIX","SATELLITES","LATITUDE","LONGITUDE","SPEED","ALTITUDE","GPS_MILLISECONDS","GPS_SECONDS","GPS_MINUTES","GPS_HOURS","MILLIS"]
lat_change = []
lon_change = []
distances = []

interface = BluetoothReceiver()

class Graph(tk.Frame):
    def __init__(self, master=None, title="", *args, **kwargs):
        super().__init__(master, *args, **kwargs)
        self.fig = Figure(figsize=(4, 3), layout='constrained')
        self.ax = self.fig.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        self.canvas.draw()
        tk.Label(self, text=f"Graph {title}").grid(row=0)
        self.canvas.get_tk_widget().grid(row=1, sticky="nesw")
        toolbar_frame = tk.Frame(self)
        toolbar_frame.grid(row=2, sticky="ew")
        NavigationToolbar2Tk(self.canvas, toolbar_frame)
        #self.fig.tight_layout()

    def setData(self, xData, yData):
        self.xData = xData
        self.yData = yData

    def setAxisLabels(self, xLabel, yLabel):
        self.ax.set_xlabel(xLabel)
        self.ax.set_ylabel(yLabel)

    def plot(self):
        self.ax.plot(self.xData, self.yData, color='#75bbf0')

    def draw(self):
        self.canvas.draw()

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('Ultimate Frisbee Data Collection')
        while True:
            data = interface.collectData()
            # Make tkinter window
            #window = tkinter.Tk()
            # Rename title of window


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
                GPU_df['DISTANCE'] = np.sqrt(GPU_df['LAT_CHANGE']*GPU_df['LAT_CHANGE'] + GPU_df['LON_CHANGE']*GPU_df['LON_CHANGE']).apply(lambda x: x * DEGREES_TO_METERS)
                GPU_df['CUM_DIST'] = GPU_df['DISTANCE'].cumsum()
                display(GPU_df)
                '''
                figure = Figure(figsize=(6, 4), dpi=100)

                # create FigureCanvasTkAgg object
                figure_canvas = FigureCanvasTkAgg(figure, self)

                # create the toolbar
                NavigationToolbar2Tk(figure_canvas, self)

                # create axes
                axes = figure.add_subplot()

                # create the barchart
                axes.plot(GPU_df.MILLIS, MPU_df.GYRO_X)
                axes.set_title('GYRO_X over time')
                axes.set_xlabel('Time (ms)')
                axes.set_ylabel('Gyro_x (rad/s)')

                figure_canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
                '''
                '''
                # create FigureCanvasTkAgg object
                figure_canvas = FigureCanvasTkAgg(figure, self)

                # create the toolbar
                NavigationToolbar2Tk(figure_canvas, self)

                # create axes
                axes = figure.add_subplot()

                # create the barchart
                axes.plot(GPU_df.MILLIS, MPU_df.GYRO_X)
                axes.set_title('GYRO_X over time')
                axes.set_xlabel('Time (ms)')
                axes.set_ylabel('Gyro_x (rad/s)')

                figure_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=1)
                break
                '''
                # Make graphs
                graphGYRO_X = Graph(self, title="GYRO_X over time", width=200)
                graphGYRO_X.grid(row=0, column=0)
                graphGYRO_X.setData(GPU_df.MILLIS, MPU_df.GYRO_X)
                graphGYRO_X.setAxisLabels("Time (ms)", "GYRO_X (rad/s)")
                graphGYRO_X.plot()
                graphGYRO_X.draw()

                graphGYRO_Y = Graph(self, title = "GYRO_Y over time", width = 200)
                graphGYRO_Y.grid(row = 0, column =1)
                graphGYRO_Y.setData(GPU_df.MILLIS, MPU_df.GYRO_Y)
                graphGYRO_Y.setAxisLabels("Time (ms)", "GYRO_Y (rad/s)")
                graphGYRO_Y.plot()
                graphGYRO_Y.draw()

                graphGYRO_Z = Graph(self, title = "GYRO_Z over time", width = 200)
                graphGYRO_Z.grid(row = 0, column =2)
                graphGYRO_Z.setData(GPU_df.MILLIS, MPU_df.GYRO_Z)
                graphGYRO_Z.setAxisLabels("Time (ms)", "GYRO_Z (rad/s)")
                graphGYRO_Z.plot()
                graphGYRO_Z.draw()

                graphSPEED = Graph(self, title = "Speed over time", width = 200)
                graphSPEED.grid(row = 1, column =0)
                graphSPEED.setData(GPU_df.MILLIS, GPU_df.SPEED)
                graphSPEED.setAxisLabels("Time (ms)", "Speed (m/s)")
                graphSPEED.plot()
                graphSPEED.draw()

                graphDISTANCE = Graph(self, title = "Distance over time", width = 200)
                graphDISTANCE.grid(row = 1, column =1)
                graphDISTANCE.setData(GPU_df.MILLIS, GPU_df.CUM_DIST)
                graphDISTANCE.setAxisLabels("Time (ms)", "Distance Traveled (m)")
                graphDISTANCE.plot()
                graphDISTANCE.draw()

                graphMAP = Graph(self, title = "Bird's Eye View of Throw", width = 200)
                graphMAP.grid(row = 2, column =1)
                graphMAP.setData(GPU_df.LATITUDE, GPU_df.LONGITUDE)
                graphMAP.setAxisLabels("Latitude", "Longitude")
                graphMAP.ax.plot(GPU_df['LATITUDE'].iloc[0], GPU_df['LONGITUDE'].iloc[0], marker="o", markersize=7, markerfacecolor="green")
                graphMAP.ax.plot(GPU_df['LATITUDE'].iloc[-1], GPU_df['LONGITUDE'].iloc[-1], marker="o", markersize=7, markerfacecolor="red")
                graphMAP.ax.set_yticklabels([])
                graphMAP.ax.set_xticklabels([])
                graphMAP.plot()
                graphMAP.draw()

                graphELEVATION = Graph(self, title = "Elevation over time", width = 200)
                graphELEVATION.grid(row = 1, column =2)
                graphELEVATION.setData(GPU_df.MILLIS, GPU_df.ALTITUDE)
                graphELEVATION.setAxisLabels("Time (ms)", "Elevation (m)")
                graphELEVATION.plot()
                graphELEVATION.draw()
                break
            time.sleep(1)
            #window.mainloop()

if __name__ == '__main__':
    app = App()
    app.mainloop()