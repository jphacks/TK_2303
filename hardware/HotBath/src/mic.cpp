#include "mic.hpp"
#include "sensor.hpp"
#include <driver/i2s.h>

#define I2S_PIN_CLK 8  // BCLK
#define I2S_PIN_WS 15  // LRCLK
#define I2S_PIN_DIN 16

#define I2S_SAMPLE_RATE 8000
#define I2S_BUFFER_COUNT 4
#define I2S_BUFFER_SIZE 1024

namespace mic
{
static uint8_t samples[I2S_BUFFER_SIZE];
static constexpr i2s_port_t i2s_port = I2S_NUM_0;
static const char TAG[] = "MIC";

xSemaphoreHandle mux;

MicLock::MicLock()
{
    xSemaphoreTake(mux, portMAX_DELAY);
}
MicLock::~MicLock()
{
    xSemaphoreGive(mux);
}

void mic_task(void* pvParameters)
{
    while (1) {
        {
            MicLock lock;
            size_t num_bytes_read;  // data length
            esp_err_t err = i2s_read(i2s_port,
                (uint8_t*)samples,
                I2S_BUFFER_SIZE,
                &num_bytes_read,
                portMAX_DELAY);  // no timeout
            if (err == ESP_OK) {
                int64_t val_sum = 0;
                for (int i = 0; i < num_bytes_read; i += 4) {
                    int32_t* val = (int32_t*)&samples[i];
                    val_sum += abs(*val);
                }
            }
        }
        vTaskDelay(50);
    }
}

void init()
{
    sensor::I2CLock lock;

    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,                     // kHz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,       // 32 bits only
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,        // monaural (stereo: I2S_CHANNEL_FMT_RIGHT_LEFT)
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,  // I2S
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,           // interrupt level 1
        .dma_buf_count = I2S_BUFFER_COUNT,                  // number of buffers
        .dma_buf_len = I2S_BUFFER_SIZE,                     // samples per buffer
        .use_apll = false,                                  // external clock source (APLL not used)
        .fixed_mclk = 0,                                    // APLL clock
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_CLK,          // BCLK
        .ws_io_num = I2S_PIN_WS,            // LRCLK
        .data_out_num = I2S_PIN_NO_CHANGE,  // not used
        .data_in_num = I2S_PIN_DIN,         // DOUT
    };

    i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_port, &pin_config);

    mux = xSemaphoreCreateMutex();
    // xTaskCreatePinnedToCore(mic_task, "mic_task", 4096, NULL, 1, NULL, 1);
}

void record_to_wav(WAVWriter* wav_writer)
{
    MicLock lock;
    ESP_LOGI(TAG, "Record started");

    bool flag = true;
    while (flag) {
        size_t num_bytes_read;  // data length
        // read
        esp_err_t err = i2s_read(i2s_port,
            (uint8_t*)samples,
            I2S_BUFFER_SIZE,
            &num_bytes_read,
            portMAX_DELAY);  // no timeout
        if (err == ESP_OK) {
            for (int i = 0; i < num_bytes_read; i += 4) {
                int32_t* val = (int32_t*)&samples[i];
                int32_t val2 = constrain((*val) * 4, -2147483648, 2147483647);
                int16_t tmp = (int16_t)(val2 >> 16);
                // Serial.println((int)tmp);
                if (!wav_writer->write((uint8_t*)&tmp, 2)) {
                    flag = false;
                    break;
                }
            }
        }
    }
    ESP_LOGI(TAG, "Record stopped.");
    wav_writer->finish();
}

}  // namespace mic