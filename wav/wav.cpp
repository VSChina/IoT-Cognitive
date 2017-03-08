#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wav.h"

WaveHeader *genericWAVHeader(uint32_t sample_rate, uint16_t bit_depth, uint16_t channels)
{
    WaveHeader *hdr;
    hdr = (WaveHeader *)malloc(sizeof(*hdr));
    if (!hdr) return NULL;

    memcpy(&hdr->RIFF_marker, "RIFF", 4);
    memcpy(&hdr->filetype_header, "WAVE", 4);
    memcpy(&hdr->format_marker, "fmt ", 4);
    hdr->data_header_length = 16;
    hdr->format_type = 1;
    hdr->number_of_channels = channels;
    hdr->sample_rate = sample_rate;
    hdr->bytes_per_second = sample_rate * channels * bit_depth / 8;
    hdr->bytes_per_frame = channels * bit_depth / 8;
    hdr->bits_per_sample = bit_depth;

    return hdr;
}

int writeWAVHeader(char * wav, WaveHeader *hdr)
{
    if (!hdr)
        return -1;

    memcpy(wav, &hdr->RIFF_marker, 4);
    memcpy(wav + 4, &hdr->file_size, 4);
    memcpy(wav + 8, &hdr->filetype_header, 4);
    memcpy(wav + 12, &hdr->format_marker, 4);
    memcpy(wav + 16, &hdr->data_header_length, 4);
    memcpy(wav + 20, &hdr->format_type, 2);
    memcpy(wav + 22, &hdr->number_of_channels, 2);
    memcpy(wav + 24, &hdr->sample_rate, 4);
    memcpy(wav + 28, &hdr->bytes_per_second, 4);
    memcpy(wav + 32, &hdr->bytes_per_frame, 2);
    memcpy(wav + 34, &hdr->bits_per_sample, 2);
    memcpy(wav + 36, "data", 4);
    uint32_t data_size = hdr->file_size - 36;
    memcpy(wav + 40, &data_size, 4);

    return 0;
}

char* recordWAV(WaveHeader *hdr, uint32_t duration, int *file_size)
{
    *file_size = hdr->sample_rate * hdr->bytes_per_frame * duration;
    hdr->file_size = *file_size + 36;
    char *wav = (char *)malloc(*file_size + 45);
    writeWAVHeader(wav, hdr);
    return wav;
}