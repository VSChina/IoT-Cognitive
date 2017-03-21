#ifndef __SPEECH_INTERFACE_OS5_H__
#define __SPEECH_INTERFACE_OS5_H__

#include "mbed.h"
#include "http_request.h"

typedef struct
{
    char * status;  //success or error
    char * text;
    float confidence;
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
        string getJwtToken();
        int setupRealTime(void);
        int sentToIotHub(char * file, int length);

        NetworkInterface * _wifi;
        char _cognitiveSubKey[33];
        char _deviceId[37];

        char * requestUri;
        HttpResponse* _response;
        bool _debug;
};

#endif
