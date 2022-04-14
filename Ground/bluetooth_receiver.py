import serial

# Config
TEST_MODE = False
DEMO_FILE = "Testfiles/"
PORT = 'COM5'

class BluetoothReceiver:
    def collectData(self):
        incomingPacket = []

        if not TEST_MODE:
            bluetoothSerialPort = None
            try:
                bluetoothSerialPort = serial.Serial(PORT, 9600)
            except:
                return None

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



        return incomingPacket

