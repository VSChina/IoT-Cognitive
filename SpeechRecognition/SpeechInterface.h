#ifndef __SPEECH_INTERFACE_OS5_H__
#define __SPEECH_INTERFACE_OS5_H__

#include "mbed.h"
#include "WString.h"

typedef struct
{
    char * status;  //success or error
    char * text;
    double confidence;
}SpeechResponse;


class SpeechInterface
{
    public:
        SpeechInterface(NetworkInterface * networkInterface, const char * subscriptionKey, const char * deviceId, bool debug = false);
        virtual ~SpeechInterface(void);

        SpeechResponse* recognizeSpeech(char * audioFileBinary, int length);
        int convertTextToSpeech(char * text, int length, char * audioFileBinary, int audioLen); 

    private:
        int generateGuidStr(char * guidStr);
        char* getJwtToken();
        int setupRealTime(void);

        NetworkInterface * _wifi;
        char* _cognitiveSubKey;
        char* _deviceId;

        char * _requestUri;
        bool _debug;
};

#endif
