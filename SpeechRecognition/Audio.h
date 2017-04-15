#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "mbed.h"
#include "NAU88C10.h"

#define DURATION_IN_SECONDS     2
#define DEFAULT_SAMPLE_RATE     8000
#define DEFAULT_BITS_PER_SAMPLE 8
#define MONO                    1
#define WAVE_HEADER_SIZE        44

typedef struct
{
    char RIFF_marker[4];
    uint32_t file_size;
    char filetype_header[4];
    char format_marker[4];
    uint32_t data_header_length;
    uint16_t format_type;
    uint16_t number_of_channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t bytes_per_frame;
    uint16_t bits_per_sample;
} WaveHeader;

class Audio
{
    public:
        Audio(uint32_t sampleRate = DEFAULT_SAMPLE_RATE, uint8_t bitDepth = DEFAULT_BITS_PER_SAMPLE, uint8_t channels = MONO);

        void format(uint32_t sampleRate, uint8_t channelCount, uint8_t sampleBitLength);
        void startRecord(char * audioFile, int fileSize, uint8_t durationInSeconds);
        char* getWav(int *fileSize);
        void stop();
        double getRecordedDuration();
        int getCurrentSize();

    private:
        uint32_t m_sample_rate;
        uint8_t m_bit_depth;
        uint8_t m_channels;
        uint8_t m_duration;

        char * m_wavFile;
        char * m_record_cursor;
        int m_file_size;
        int m_max_pcm_size;
        Ticker _ticker;

        WaveHeader * m_hdr;
        NAU88C10 m_codec;

        void readMic(void);
        WaveHeader *genericWAVHeader();
        int writeWAVHeader(char * wav, WaveHeader *hdr);
};

#endif
