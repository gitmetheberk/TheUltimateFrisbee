import serial

with serial.Serial('COM5', 9600) as serialIn:
    while True:
        x = serialIn.readline()
        print(x)