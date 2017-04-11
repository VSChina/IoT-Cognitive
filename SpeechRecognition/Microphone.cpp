#include "Microphone.h"

void Microphone::readMic() {
    if (_recording >= _wavFile + _file_size) return;
    int x = 0; //_microphone.read_u16();
    int y = x & 255;
    x >>= 8;
    if (_bit_depth == 16) {
        memcpy(_recording, &y, 1);
        _recording++;
    }
    memcpy(_recording, &x, 1);
    _recording++;
}

Microphone::Microphone(PinName microphone, uint32_t duration, uint32_t sample_rate, uint16_t bit_depth, uint16_t channels) 
//    : _microphone(microphone)
{
    _ticker.attach(callback(this, &Microphone::readMic), 1.0 / sample_rate);
    _duration = duration;
    _sample_rate = sample_rate;
    _bit_depth = bit_depth;
    _channels = channels;
    _hdr = genericWAVHeader();
    _file_size = sample_rate * _hdr->bytes_per_frame * duration + 44;
    _wavFile = (char *)malloc(_file_size + 1);
    _wavFile[_file_size] = 0x0;
    writeWAVHeader(_wavFile, _hdr);
    _recording = _wavFile + _file_size;
}

WaveHeader* Microphone::genericWAVHeader()
{
    WaveHeader *hdr;
    hdr = (WaveHeader *)malloc(sizeof(*hdr));
    if (!hdr) return NULL;

    memcpy(&hdr->RIFF_marker, "RIFF", 4);
    memcpy(&hdr->filetype_header, "WAVE", 4);
    memcpy(&hdr->format_marker, "fmt ", 4);
    hdr->data_header_length = 16;
    hdr->format_type = 1;
    hdr->number_of_channels = _channels;
    hdr->sample_rate = _sample_rate;
    hdr->bytes_per_second = _sample_rate * _channels * _bit_depth / 8;
    hdr->bytes_per_frame = _channels * _bit_depth / 8;
    hdr->bits_per_sample = _bit_depth;
    hdr->file_size = hdr->sample_rate * hdr->bytes_per_frame * _duration + 36;

    return hdr;
}

int Microphone::writeWAVHeader(char * wav, WaveHeader *hdr)
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

void Microphone::startRecord() {
    if (_wavFile == NULL) return;
    _recording = _wavFile + 44;
}

char* Microphone::getWav(int *file_size) {
    *file_size  = (int)_file_size;
    return _wavFile;
}