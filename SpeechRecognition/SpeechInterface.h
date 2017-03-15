#ifndef __SPEECH_INTERFACE_OS5_H__
#define __SPEECH_INTERFACE_OS5_H__

#include "mbed.h"
#include "http_request.h"

class SpeechInterface
{
    public:
        SpeechInterface(NetworkInterface * networkInterface, char * subId);
        virtual ~SpeechInterface();

        char * recognizeSpeech(char * audioFileBinary, int length);
        char * convertTextToSpeech(char * text, int length); 

    private:
        char * generateGuidStr();
        char * getJwtToken(char * subId);

        NetworkInterface * netwowrkInterface;
        char cognitive_sub_id[32];
        bool _debug;
};

#endif
