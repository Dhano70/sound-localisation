import serial
import time

PORT = "/dev/cu.usbmodem5CE51435351"
BAUD_RATE = 921600

with serial.Serial(PORT, BAUD_RATE, timeout=5, exclusive=True) as ser:
    time.sleep(2) 
    ser.reset_input_buffer() # dispose of any old data in the buffer
    ser.write(b"r")

    #read garbage data until we get the start signal
    while True:
        line = ser.readline().rstrip()
        if line == b"START" or line == b"":
            break

    samples = []

    while True:
        line = ser.readline().rstrip()
        if line == b"END" or line == b"":
            break
        samples.append(int(line))

    print(len(samples))