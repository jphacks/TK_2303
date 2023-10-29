#pragma once
#include <driver/i2s.h>

#define I2S_PIN_WS 25
#define I2S_PIN_DIN 26

#define I2S_SAMPLE_RATE 32000
#define I2S_BUFFER_COUNT 4
#define I2S_BUFFER_SIZE 64

namespace mic {
const i2s_port_t i2s_port = I2S_NUM_0;
const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
  .sample_rate = I2S_SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
  .communication_format = I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = I2S_BUFFER_COUNT,
  .dma_buf_len = I2S_BUFFER_SIZE,
  .use_apll = false,
  .fixed_mclk = 0,
};
const i2s_pin_config_t pin_config = {
  .bck_io_num = I2S_PIN_NO_CHANGE,
  .ws_io_num = I2S_PIN_WS,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_PIN_DIN,
};

uint8_t samples[I2S_BUFFER_SIZE];

const static int volume = 16;
const static int val_offset = 0;
float average = 0;

bool big_sound = false;

void micTask(void *param) {
  while (true) {
    int16_t ret;
    size_t num_bytes_read;
    esp_err_t err = i2s_read(i2s_port,
                             (char *)samples,
                             I2S_BUFFER_SIZE,
                             &num_bytes_read,
                             portMAX_DELAY);
    if (err == ESP_OK && num_bytes_read == I2S_BUFFER_SIZE) {
      for (int i = 0; i < num_bytes_read; i += 4) {
        int32_t *val = (int32_t *)&samples[i];
        *val = (*val >> volume) + val_offset;
        // Serial.print("-10000,");
        // Serial.print(*val - average);
        // Serial.println(",10000\n");
        average = average * 0.999 + (*val) * 0.001;
        if (abs(average - (*val)) > 2000) {
          big_sound = true;
        }
      }
    }
    vTaskDelay(10);
    // for (int i = 0; i < read; i++) {
    //   Serial.print("-32768,");
    //   Serial.print(buffer[i]);
    //   Serial.println(",32768");
    // }
  }
}

void setup() {
  esp_err_t _err = i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
  if (_err != ESP_OK) {
    Serial.printf("Failed install %d\n", _err);
    while (true) {
    };
  }
  _err = i2s_set_pin(i2s_port, &pin_config);
  if (_err != ESP_OK) {
    Serial.printf("Failed set pin %d\n", _err);
    while (true) {
    };
  }
  Serial.println("I2S driver installed");

  xTaskCreatePinnedToCore(micTask, "MicTask", 0x1000, NULL, 2, NULL, 1);
}
}
