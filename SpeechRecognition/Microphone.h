#ifndef __MICROPHONE_H__
#define __MICROPHONE_H__

#include "mbed.h"
#include <stdint.h>
#include <stdlib.h>

#define DURATION 2
#define DEFAULT_SAMPLE_RATE 8000
#define DEFAULT_BITS_PER_SAMPLE 8
#define MONO 1

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

class Microphone
{
    public:
        Microphone(PinName microphone, uint32_t duration = DURATION, uint32_t sample_rate = DEFAULT_SAMPLE_RATE, uint16_t bit_depth = DEFAULT_BITS_PER_SAMPLE, uint16_t channels = MONO);
        char* getWav(int *file_size);

    private:
        Ticker _ticker;
//        AnalogIn _microphone;
        uint32_t _duration;
        uint32_t _sample_rate;
        uint16_t _bit_depth;
        uint16_t _channels;
        WaveHeader *_hdr;
        char *_wavFile;
        char *_recording;
        uint32_t _file_size;
        void readMic();
        WaveHeader *genericWAVHeader();
        int writeWAVHeader(char * wav, WaveHeader *hdr);
};

#endif
