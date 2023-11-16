#include "esp_log.h"
#include "wav.hpp"

static const char* TAG = "WAV";

uint16_t wav_buffer[WAV_BUFFER_SIZE];

WAVWriter::WAVWriter(uint8_t* buffer, size_t buffer_size, int sample_rate, int bit_depth)
{
    m_buffer = buffer;
    m_buffer_size = buffer_size;
    m_file_size = sizeof(wav_header_t);

    m_header.sample_rate = sample_rate;
    m_header.bit_depth = bit_depth;
}

bool WAVWriter::write(uint8_t* samples, size_t size)
{
    if (m_file_size + size > m_buffer_size) {
        ESP_LOGE(TAG, "buffer overflow!");
        return false;
    }
    memcpy(m_buffer + m_file_size, samples, size);
    m_file_size += size;
    return true;
}

void WAVWriter::finish()
{
    ESP_LOGI(TAG, "Finishing wav file size: %d", m_file_size);

    short bytes_per_sample = m_header.bit_depth / 8;
    m_header.byte_rate = m_header.sample_rate * m_header.num_channels * bytes_per_sample;
    m_header.sample_alignment = m_header.num_channels * bytes_per_sample;

    m_header.data_bytes = m_file_size - sizeof(wav_header_t);
    m_header.wav_size = m_file_size - 8;
    memcpy(m_buffer, &m_header, sizeof(wav_header_t));
}