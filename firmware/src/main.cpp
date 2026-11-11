#include <Arduino.h>
#include "driver/i2s.h"

// Microphone pins
#define PIN_WS 40
#define PIN_SD 41
#define PIN_SCK 42

// Amp pins
#define PIN_BCLK 4
#define PIN_LRC 5
#define PIN_DIN 6
#define AMP_CHANNELS 2

// Recording settings
#define SAMPLE_RATE 16000
#define AUDIO_DURATION_MS 2000
#define NUM_SAMPLES (SAMPLE_RATE * AUDIO_DURATION_MS / 1000)
#define SAMPLES_BEFORE_BEEP (NUM_SAMPLES / 2)

// Tone constants
#define TONE_FREQUENCY 1000
#define TONE_DURATION_MS 200
#define TONE_AMPLITUDE 3000
#define TONE_FRAMES (SAMPLE_RATE * TONE_DURATION_MS / 1000)

int32_t *audio_buffer;
int16_t tone_buffer[TONE_FRAMES * AMP_CHANNELS];

/* Helper functions */
void play_tone() {
  size_t bytes_written = 0;
  i2s_write(I2S_NUM_1, tone_buffer, sizeof(tone_buffer), &bytes_written, portMAX_DELAY);
}

size_t record(size_t start, size_t sample_count) {
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, audio_buffer + start, sample_count * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  return bytes_read / sizeof(int32_t);
}

void send_samples(size_t sample_count) {
  for (size_t i = 0; i < sample_count; i++) {
    Serial.println(audio_buffer[i] >> 8); // 24-bit sample sits at the top of a 32-bit slot
  }
}

void setup() {
  Serial.begin(921600);

  /* I2S settings to read the INMP441 microphone */
  i2s_config_t i2s_config_mic = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 256
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config_mic, 0, NULL);
  const i2s_pin_config_t pin_config_mic = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = PIN_SCK,
    .ws_io_num = PIN_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PIN_SD
  };
  i2s_set_pin(I2S_NUM_0, &pin_config_mic);

  /* I2S settings to play sound through the MAX98357A amplifier */
  i2s_config_t i2s_config_amp = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 256,
      .use_apll = false,
      .tx_desc_auto_clear = true
  };
  i2s_driver_install(I2S_NUM_1, &i2s_config_amp, 0, NULL);
  const i2s_pin_config_t pin_config_amp = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = PIN_BCLK,
    .ws_io_num = PIN_LRC,
    .data_out_num = PIN_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_set_pin(I2S_NUM_1, &pin_config_amp);

  // Allocate audio buffer in PSRAM
  audio_buffer = (int32_t *)heap_caps_malloc(NUM_SAMPLES * sizeof(int32_t), MALLOC_CAP_SPIRAM);
  if (audio_buffer == NULL) {
    Serial.println("Failed to allocate audio buffer in PSRAM");
  }

  // Generate a 1kHz tone for 200ms
  for (size_t i = 0; i < TONE_FRAMES; i++) {
    int16_t sample = (int16_t)(TONE_AMPLITUDE * sin(2*PI * TONE_FREQUENCY * i / SAMPLE_RATE));
    // same sample in both channels, so it works with any SD setting
    tone_buffer[i * 2] = sample; 
    tone_buffer[i * 2 + 1] = sample; 
  }
}

void loop() {
  int command = Serial.read();

  if (command == 'r'){
    if (audio_buffer == NULL) {
      Serial.println("ERROR");
      return;
    }

    Serial.println("START");
    size_t samples_read = record(0, SAMPLES_BEFORE_BEEP);
    play_tone();
    samples_read += record(SAMPLES_BEFORE_BEEP, NUM_SAMPLES - SAMPLES_BEFORE_BEEP);
    send_samples(samples_read);
    Serial.println("END");
  }

  else if (command == 'p'){
    play_tone();
    Serial.println("PLAYING TONE");
  }
}
