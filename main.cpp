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

int setupRealTime(void)
{
    NTPClient ntp(spwf);
    int result = 0;
    do {
        result = ntp.setTime("0.pool.ntp.org");
    } while (result != 0);
    return result;
}

const char CERT[] = 
"-----BEGIN CERTIFICATE-----\r\nMIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\nVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\nZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\nmD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\nmpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\ndc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\nBE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\nEpn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\nR9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n-----END CERTIFICATE-----\r\n";

HttpResponse* response;
int request(char *file, int file_size)
{

    /*
    HttpRequest* guidRequest = new HttpRequest(&spwf, HTTP_GET, "http://www.fileformat.info/tool/guid.htm?count=1&format=text");
    response = guidRequest->send();
    if (!response) {
        printf("HttpRequest failed\n");
        return 1;
    }
    string body = response->get_body();

    char *requestUri = (char *)malloc(300);
    sprintf(requestUri, "https://speech.platform.bing.com/recognize?scenarios=smd&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5&locale=en-us&device.os=bot\
&form=BCSSTT&version=3.0&format=json&instanceid=0E08849D-51AE-4C0E-81CD-21FE3A419868&requestid=%s-%s-%s-%s-%s", 
        body.substr(0, 8).c_str(), body.substr(8, 4).c_str(), body.substr(12, 4).c_str(), body.substr(16, 4).c_str(), body.substr(20, 12).c_str());

    printf("%s\r\n", requestUri);
    delete guidRequest;
    */

    SpeechInterface * speechInterface = new SpeechInterface(&spwf, "a4b1a40cb1f74ab1af757a0e700fa847", true);
    char * guid = (char *)malloc(33);
    speechInterface->generateGuidStr(guid);

    char *requestUri = (char *)malloc(300);
    sprintf(requestUri, "https://speech.platform.bing.com/recognize?scenarios=smd&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5&locale=en-us&device.os=bot\
&form=BCSSTT&version=3.0&format=json&instanceid=0E08849D-51AE-4C0E-81CD-21FE3A419868&requestid=%s", guid);
    printf("%s\r\n", requestUri);

    

    HttpsRequest* tokenRequest = new HttpsRequest(&spwf, CERT, HTTP_POST, "https://api.cognitive.microsoft.com/sts/v1.0/issueToken");
    tokenRequest->set_header("Ocp-Apim-Subscription-Key", "a4b1a40cb1f74ab1af757a0e700fa847");
    response = tokenRequest->send();
    if (!tokenRequest) {
        printf("HttpRequest failed (error code %d)\n", tokenRequest->get_error());
        return 1;
    }
    string body = response->get_body();
    printf("token<%s>\r\n", body.c_str());
    delete tokenRequest;

    HttpsRequest* speechRequest = new HttpsRequest(&spwf, CERT, HTTP_POST, requestUri);
    speechRequest->set_header("Authorization", "Bearer " + body);
    speechRequest->set_header("Content-Type", "plain/text");
    response = speechRequest->send(file, file_size);
    body = response->get_body();
    printf("congnitive result <%s>\r\n", body.c_str());
    delete speechRequest;
    SASToken iothubtoken;
    do {
        setupRealTime();
    } while(strlen(iothubtoken.getValue(time(NULL))) == 0);
    sprintf(requestUri, "https://%s/devices/%s/messages/events?api-version=2016-11-14", IOTHUB_HOST, DEVICE_ID);
    printf("<%s>\r\n", requestUri);
    HttpsRequest* iotRequest = new HttpsRequest(&spwf, CERT, HTTP_POST, requestUri);
    iotRequest->set_header("Authorization", iothubtoken.getValue(time(NULL)));
    response = iotRequest->send(body.c_str(), body.length());
    body = response->get_body();
    printf("iot hub result <%s>\r\n", body.c_str());
    delete iotRequest;
}

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

    
    int file_size;
    char *file;
    DigitalIn button(USER_BUTTON);
    button.mode(PullUp);
    int old_pb = 1, new_pb;
    while(1) {
        new_pb = button.read();
        if(new_pb == 1 && old_pb == 0) {
            printf("Start record...\r\n");
            file = microphone.getWav(&file_size);
            
            for (int i = 0; i < 45 + file_size; ++i) {
                if (i % 50 == 0) printf("\n");
                printf("%d,", file[i]);
            }

            printf("//%d\r\n", file_size);
            request(file, file_size);
        }
        old_pb = new_pb;
    }
    
    return 0;
}
