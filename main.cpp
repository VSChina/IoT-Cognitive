#include "Microphone.h"
#include "SPWFSAInterface5.h"
#include "SpeechInterface.h"

Microphone microphone(A0);
Serial pc(USBTX, USBRX, 115200);
SPWFSAInterface5 spwf(D8, D2, false);

const char* ssid = "Ruff_R0101965"; // Ruff_R0101965, laptop_jiaqi
const char* pwd = "Password01!";
const char* subscriptionKey = "a4b1a40cb1f74ab1af757a0e700fa847";
const char* deviceId = "0E08849D-51AE-4C0E-81CD-21FE3A419868";

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
int main(void)
{
    printf("Start...\n");
    while(true) {
        printf("Try to connect %s ...\n", ssid);
        if(spwf.connect(ssid, pwd, NSAPI_SECURITY_WPA2) == NSAPI_ERROR_OK) {
            printf("connected %s\r\n", spwf.get_mac_address());
            break;
        }
        else {
            printf("failed.\r\n");
        }
    }

    SpeechInterface * speechInterface = new SpeechInterface(&spwf, subscriptionKey, deviceId, true);
    
    int audio_size;
    char * audio_file;
    DigitalIn button(USER_BUTTON);
    int old_pb = 0, new_pb;
    while(1) {
        new_pb = button.read();
        if(new_pb == 0 && old_pb == 1) {
            printf("start record...\r\n");
            audio_file = microphone.getWav(&audio_size);
            
            /*
            for (int i = 0; i < audio_size; ++i) {
                if (i % 50 == 0) printf("\n");
                printf("%d,", audio_file[i]);
            }
            printf("\r\n");
            */
            printf("//%d\r\n", audio_size);
            
            SpeechResponse* speechResponse = speechInterface->recognizeSpeech(audio_file, audio_size);
            if (speechResponse != NULL)
            {
                printf("Speech recognition result:\r\n");
                printf("status = %s\r\n", speechResponse->status);
                printf("speech text = %s\r\n", speechResponse->text);
                printf("confidence = %f\r\n", speechResponse->confidence);
            }           
        }
        old_pb = new_pb;
    }

    free(audio_file);
    return 0;
}
