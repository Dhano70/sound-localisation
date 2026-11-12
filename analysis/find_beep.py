import sys
import numpy as np
import matplotlib.pyplot as plt

# Must match firmware
SAMPLE_RATE = 16000
TONE_FREQUENCY = 1000
TONE_DURATION_MS = 200
TONE_SAMPLES = SAMPLE_RATE * TONE_DURATION_MS // 1000

recording = np.load(sys.argv[1])
recording = recording - recording.mean()  # remove DC offset

reference_times = np.arange(TONE_SAMPLES) / SAMPLE_RATE
reference = np.sin(2 * np.pi * TONE_FREQUENCY * reference_times)

match_scores = np.correlate(recording, reference, mode="valid")
beep_start_sample = np.argmax(match_scores)
beep_start_time = beep_start_sample / SAMPLE_RATE
print(f"Beep starts at sample number: {beep_start_sample} with time at: {beep_start_time:.4f} seconds")

recording_times = np.arange(len(recording)) / SAMPLE_RATE
score_times = np.arange(len(match_scores)) / SAMPLE_RATE

plt.subplot(2, 1, 1)
plt.plot(recording_times, recording)
plt.axvline(beep_start_time, color="red", alpha=0.4)
plt.title("Recording with detected beep start")
plt.ylabel("Amplitude")

plt.subplot(2, 1, 2, sharex=plt.gca())
plt.plot(score_times, match_scores)
plt.axvline(beep_start_time, color="red", alpha=0.4)
plt.title("Match scores")
plt.xlabel("Time (s)")

plt.tight_layout()
plt.show()