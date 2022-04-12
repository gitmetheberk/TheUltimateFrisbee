import serial

with serial.Serial('COM5', 9600) as bluetoothSerialPort:
        while True:
            stringIn = bluetoothSerialPort.readline().decode()
            print(stringIn)

            if stringIn.startswith("ComCheck"):
                bluetoothSerialPort.write(bytes("ComEstablished\n", 'utf-8'))
            elif stringIn.startswith("SendingData"):
                dataSum = 0
                data = bluetoothSerialPort.readline().decode()
                data = data.split(',')
                data = data[:-1]
                for d in data:
                    dataSum += int(d)

                print("Data sum: {}".format(dataSum))
                if dataSum == 44850:
                    print("Sum correct")

