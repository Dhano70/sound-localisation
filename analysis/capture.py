import serial
import time

PORT = "/dev/cu.usbmodem5CE51435351"
BAUD_RATE = 921600

with serial.Serial(PORT, BAUD_RATE, timeout=5, exclusive=True) as ser:
    time.sleep(2)  
    ser.reset_input_buffer()
    ser.write(b"r") 

    while True:
        line = ser.readline().rstrip()
        if line == b"END" or line == b"":
            break
        print(line)