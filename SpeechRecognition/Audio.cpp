#include "Audio.h"
#include <stdint.h>
#include <stdlib.h>

Audio::Audio(uint32_t sampleRate, uint8_t channelCount, uint8_t sampleBitLength)
    : m_codec(I2C_SDA, I2C_SCL, 0x1A, /*fake pin names*/ D8, D9, D10, D11, D12)
{
    m_sample_rate = sampleRate;
    m_bit_depth = sampleBitLength;
    m_channels = channelCount;

    // Initialize codec moduel
    m_codec.format(m_sample_rate, m_channels, m_bit_depth);
}

/*
** Re-configure the audio data format
*/
void Audio::format(uint32_t sampleRate, uint8_t channelCount, uint8_t sampleBitLength)
{
    m_sample_rate = sampleRate;
    m_channels = channelCount;
    m_bit_depth = sampleBitLength;
    
    m_codec.format(m_sample_rate, m_channels, m_bit_depth);
}

/*
** Start recording audio data usine underlying codec
*/
void Audio::startRecord(char * audioFile, int fileSize, uint8_t durationInSeconds)
{
    if (audioFile == NULL) {
        // TODO: log error
        return;
    }

    if (fileSize < WAVE_HEADER_SIZE) {
        // TODO: log error
        return;
    }

    m_wavFile = audioFile;
    m_file_size = fileSize;
    m_duration = durationInSeconds;
    m_record_cursor = audioFile + WAVE_HEADER_SIZE;

    // attach callback function to read audio data from codec
    m_codec.attach(this, &Audio::readMic);

    printf("Start recording...\r\n");
    m_codec.startRecord();
}

/*
** Get wave file
*/
char* Audio::getWav(int *file_size)
{
    int currentSize = m_record_cursor - m_wavFile;
    *file_size  = (int)currentSize;

    return m_wavFile;
}

void Audio::stop()
{
    printf("Stop recording.\r\n");
    m_codec.stop();
}


/*
** Read microphone data from codec
*/
void Audio::readMic(void)
{
    // If m_record_cursor exceeds given buffer size, then stop recording
    if (m_record_cursor >= m_wavFile + m_file_size - 16 ) return;

    // read audion data into codex buffer
    m_codec.read();

    // Copy audio data from codec to given user buffer.
    for (int i = 0; i < 4; i++)
    {
        int data = m_codec.rxBuffer[i];
        memcpy(m_record_cursor, &data, 4);     // Need to check data endian
        m_record_cursor += 4;
    }
}

WaveHeader* Audio::genericWAVHeader()
{
    WaveHeader *hdr;
    hdr = (WaveHeader *)malloc(sizeof(*hdr));
    if (!hdr) return NULL;

    memcpy(&hdr->RIFF_marker, "RIFF", 4);
    memcpy(&hdr->filetype_header, "WAVE", 4);
    memcpy(&hdr->format_marker, "fmt ", 4);
    hdr->data_header_length = 16;
    hdr->format_type = 1;
    hdr->number_of_channels = m_channels;
    hdr->sample_rate = m_sample_rate;
    hdr->bytes_per_second = m_sample_rate * m_channels * m_bit_depth / 8;
    hdr->bytes_per_frame = m_channels * m_bit_depth / 8;
    hdr->bits_per_sample = m_bit_depth;
    hdr->file_size = hdr->sample_rate * hdr->bytes_per_frame * m_duration + 36;

    return hdr;
}

int Audio::writeWAVHeader(char * wavFile, WaveHeader *hdr)
{
    if (!hdr)
        return -1;

    memcpy(wavFile, &hdr->RIFF_marker, 4);
    memcpy(wavFile + 4, &hdr->file_size, 4);
    memcpy(wavFile + 8, &hdr->filetype_header, 4);
    memcpy(wavFile + 12, &hdr->format_marker, 4);
    memcpy(wavFile + 16, &hdr->data_header_length, 4);
    memcpy(wavFile + 20, &hdr->format_type, 2);
    memcpy(wavFile + 22, &hdr->number_of_channels, 2);
    memcpy(wavFile + 24, &hdr->sample_rate, 4);
    memcpy(wavFile + 28, &hdr->bytes_per_second, 4);
    memcpy(wavFile + 32, &hdr->bytes_per_frame, 2);
    memcpy(wavFile + 34, &hdr->bits_per_sample, 2);
    memcpy(wavFile + 36, "data", 4);
    uint32_t data_size = hdr->file_size - 36;
    memcpy(wavFile + 40, &data_size, 4);

    return 0;
}
