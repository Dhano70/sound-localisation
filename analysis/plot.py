import matplotlib.pyplot as plt
import numpy as np
import sys

SAMPLE_RATE = 16000

data = np.load(sys.argv[1])
data = data - data.mean()   # remove DC offset

time = np.arange(len(data)) / SAMPLE_RATE

plt.plot(time, data)
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title(f"Audio Signal: {sys.argv[1]}")
plt.grid() 
plt.show()