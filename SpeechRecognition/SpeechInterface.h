#ifndef __SPEECH_INTERFACE_OS5_H__
#define __SPEECH_INTERFACE_OS5_H__

#include "mbed.h"
#include "http_request.h"


class SpeechInterface
{
    public:
        SpeechInterface(NetworkInterface * networkInterface, const char * subscriptionKey, bool debug = false);
        virtual ~SpeechInterface(void);

        int recognizeSpeech(char * audioFileBinary, int length, char * text, int textLen);
        int convertTextToSpeech(char * text, int length, char * audioFileBinary, int audioLen); 

    //private:
        int generateGuidStr(char * guidStr);
        int getJwtToken(char * token, int tokenLen);

        NetworkInterface * _wifi;
        char _cognitiveSubKey[33];
        bool _debug;
};

#endif
