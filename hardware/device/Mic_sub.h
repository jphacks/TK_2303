#pragma once
#include <I2S.h>

namespace mic {
const int buff_size = 128;
int available, read;
int16_t buffer[buff_size];

void micTask(void *param) {
  while (true) {
    available = I2S.available();
    if (available < buff_size) {
      read = I2S.read(buffer, available);
    } else {
      read = I2S.read(buffer, buff_size);
    }
    vTaskDelay(1);
    for (int i = 0; i < read; i++) {
      Serial.print("-32768,");
      Serial.print(buffer[i]);
      Serial.println(",32768");
    }
  }
}

void setup() {
  if (!I2S.begin(PDM_MONO_MODE, 8000, 16)) {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ;  // do nothing
  }
  xTaskCreatePinnedToCore(micTask, "MicTask", 0x1000, NULL, 2, NULL, 1);
}
}
