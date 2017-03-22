#include "mbed.h"
#include "NTPClient.h"
#include "SPWFSAInterface5.h"
#include "https_request.h"
#include "http_request.h"
#include "SASToken.h"
#include "Microphone.h"
#include "SpeechInterface.h"
//#include "picojson.h"

Microphone microphone(A0);
Serial pc(USBTX, USBRX, 115200);
SPWFSAInterface5 spwf(D8, D2, false);

const char* ssid = "Ruff_R0101965"; // Ruff_R0101965, laptop_jiaqi
const char* pwd = "Password01!";
const char* subscriptionKey = "a4b1a40cb1f74ab1af757a0e700fa847";
const char* deviceId = "0E08849D-51AE-4C0E-81CD-21FE3A419868";

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
void parseSpeechResponse() {
    picojson::value v;
    const char *jsonsoure = "{\"version\":\"3.0\",\"header\":{\"status\":\"success\",\"scenario\":\"smd\",\"name\":\"Close the window.\",\"lexical\":\"close the window\",\"properties\":{\"requestid\":\"96d8c7a2-18d7-4d0c-a49c-4f08a75373c7\",\"MIDCONF\":\"1\"}},\"results\":[{\"scenario\":\"smd\",\"name\":\"Close the window.\",\"lexical\":\"close the window\",\"confidence\":0.8521699,\"properties\":{\"MIDCONF\":\"1\"}}]}";
    char * json = (char*) malloc(strlen(jsonsoure)+1);
    strcpy(json, jsonsoure);
    string err = picojson::parse(v, json, json + strlen(json));
    if (err != "")
    {
        printf("res error = %s\r\n", err.c_str());
    }
    
    SpeechResponse* response;
    response->status = (char *)v.get("header").get("status").get<string>().c_str();
    //printf("status = %s\r\n", v.get("header").get("status").get<string>().c_str());

    picojson::array results = v.get("results").get<picojson::array>();
    picojson::array::iterator iter = results.begin();  
    response->text = (char *)(*iter).get("name").get<string>().c_str();
    response->confidence = (double)(*iter).get("confidence").get<double>();

    printf("status = %s\r\n", response->status);
    printf("speech text = %s\r\n", response->text);
    printf("confidence = %f\r\n", response->confidence);
}
*/

int main(void)
{
    //parseSpeechResponse();

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
            
            SpeechResponse* speechResponse = speechInterface->recognizeSpeech(file, file_size);
        }
        old_pb = new_pb;
    }

    free(file);
    return 0;
}
