#include <Arduino.h>
#include "driver/i2s.h"

// microphone pins
#define PIN_WS 40
#define PIN_SD 41
#define PIN_SCK 42

#define SAMPLE_RATE 16000
#define AUDIO_SECONDS 2
#define NUM_SAMPLES (SAMPLE_RATE * AUDIO_SECONDS)

int32_t *audio_buffer;

void setup() {
  Serial.begin(921600);

  /* I2S settings to read the INMP441 microphone */
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 256
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = PIN_SCK,
    .ws_io_num = PIN_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PIN_SD
  };

  i2s_set_pin(I2S_NUM_0, &pin_config);

  audio_buffer = (int32_t *)heap_caps_malloc(NUM_SAMPLES * sizeof(int32_t), MALLOC_CAP_SPIRAM);
  if (audio_buffer == NULL) {
    Serial.println("Failed to allocate audio buffer in PSRAM");
  }
}

void loop() {
  if (Serial.read() != 'r'){
    return;
  }

  if (audio_buffer == NULL) {
    Serial.println("ERROR");
    return;
  }

  Serial.println("START");

  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, audio_buffer, NUM_SAMPLES * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  Serial.println("Samples read: " + String(bytes_read / sizeof(int32_t)));

  Serial.println("END");
}
