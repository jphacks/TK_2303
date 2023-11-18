#include "speaker.hpp"
#include "driver/i2s.h"
#include "mic.hpp"
#include "sensor.hpp"
#include "config.hpp"
#include <Arduino.h>
#include <string.h>

#define I2S_NUM I2S_NUM_1
#define SPEAKER_BCK 35
#define SPEAKER_WS 38
#define SPEAKER_DOUT 39

namespace speaker
{
static uint16_t soundBuf[512];
static const char TAG[] = "SPEAKER";
__NOINIT_ATTR Sound_Type sound_type = NoSound;
bool initialized = false;

static constexpr i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  // Only TX
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,  // Interrupt level 1
    .dma_buf_count = 16,
    .dma_buf_len = 128,
};
static constexpr i2s_pin_config_t pin_config = {
    .bck_io_num = SPEAKER_BCK,
    .ws_io_num = SPEAKER_WS,
    .data_out_num = SPEAKER_DOUT,
    .data_in_num = -1  // Not used
};

void set_sound_and_restart(Sound_Type type)
{
    sound_type = type;
    ESP.restart();
}

void play(const uint16_t* audio_data, size_t remain)
{
    ESP_LOGI(TAG, "play start");

    if (!initialized) {
        ESP_LOGI(TAG, "i2s_driver_install");
        i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
        i2s_set_pin(I2S_NUM, &pin_config);
        i2s_set_sample_rates(I2S_NUM, i2s_config.sample_rate);
        initialized = true;
    }

    i2s_zero_dma_buffer(I2S_NUM);

    size_t written = 0;
    const unsigned char* p = (unsigned char*)audio_data;
    const size_t bufSize = sizeof(soundBuf);

    while (remain > 0) {
        int size = (remain < bufSize) ? remain : bufSize;
        memcpy(soundBuf, p, size);

        // ボリューム調整.
        for (int i = 0; i < sizeof(soundBuf) / sizeof(soundBuf[0]); ++i) {
            int16_t v = (int16_t)soundBuf[i];
            soundBuf[i] = v * 0.05f;
        }

        i2s_write(I2S_NUM, soundBuf, size, &written, portMAX_DELAY);

        p += written;
        remain -= written;
    }
    i2s_zero_dma_buffer(I2S_NUM);

    ESP_LOGI(TAG, "play end");
}

void init_play_and_restart()
{
    if (config::is_first_boot()) {
        return;
    }

    switch (sound_type) {
    case NoSound:
        return;
        break;
    case LongTime:
        play(chime_sound, sizeof(chime_sound));
        play(long_time_sound, sizeof(long_time_sound));
        break;
    case Heat:
        play(chime_sound, sizeof(chime_sound));
        play(heat_sound, sizeof(heat_sound));
        break;
    case Alert:
        play(chime_sound, sizeof(chime_sound));
        play(alert_sound, sizeof(alert_sound));
        break;
    default:
        break;
    }
    sound_type = NoSound;
    ESP.restart();
}
}  // namespace speaker