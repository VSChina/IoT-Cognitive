#ifndef __VOICE_UTILITY_H__
#define __VOICE_UTILITY_H__

#include "mbed.h"

#define SAMPLE_RATE 16000
#define BITS_PER_SAMPLE 16

class VoiceUtility
{
    public:
        char * getWav();
        char * readMic(AnalogIn microphone);

    private:

};

#endif
