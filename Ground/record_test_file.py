from bluetooth_receiver import BluetoothReceiver
import time

interface = BluetoothReceiver()

while True:
    data = interface.recordTestFile("360_degree_rotation")
    if data is not None:
        for List in data:
            print(List)

        break

    time.sleep(1)