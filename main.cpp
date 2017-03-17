#include "mbed.h"
#include "NTPClient.h"
#include "SPWFSAInterface5.h"
#include "https_request.h"
#include "http_request.h"
#include "SASToken.h"
#include "Microphone.h"
#include "SpeechInterface.h"

Microphone microphone(A0);
Serial pc(USBTX, USBRX, 115200);
SPWFSAInterface5 spwf(D8, D2, false);
const char *ssid = "Ruff_R0101965"; // Ruff_R0101965, laptop_jiaqi
const char *pwd = "Password01!";

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

    SpeechInterface * speechInterface = new SpeechInterface(&spwf, "a4b1a40cb1f74ab1af757a0e700fa847", true);
    
    int file_size;
    char * file;
    DigitalIn button(USER_BUTTON);
    int old_pb = 0, new_pb;
    while(1) {
        new_pb = button.read();
        if(new_pb == 0 && old_pb == 1) {
            printf("start record...\r\n");
            file = microphone.getWav(&file_size);
            
            /*
            for (int i = 0; i < file_size; ++i) {
                if (i % 50 == 0) printf("\n");
                printf("%d,", file[i]);
            }
            printf("\r\n");
            */
            printf("//%d\r\n", file_size);
            
            SpeechResponse* response = speechInterface->recognizeSpeech(file, file_size);
        }
        old_pb = new_pb;
    }

    return 0;
}
