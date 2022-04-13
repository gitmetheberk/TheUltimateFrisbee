from bluetooth_receiver import BluetoothReceiver
import time

interface = BluetoothReceiver()

while (True):
    data = interface.collectData()

    if data is not None:
        # if data is not none, you have received some data

        for List in data:
            print(List)

    time.sleep(1)