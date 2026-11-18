import sys
import numpy as np
import matplotlib.pyplot as plt

# Must match firmware
SAMPLE_RATE = 16000
CHIRP_FREQUENCY_START = 2000
CHIRP_FREQUENCY_END = 6000
CHIRP_DURATION_MS = 50
CHIRP_DURATION_S = CHIRP_DURATION_MS / 1000
CHIRP_SAMPLES = SAMPLE_RATE * CHIRP_DURATION_MS // 1000

recording = np.load(sys.argv[1])
recording = recording - recording.mean()  # remove DC offset

reference_times = np.arange(CHIRP_SAMPLES) / SAMPLE_RATE
frequency_slope = (CHIRP_FREQUENCY_END - CHIRP_FREQUENCY_START) / CHIRP_DURATION_S  # Hz per second
waves_passed = CHIRP_FREQUENCY_START * reference_times + frequency_slope / 2 * reference_times ** 2
reference = np.sin(2 * np.pi * waves_passed)

match_scores = np.correlate(recording, reference, mode="valid")
reference_norm = np.linalg.norm(reference)
window_norms = np.array([
    np.linalg.norm(recording[k : k + CHIRP_SAMPLES])
    for k in range(len(match_scores))
])
ncc_scores = match_scores / (window_norms * reference_norm + 1e-12)  # add small value to avoid division by zero

chirp_start_sample = np.argmax(np.abs(ncc_scores))
chirp_start_time = chirp_start_sample / SAMPLE_RATE
print(f"Chirp starts at sample number: {chirp_start_sample} ({chirp_start_time:.4f} s), "
      f"NCC score: {ncc_scores[chirp_start_sample]:.4f}"
)

recording_times = np.arange(len(recording)) / SAMPLE_RATE
score_times = np.arange(len(ncc_scores)) / SAMPLE_RATE

plt.subplot(2, 1, 1)
plt.plot(recording_times, recording)
plt.axvline(chirp_start_time, color="red", alpha=0.4)
plt.title("Recording with detected chirp start")
plt.ylabel("Amplitude")
plt.grid()

plt.subplot(2, 1, 2, sharex=plt.gca())
plt.plot(score_times, ncc_scores)
plt.axvline(chirp_start_time, color="red", alpha=0.4)
plt.title("NCC scores")
plt.xlabel("Time (s)")
plt.grid()

plt.tight_layout()
plt.show()