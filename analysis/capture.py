import serial
import time
import sys
import numpy as np
import os

PORT = "/dev/cu.usbmodem5CE51435351"
BAUD_RATE = 921600

with serial.Serial(PORT, BAUD_RATE, timeout=5, exclusive=True) as ser:
    # wait for initialization and dispose of any old data in the buffer
    time.sleep(2) 
    ser.reset_input_buffer()
    
    ser.write(b"r")

    #read garbage data until we get the start signal
    while True:
        line = ser.readline().rstrip()
        if line == b"START" or line == b"":
            break

    if line == b"":
        print("no START received")
        sys.exit(1)

    print("recording...")

    samples = []

    while True:
        line = ser.readline().rstrip()
        if line == b"END" or line == b"":
            break
        if len(samples) == 0:
            print("recording finished, receiving...")
        samples.append(int(line))

    print(len(samples))

    audio_data = np.array(samples, dtype=np.int32)
    filename = f"recordings/{time.strftime('%Y-%m-%d_%H-%M-%S')}.npy"

    
    os.makedirs("recordings", exist_ok=True)
    np.save(filename, audio_data)
