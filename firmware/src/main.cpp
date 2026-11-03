#include <Arduino.h>
#include "driver/i2s.h"

// microhphone pins
#define PIN_WS 40
#define PIN_SD 41
#define PIN_SCK 42

#define SAMPLE_RATE 16000



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
}

void loop() {
  if (Serial.read() != 'r'){
    return;
  }

  Serial.println("START");
  Serial.println("END");
}
