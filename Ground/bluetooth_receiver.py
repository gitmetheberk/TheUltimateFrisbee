import serial

# Config
TEST_MODE = False
TEST_FILE = "stationary_noGPS.txt"
PORT = 'COM5'


class BluetoothReceiver:
    def collectData(self):
        incomingPacket = []

        if not TEST_MODE:
            bluetoothSerialPort = None
            try:
                bluetoothSerialPort = serial.Serial(PORT, 9600)
            except:
                print("Could not establish connection")
                return None

            print("Connection established")

            while True:
                stringIn = bluetoothSerialPort.readline().decode()

                if stringIn.startswith("ComCheck"):
                    bluetoothSerialPort.write(bytes("ComEstablished\n", 'utf-8'))
                elif stringIn.startswith("TRANSMISSION_BEGIN"):
                    while not stringIn.startswith("TRANSMISSION_END"):
                        stringIn = bluetoothSerialPort.readline().decode()
                        data = stringIn.split(',')
                        data[-1] = data[-1][:-1]

                        incomingPacket.append(data)

                    break

        else:
            # Test mode
            print("Loading incoming packet from file: {}".format(TEST_FILE))
            with open("test_data/" + TEST_FILE, 'r') as file:
                fileLine = "."
                while fileLine:
                    fileLine = file.readline()
                    incomingPacket.append(fileLine[:-1].split(','))

            incomingPacket = incomingPacket[:-1]

        return incomingPacket

    def recordTestFile(self, filename):
        if not filename.endswith(".txt"):
            filename = filename + ".txt"

        packet = self.collectData()

        with open("test_data/" + filename, 'w') as file:
            for line in packet:
                if len(line) == 1:
                    file.write(line[0] + "\n")
                else:
                    lineToWrite = ""
                    for item in line:
                        lineToWrite += item + ","

                    lineToWrite = lineToWrite[:-1]
                    file.write(lineToWrite + "\n")

        return packet